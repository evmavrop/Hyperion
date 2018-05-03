/**
 * Hyperion
 * utilities.cpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 14/8/2016
 */

#include <utilities.hpp>

/**
 * Copies the input image in a container of type Mat.
 *
 * @param image
 * @param imageMat
 */
void utilities::fromImageToMat(Mat &imageMat, Image &image) {
	int rows = imageMat.rows;
	int cols = imageMat.cols;
	int channels = imageMat.channels();
	int size = rows * cols;

	int realSize = size * channels;
	unsigned char *R = image.R;
	unsigned char *G = image.G;
	unsigned char *B = image.B;
	unsigned char *temp = imageMat.data;

	for (int i = 0, j = 0; i < realSize - 2; i += 3, j++) {
		temp[i] = B[j];
		temp[i + 1] = G[j];
		temp[i + 2] = R[j];
	}

}

