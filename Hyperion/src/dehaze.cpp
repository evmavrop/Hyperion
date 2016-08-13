/**
 * Hyperion
 * dehaze.cpp
 * Purpose: Contains the functions required for the dehazing process
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 12/8/2016
 */

#include <dehaze.hpp>
#include <iostream>

Dehazing::Dehazing(Mat frameMat) {
	int rows = this->RGBrows = frameMat.rows;
	int cols = this->RGBcols = frameMat.cols;
	int channels = frameMat.channels();
	int size = rows * cols;
	int realSize = size * channels;

	this->R = new unsigned char[size]();
	this->G = new unsigned char[size]();
	this->B = new unsigned char[size]();

	unsigned char *R = this->R;
	unsigned char *G = this->G;
	unsigned char *B = this->B;
	unsigned char *temp = static_cast<unsigned char*>(frameMat.data);

	for (int i = 0, j = 0; i < realSize - 2; i += 3, j++) {
		B[j] = temp[i];
		G[j] = temp[i + 1];
		R[j] = temp[i + 2];
	}
	this->paddedR = this->paddedG = this->paddedB = 0;
	paddedRGBrows = paddedRGBcols = 0;

}

Dehazing::~Dehazing() {
	delete[] this->R;
	delete[] this->G;
	delete[] this->B;
	delete[] this->paddedR;
	delete[] this->paddedG;
	delete[] this->paddedB;
}

template<class T> inline T clipping255(const T a) {
	return a < 255 ? a : 255;
}

template<class T> inline T clipping0(const T a) {
	return a > 0 ? a : 0;
}

/**
 * Generate space around the image with size relative to the block size.
 *
 * @param[in] inpParam
 */
void Dehazing::padding(InputParameters &inpParam) {
	this->paddedRGBrows = this->RGBrows + 2 * inpParam.blockSizeY;
	this->paddedRGBcols = this->RGBcols + 2 * inpParam.blockSizeX;
	int paddedSize = this->paddedRGBrows * this->paddedRGBcols;
	int frmRows = this->RGBrows;
	int frmCols = this->RGBcols;

	this->paddedR = new unsigned char[paddedSize]();
	this->paddedG = new unsigned char[paddedSize]();
	this->paddedB = new unsigned char[paddedSize]();

	fill_n(this->paddedR, paddedSize, 255);
	fill_n(this->paddedG, paddedSize, 255);
	fill_n(this->paddedB, paddedSize, 255);

	unsigned char *R, *G, *B, *pR, *pG, *pB;
	R = this->R;
	G = this->G;
	B = this->B;
	pR = this->paddedR;
	pG = this->paddedG;
	pB = this->paddedB;
	int counter = inpParam.blockSizeY * this->paddedRGBcols + inpParam.blockSizeX;
	int row, column;

	for (int i = 0; i < frmRows; i++) {
		row = i * frmCols;
		for (int j = 0; j < frmCols; j++) {
			column = row + j;
			pR[counter] = R[column];
			pG[counter] = G[column];
			pB[counter] = B[column];
			counter++;
		}
		counter += inpParam.blockSizeX + inpParam.blockSizeX;
	}
}

/**
 * Returns the positions of the pixel whose sum of RGB values is the highest amongst the pixels corresponding
 * to the 0.1% of the brightest pixels.
 *
 * @param[in] minRGBArray the minimum of the RGB values of every pixel in a descending order
 * @param[in] size
 * @return position
 */
int Dehazing::brightestPix(unsigned int **minRGBArray, int size) {
	int brightestPixelsNum = (int) (0.001 * (float) size) + 1;
	int position, pointer;
	unsigned int *top = new unsigned int[brightestPixelsNum];

	for (int i = 0; i < brightestPixelsNum; i++) {
		pointer = minRGBArray[1][i];
		top[i] = this->R[pointer] + this->G[pointer] + this->B[pointer];
	}

	unsigned int max = 0;
	int pos = 0;

	for (int i = 0; i < brightestPixelsNum; i++) {
		if (top[i] > max) {
			max = top[i];
			pos = minRGBArray[1][i];
		}
	}

	delete[] top;
	position = pos;
	return position;
}

/**
 * Creates a array containing the minimum of the RGB values for every pixel and the position
 * of the pixel in the image.
 *
 * @param[in] size
 * @param[out] minRGBArray
 */
void Dehazing::minRGB(unsigned int **minRGBArray, int size) {

	unsigned char *R, *G, *B;
	R = this->R;
	G = this->G;
	B = this->B;

	for (int i = 0; i < size; i++) {
		minRGBArray[0][i] = min(min(G[i], B[i]), R[i]);
		minRGBArray[1][i] = i;
	}
}

/**
 * Sorts the minRGBArray in order to speedup the process.
 *
 * @param[in,out] minRGBArray
 * @param[in] left
 * @param[in] right
 */
void Dehazing::quicksort(unsigned int **minRGBArray, int left, int right) {
	int i = left, j = right;
	int tmp;
	unsigned int pivot = minRGBArray[0][(left + right) / 2];

	while (i <= j) {
		while (minRGBArray[0][i] > pivot)
			i++;
		while (minRGBArray[0][j] < pivot)
			j--;

		if (i <= j) {
			tmp = minRGBArray[0][i];
			minRGBArray[0][i] = minRGBArray[0][j];
			minRGBArray[0][j] = tmp;
			tmp = minRGBArray[1][i];
			minRGBArray[1][i] = minRGBArray[1][j];
			minRGBArray[1][j] = tmp;
			i++;
			j--;
		}
	}

	if (left < j) {
		quicksort(minRGBArray, left, j);
	}
	if (i < right) {
		quicksort(minRGBArray, i, right);
	}
}

/**
 * Calculates the medium transmission function for every pixel in the image.
 *
 * @param[in] position
 * @param[in] size
 * @return t
 */
float *Dehazing::findMediumTransmission(int position, int size, InputParameters &inpParam) {
	float *t = new float[this->RGBcols * this->RGBrows];
	float omega = 0.8;
	int frmCols, frmRows, blockSizeX, blockSizeY, counter, startX, startY, endX, endY, pos;
	frmCols = this->RGBcols;
	frmRows = this->RGBrows;
	blockSizeX = inpParam.blockSizeX;
	blockSizeY = inpParam.blockSizeY;
	endX = (int) ((float) blockSizeX / 2) + 0.5;
	endY = (int) ((float) blockSizeY / 2) + 0.5;
	startX = -(int) (blockSizeX / 2);
	startY = -(int) (blockSizeY / 2);
	int A[3];
	A[0] = this->R[position];
	A[1] = this->G[position];
	A[2] = this->B[position];

	padding(inpParam);
	int paddedFrameCols = this->paddedRGBcols;
	pos = 0;
	unsigned char min;

	counter = blockSizeX + blockSizeY * paddedFrameCols;

	for (int i = 0; i < frmRows; i++) {
		for (int j = 0; j < frmCols; j++) {
			min = 255;

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > this->paddedR[counter + m + l * paddedFrameCols]) {
						min = this->paddedR[counter + m + l * paddedFrameCols];
						pos = 0;
					}
				}
			}

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > this->paddedG[counter + m + l * paddedFrameCols]) {
						min = this->paddedG[counter + m + l * paddedFrameCols];
						pos = 1;
					}
				}
			}

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > this->paddedB[counter + m + l * paddedFrameCols]) {
						min = this->paddedB[counter + m + l * paddedFrameCols];
						pos = 2;
					}
				}
			}

			// FIXME Zero value
			t[i * frmCols + j] = 1.0 - omega * ((float) min / (float) A[pos]);
			counter++;
		}
		counter += blockSizeX + blockSizeX;
	}

	return t;
}

/**
 * Finds the position of the pixel that it is used as the Airlight.
 *
 * @param[in] size
 * @return position
 */
int Dehazing::findAirlight(int size) {
	unsigned int **minRGBArray = new unsigned int*[2];
	minRGBArray[0] = new unsigned int[size];
	minRGBArray[1] = new unsigned int[size];

	minRGB(minRGBArray, size);

	quicksort(minRGBArray, 0, size - 1);

	int position = brightestPix(minRGBArray, size);

	delete[] minRGBArray[0];
	delete[] minRGBArray[1];
	delete[] minRGBArray;

	return position;
}

/**
 * Applies the dehazing process on the image contained in the object.
 *
 * @param[in] inpParam
 */
void Dehazing::dehazeProc(InputParameters &inpParam) {
	int size = this->RGBcols * this->RGBrows;
	float *t, P;
	int A[3];

	int position = findAirlight(size);

	A[0] = this->R[position];
	A[1] = this->G[position];
	A[2] = this->B[position];

	t = findMediumTransmission(position, size, inpParam);

	if (inpParam.boosting == 1) {
		for (int i = 0; i < size; i++) {
			if (t[i] > 1) {
				t[i] = 1;
			}
			else if (t[i] <= 0.859734) {
				P = 2.0 * t[i] * t[i];
			}
			else {
				P = -2 * t[i] * t[i] * t[i] + 8 * t[i] - 3;
			}
			this->R[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->R[i] - A[0]) / P + A[0])));
			this->G[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->G[i] - A[1]) / P + A[1])));
			this->B[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->B[i] - A[2]) / P + A[2])));
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			this->R[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->R[i] - A[0]) / t[i] + A[0])));
			this->G[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->G[i] - A[1]) / t[i] + A[1])));
			this->B[i] = static_cast<unsigned char>(clipping0(
					clipping255((this->B[i] - A[2]) / t[i] + A[2])));
		}
	}
	delete[] t;
}
