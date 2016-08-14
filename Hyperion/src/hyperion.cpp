/**
 * Hyperion
 * hyperion.cpp
 * Purpose: Dehazing and low light image enhancement.
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 12/8/2016
 */

#include <list>
#include <inputParameters.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <dehaze.hpp>
#include <ctime>
#include <tests.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define DEBUG 1
#define TESTS 1

/**
 * Calculates the negative of the RGB image.
 *
 * @param[in,out] frm
 */
void negative(Dehazing &frm) {
	int size = frm.RGBcols * frm.RGBrows;

	for (int i = 0; i < size; i++) {
		frm.R[i] = 255 - frm.R[i];
		frm.G[i] = 255 - frm.G[i];
		frm.B[i] = 255 - frm.B[i];
	}
}

/**
 * Copies the input image in a container of type Mat.
 *
 * @param[in] frame1D
 * @param[out] frameMat
 */
void fromArrayToMat(Mat &frameMat, const Dehazing &frame1D) {
	int rows = frameMat.rows;
	int cols = frameMat.cols;
	int channels = frameMat.channels();
	int size = rows * cols;

	int realSize = size * channels;
	unsigned char *R = frame1D.R;
	unsigned char *G = frame1D.G;
	unsigned char *B = frame1D.B;
	unsigned char *temp = frameMat.data;

	for (int i = 0, j = 0; i < realSize - 2; i += 3, j++) {
		temp[i] = B[j];
		temp[i + 1] = G[j];
		temp[i + 2] = R[j];
	}

}

/**
 * Reads the configuration file and saves the necessary values.
 *
 * @param[in] configFile
 * @param[out] inpParam
 */
void readConfig(InputParameters &inpParam, const char* configFile) {
	ifstream config;
	config.open(configFile, ios::in);
	char data[256];
	char numString[1];

	if (config.is_open()) {
		config >> inpParam.inputFileName;
		config.getline(data, 256);

		config >> inpParam.outputFileName;
		config.getline(data, 256);

		config >> numString;
		inpParam.typeOfFile = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		inpParam.blockSizeX = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		inpParam.blockSizeY = atoi(numString);
		inpParam.blockSize = inpParam.blockSizeX * inpParam.blockSizeY;
		config.getline(data, 256);

		config >> numString;
		inpParam.boosting = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		inpParam.lowLightVideo = atoi(numString);
		config.getline(data, 256);
	}
	else {
		cout << "Unable to open hyperion.cfg file" << endl;
		exit(-1);
	}
	config.close();
#if DEBUG
	cout << configFile << endl;
	cout << "Input file name:" << inpParam.inputFileName << endl;
	cout << "Output file name:" << inpParam.outputFileName << endl;
	cout << "Type of file: " << inpParam.typeOfFile << "(JPEG = 0 Camera = 1)" << endl;
	cout << "Block size: " << inpParam.blockSizeX << "X" << inpParam.blockSizeY << endl;
	cout << "Boosting: " << inpParam.boosting << endl;
	cout << "Low light video: " << inpParam.lowLightVideo << endl;
#endif
}

/**
 * Reads a JPEG image and stores it in a Mat container.
 *
 * @param[in] inpParam
 * @param[out] pFrm
 */
void readFromJPEG(InputParameters &inpParam, list<Mat> &pFrm) {
	Mat frame = imread(inpParam.inputFileName, 1).clone();
	if (frame.empty()) {
		cout << "Couldn't read the image " << inpParam.inputFileName << endl << "Exiting..."
				<< endl;
		exit(-1);
	}
	pFrm.push_front(frame);
}

/**
 * Function responsible for reading and storing the input parameters and the input image.
 *
 * @param[in] ac argument counter
 * @param[in] av argument vector
 * @param[out] pFrm
 * @param[out] inpParam
 */
void Configure(list<Mat> &pFrm, InputParameters &inpParam, const int ac, char** av) {
	if (ac < 1) {
		cout << "Input the configuration file name as the first argument" << endl;
		exit(-1);
	}
	else if (ac == 2) {
		readConfig(inpParam, av[1]);
	}
	else {
		// TODO Create other input method
	}

	if (inpParam.typeOfFile == 0) {
		readFromJPEG(inpParam, pFrm);
	}
	else if (inpParam.typeOfFile == 1) {
		// TODO Read from camera input
	}
}

/**
 * The first argument should be the path full path to configuration file.
 */
int main(int argc, char** argv) {
	InputParameters inpParam;
	list<Mat> listOfFrames;					// The purpose of the list is to store multiple frames
	Configure(listOfFrames, inpParam, argc, argv);
	list<Mat>::iterator it = listOfFrames.begin();
	clock_t t;

	if (inpParam.typeOfFile == 0) {
#if DEBUG
#if TESTS
		Test tst;
		tst.testFuncPadding();
		tst.testFuncFindAirlight();
#endif
		cout << "dims:" << it->dims << endl << "depth:" << it->depth() << endl;
		cout << "width:" << it->cols << endl << "height:" << it->rows << endl;
		t = clock();
#endif
		Dehazing frm(*it);

		if (inpParam.lowLightVideo == 1) {
			negative(frm);
		}

		frm.dehazeProc(inpParam);

		if (inpParam.lowLightVideo == 1) {
			negative(frm);
		}
		fromArrayToMat(*it, frm);

		imwrite(inpParam.outputFileName, *it);
#if DEBUG
		t = clock() - t;
		cout << "dehazing ended" << endl;
		cout << "Executed in " << t / CLOCKS_PER_SEC << " seconds" << endl;
#endif
	}
	else if (inpParam.typeOfFile == 1) {
		// TODO Make it work with camera input (multiple frames)
	}

	return 1;
}
