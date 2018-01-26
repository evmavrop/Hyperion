/**
 * Hyperion
 * frame.cpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */
#include <image.hpp>

Image::Image() {
	this->RGBrows = 0;
	this->RGBcols = 0;
	this->channels = 0;

	this->R = nullptr;
	this->G = nullptr;
	this->B = nullptr;

}

Image::Image(const Mat& imageMat) {
	int rows = this->RGBrows = imageMat.rows;
	int cols = this->RGBcols = imageMat.cols;
	int channels = this->channels = imageMat.channels();
	int size = rows * cols;
	int realSize = size * channels;

	this->R = new unsigned char[size]();
	this->G = new unsigned char[size]();
	this->B = new unsigned char[size]();

	unsigned char *R = this->R;
	unsigned char *G = this->G;
	unsigned char *B = this->B;
	unsigned char *temp = static_cast<unsigned char*> (imageMat.data);

	for (int i = 0, j = 0; i < realSize - 2; i += 3, j++) {
		B[j] = temp[i];
		G[j] = temp[i + 1];
		R[j] = temp[i + 2];
	}
}

/**
 * Generate space around the image with size relative to the padSize.
 */
Image::Image(const Image &originalImage, int padSizeY, int padSizeX) {
	int rows = this->RGBrows = originalImage.RGBrows + 2 * padSizeY;
	int cols = this->RGBcols = originalImage.RGBcols + 2 * padSizeX;
	this->channels = originalImage.channels;
	int size = rows * cols;

	this->R = new unsigned char[size]();
	this->G = new unsigned char[size]();
	this->B = new unsigned char[size]();

	std::fill_n(this->R, size, 255);
	std::fill_n(this->G, size, 255);
	std::fill_n(this->B, size, 255);

	unsigned char *R, *G, *B, *pR, *pG, *pB;
	R = originalImage.R;
	G = originalImage.G;
	B = originalImage.B;
	pR = this->R;
	pG = this->G;
	pB = this->B;
	int counter = padSizeY * this->RGBcols + padSizeX;
	int row, pos;

	for (int i = 0; i < originalImage.RGBrows; i++) {
		row = i * originalImage.RGBcols;
		for (int j = 0; j < originalImage.RGBcols; j++) {
			pos = row + j;
			pR[counter] = R[pos];
			pG[counter] = G[pos];
			pB[counter] = B[pos];
			counter++;
		}
		counter += padSizeX + padSizeX;
	}
}

Image::Image(const Image &originalImage) {
	this->RGBcols = originalImage.RGBcols;
	this->RGBrows = originalImage.RGBrows;
	this->channels = originalImage.channels;
	int size = this->RGBcols * this->RGBrows;

	this->R = new unsigned char[size]();
	this->G = new unsigned char[size]();
	this->B = new unsigned char[size]();

	memcpy(this->R, originalImage.R, size);
	memcpy(this->G, originalImage.G, size);
	memcpy(this->B, originalImage.B, size);

}

Image::Image(int rows, int cols, unsigned char channels) {
	this->RGBrows = rows;
	this->RGBcols = cols;
	this->channels = channels;
	int size = rows * cols;

	this->R = new unsigned char[size]();
	this->G = new unsigned char[size]();
	this->B = new unsigned char[size]();
}

Image::~Image() {
	delete[] this->R;
	delete[] this->G;
	delete[] this->B;
}
