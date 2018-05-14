/**
 * Hyperion
 * dehaze.cpp
 * Purpose: Contains the functions required for the dehazing process
 *
 * @author: Evangelos Mavropoulos <evmavrop@gmail.com>
 * @date: 12/8/2016
 */

#include <dehaze.hpp>
#include <iostream>

/**
 * This constructor's values are used for testing.
 */
Dehazing::Dehazing() {

}

Dehazing::~Dehazing() {

}

template<class T> inline T clipping255(const T a) {
	return a < 255 ? a : 255;
}

template<class T> inline T clipping0(const T a) {
	return a > 0 ? a : 0;
}

/**
 * @brief Locates the brightest pixels
 * Returns the positions of the pixel whose sum of RGB values is the highest amongst the pixels corresponding
 * to the 0.1% of the brightest pixels.
 *
 * @param inpImage
 * @param minRGBArray the minimum of the RGB values of every pixel in a descending order
 * @param size
 * @return position
 */
int Dehazing::brightestPix(Image &inpImage, unsigned int **minRGBArray) {
	int size = inpImage.RGBrows * inpImage.RGBcols;
	int brightestPixelsNum = (int) (0.001 * (float) size) + 1;
	int position, pointer;
	unsigned int *top = new unsigned int[brightestPixelsNum];

	for (int i = 0; i < brightestPixelsNum; i++) {
		pointer = minRGBArray[1][i];
		top[i] = inpImage.R[pointer] + inpImage.G[pointer] + inpImage.B[pointer];
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
 * @brief Locates the minimum of the RGB values
 * Creates a array containing the minimum of the RGB values for every pixel and the position
 * of the pixel in the image.
 *
 * @param inpImage Input image
 * @param minRGBArray Array containing the min of every pixels RGB values for every pixel and the position
 * of the pixel in the image.
 */
void Dehazing::minRGB(Image &inpImage, unsigned int **minRGBArray) {
	int size = inpImage.RGBrows * inpImage.RGBcols;
	unsigned char *R, *G, *B;
	R = inpImage.R;
	G = inpImage.G;
	B = inpImage.B;

	for (int i = 0; i < size; i++) {
		minRGBArray[0][i] = min(min(G[i], B[i]), R[i]);
		minRGBArray[1][i] = i;
	}
}

/**
 * @brief Sorts the minRGBArray
 * Sorts the minRGBArray in order to speedup the process.
 *
 * @param minRGBArray Array containing the min of every pixels RGB values for every pixel and the position
 * of the pixel in the image.
 * @param left
 * @param right
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
 * @brief Medium transmission function
 * Calculates the medium transmission function for every pixel in the image.
 *
 * @param inpImage Input Image
 * @param position Airlight position
 * @return t
 */
float *Dehazing::findMediumTransmission(Image &inpImage, int position, InputParameters &inpParam) {
	float *t = new float[inpImage.RGBcols * inpImage.RGBrows];
	float omega = 0.8;
	int frmCols, frmRows, blockSizeX, blockSizeY, counter, startX, startY, endX, endY, pos;
	frmCols = inpImage.RGBcols;
	frmRows = inpImage.RGBrows;
	blockSizeX = inpParam.blockSizeX;
	blockSizeY = inpParam.blockSizeY;
	endX = static_cast<int> (static_cast<float> (blockSizeX) / 2) + 0.5;
	endY = static_cast<int> (static_cast<float> (blockSizeY) / 2) + 0.5;
	startX = -blockSizeX / 2;
	startY = -blockSizeY / 2;
	int A[3];
	A[0] = inpImage.R[position];
	A[1] = inpImage.G[position];
	A[2] = inpImage.B[position];

	Image paddedImage(inpImage, inpParam.blockSizeY, inpParam.blockSizeX);
	int paddedFrameCols = paddedImage.RGBcols;
	pos = 0;
	unsigned char min;

	counter = blockSizeX + blockSizeY * paddedFrameCols;

	for (int i = 0; i < frmRows; i++) {
		for (int j = 0; j < frmCols; j++) {
			min = 255;

			for (int l = startY; l <= endY; l++) {
				for (int m = startX; m <= endX; m++) {
					if (min > paddedImage.R[counter + m + l * paddedImage.RGBcols]) {
						min = paddedImage.R[counter + m + l * paddedImage.RGBcols];
						pos = 0;
					}
				}
			}

			for (int l = startY; l <= endY; l++) {
				for (int m = startX; m <= endX; m++) {
					if (min > paddedImage.G[counter + m + l * paddedImage.RGBcols]) {
						min = paddedImage.G[counter + m + l * paddedImage.RGBcols];
						pos = 1;
					}
				}
			}

			for (int l = startY; l <= endY; l++) {
				for (int m = startX; m <= endX; m++) {
					if (min > paddedImage.B[counter + m + l * paddedImage.RGBcols]) {
						min = paddedImage.B[counter + m + l * paddedImage.RGBcols];
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
 * @brief Airlight
 * Finds the position of the pixel that it is used as the Airlight.
 *
 * @param inpImage Input image
 * @return position Position of the pixel used as Airlight
 */
int Dehazing::findAirlight(Image &inpImage) {
	int size = inpImage.RGBrows * inpImage.RGBcols;
	unsigned int **minRGBArray = new unsigned int*[2];
	minRGBArray[0] = new unsigned int[size];
	minRGBArray[1] = new unsigned int[size];

	minRGB(inpImage, minRGBArray);

	quicksort(minRGBArray, 0, size - 1);

	int position = brightestPix(inpImage, minRGBArray);

	delete[] minRGBArray[0];
	delete[] minRGBArray[1];
	delete[] minRGBArray;

	return position;
}

/**
 * Calculates the negative of the RGB image.
 *
 * @param inpImage Input Image
 */
void Dehazing::negative(Image &inpImage) {
	int size = inpImage.RGBcols * inpImage.RGBrows;

	for (int i = 0; i < size; i++) {
		inpImage.R[i] = 255 - inpImage.R[i];
		inpImage.G[i] = 255 - inpImage.G[i];
		inpImage.B[i] = 255 - inpImage.B[i];
	}
}

/**
 * Applies the dehazing process on the image contained in the object.
 *
 * @param inpParam Parameters of the dehazing process
 * @param inpImage Input image
 * @param outpImage Output image
 */
void Dehazing::dehazeProc(InputParameters &inpParam, Image &inpImage, Image &outpImage) {
	int size = inpImage.RGBcols * inpImage.RGBrows;
	float *t, P = 0;
	int A[3];

	if (inpParam.lowLight == 1) {
		negative(inpImage);
	}

	int position = findAirlight(inpImage);

	A[0] = inpImage.R[position];
	A[1] = inpImage.G[position];
	A[2] = inpImage.B[position];

	t = findMediumTransmission(inpImage, position, inpParam);

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
			outpImage.R[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.R[i] - A[0]) / P + A[0])));
			outpImage.G[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.G[i] - A[1]) / P + A[1])));
			outpImage.B[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.B[i] - A[2]) / P + A[2])));
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			outpImage.R[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.R[i] - A[0]) / t[i] + A[0])));
			outpImage.G[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.G[i] - A[1]) / t[i] + A[1])));
			outpImage.B[i] = static_cast<unsigned char> (clipping0(
					clipping255((inpImage.B[i] - A[2]) / t[i] + A[2])));
		}
	}
	delete[] t;

}

