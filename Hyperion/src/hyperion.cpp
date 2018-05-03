/**
 * Hyperion
 * hyperion.cpp
 * Purpose: Dehazing and low light image enhancement.
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */

#include <list>
#include <inputParameters.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <dehaze.hpp>
#include <ctime>
#include <image.hpp>
#include <utilities.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define DEBUG 1

/**
 * The first argument should be the path full path to configuration file.
 */
int main(int argc, char** argv) {
	InputParameters inpParam;
	Mat inpImageMat;
	inpParam.Configure(inpImageMat, argc, argv);
	clock_t t;

	if (inpParam.typeOfFile == 0) {
#if DEBUG
		cout << "depth:" << inpImageMat.channels() << endl;
		cout << "width:" << inpImageMat.cols << endl << "height:" << inpImageMat.rows << endl;
		t = clock();
#endif
		Image img(inpImageMat);
		Image outputImage(img.RGBrows, img.RGBcols, img.channels);

		Dehazing dh;
		dh.dehazeProc(inpParam, img, outputImage);
		Mat outImageMat(outputImage.RGBrows, outputImage.RGBcols, inpImageMat.type());

		utilities::fromImageToMat(outImageMat, outputImage);
		imwrite(inpParam.outputFileName, outImageMat);
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
