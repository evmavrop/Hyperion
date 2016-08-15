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

	unsigned char *temp = static_cast<unsigned char*>(frameMat.data);

	vector<unsigned char>&iR = this->R;
	vector<unsigned char>&iG = this->G;
	vector<unsigned char>&iB = this->B;

	iR.resize(size);
	iG.resize(size);
	iB.resize(size);

	for (int i = 0, j = 0; i < realSize - 2; i += 3, j++) {
		iB[j] = temp[i];
		iG[j] = temp[i + 1];
		iR[j] = temp[i + 2];
	}
	paddedRGBrows = paddedRGBcols = 0;

}

/**
 * This constructor's values are used for testing.
 */
Dehazing::Dehazing() {
	this->RGBrows = 9;
	this->RGBcols = 9;

	int size = this->RGBrows * this->RGBcols;

	vector<unsigned char>&iR = this->R;
	vector<unsigned char>&iG = this->G;
	vector<unsigned char>&iB = this->B;

	iR.resize(size);
	iG.resize(size);
	iB.resize(size);

	for (int i = 0; i < size; i++) {
		iR[i] = i;
		iG[i] = i;
		iB[i] = i;
	}
	paddedRGBrows = paddedRGBcols = 0;
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

	this->paddedR.resize(paddedSize, 255);
	this->paddedG.resize(paddedSize, 255);
	this->paddedB.resize(paddedSize, 255);

	vector<unsigned char>&iR = this->R;
	vector<unsigned char>&iG = this->G;
	vector<unsigned char>&iB = this->B;

	vector<unsigned char>&pR = this->paddedR;
	vector<unsigned char>&pG = this->paddedG;
	vector<unsigned char>&pB = this->paddedB;

	int counter = inpParam.blockSizeY * this->paddedRGBcols + inpParam.blockSizeX;
	int row, column;

	for (int i = 0; i < frmRows; i++) {
		row = i * frmCols;
		for (int j = 0; j < frmCols; j++) {
			column = row + j;
			pR[counter] = iR[column];
			pG[counter] = iG[column];
			pB[counter] = iB[column];
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
int Dehazing::brightestPix(vector<vector<unsigned int> >&minRGBArray, int size) {
	int brightestPixelsNum = (int) (0.001 * (float) size) + 1;
	int position, pointer;
	vector<unsigned int> top(brightestPixelsNum);

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
void Dehazing::minRGB(vector<vector<unsigned int> >&minRGBArray, int size) {

	vector<unsigned char>&iR = this->R;
	vector<unsigned char>&iG = this->G;
	vector<unsigned char>&iB = this->B;

	for (int i = 0; i < size; i++) {
		minRGBArray[0][i] = min(min(iG[i], iB[i]), iR[i]);
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
void Dehazing::quicksort(vector<vector<unsigned int> >&minRGBArray, int left, int right) {
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
	endX = static_cast<int>(static_cast<float>(blockSizeX) / 2) + 0.5;
	endY = static_cast<int>(static_cast<float>(blockSizeY) / 2) + 0.5;
	startX = -blockSizeX / 2;
	startY = -blockSizeY / 2;
	int A[3];
	A[0] = this->R[position];
	A[1] = this->G[position];
	A[2] = this->B[position];

	padding(inpParam);
	int paddedFrameCols = this->paddedRGBcols;
	pos = 0;
	unsigned char min;

	counter = blockSizeX + blockSizeY * paddedFrameCols;

	vector<unsigned char>&pR = this->paddedR;
	vector<unsigned char>&pG = this->paddedG;
	vector<unsigned char>&pB = this->paddedB;

	for (int i = 0; i < frmRows; i++) {
		for (int j = 0; j < frmCols; j++) {
			min = 255;

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > pR[counter + m + l * paddedFrameCols]) {
						min = pR[counter + m + l * paddedFrameCols];
						pos = 0;
					}
				}
			}

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > pG[counter + m + l * paddedFrameCols]) {
						min = pG[counter + m + l * paddedFrameCols];
						pos = 1;
					}
				}
			}

			for (int l = startY; l < endY; l++) {
				for (int m = startX; m < endX; m++) {
					if (min > pB[counter + m + l * paddedFrameCols]) {
						min = pB[counter + m + l * paddedFrameCols];
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
	vector<vector<unsigned int> > minRGBArray(2, vector<unsigned int>(size));

	minRGB(minRGBArray, size);

	quicksort(minRGBArray, 0, size - 1);

	int position = brightestPix(minRGBArray, size);

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

	vector<unsigned char>&iR = this->R;
	vector<unsigned char>&iG = this->G;
	vector<unsigned char>&iB = this->B;

	int position = findAirlight(size);

	A[0] = iR[position];
	A[1] = iG[position];
	A[2] = iB[position];

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
				P = -2.0 * t[i] * t[i] * t[i] + 8.0 * t[i] - 3.0;
			}
			iR[i] = static_cast<unsigned char>(clipping0(clipping255((iR[i] - A[0]) / P + A[0])));
			iG[i] = static_cast<unsigned char>(clipping0(clipping255((iG[i] - A[1]) / P + A[1])));
			iB[i] = static_cast<unsigned char>(clipping0(clipping255((iB[i] - A[2]) / P + A[2])));
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			iR[i] =
					static_cast<unsigned char>(clipping0(clipping255((iR[i] - A[0]) / t[i] + A[0])));
			iG[i] =
					static_cast<unsigned char>(clipping0(clipping255((iG[i] - A[1]) / t[i] + A[1])));
			iB[i] =
					static_cast<unsigned char>(clipping0(clipping255((iB[i] - A[2]) / t[i] + A[2])));
		}
	}
	delete[] t;
}
