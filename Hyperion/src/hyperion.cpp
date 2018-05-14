/**
 * Hyperion
 * hyperion.cpp
 * Purpose: Dehazing and low light image enhancement.
 *
 * @author: Evangelos Mavropoulos <evmavrop@gmail.com>
 * @date: 12/8/2016
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
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/videoio/videoio.hpp>

using namespace std;
using namespace cv;

#define DEBUG 1

/**
 * The first argument should be the path full path to configuration file.
 */
int main(int argc, char** argv) {
	InputParameters inpParam;
	inpParam.Configure(argc, argv);

	clock_t t;

	if (inpParam.typeOfFile == 0) {
		Mat inpImageMat;
		inpParam.readFromImageFile(inpImageMat);

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
		VideoCapture inputVideo;
		VideoWriter outputVideo;
		inpParam.readFromVideoFile(inputVideo);
		Size S = Size((int) (inputVideo.get(CAP_PROP_FRAME_WIDTH)),
				(int) inputVideo.get(CAP_PROP_FRAME_HEIGHT));

		outputVideo .open(inpParam.outputFileName, inputVideo.get(CAP_PROP_FOURCC),
				inputVideo.get(CAP_PROP_FPS), S, true);

		if (!outputVideo.isOpened()) {
			cout << "Could not open the output video for write: " << inpParam.outputFileName
					<< endl;
			exit(-1);
		}

		Mat frame, outputFrame;
		Dehazing dh;
		int frameCounter = 0;
		while (1) {
			inputVideo.read(frame);
			if (frame.empty()) {
				break;
			}

			Image img(frame);
			Image outputImage(img.RGBrows, img.RGBcols, img.channels);

			dh.dehazeProc(inpParam, img, outputImage);
			Mat outImageMat(outputImage.RGBrows, outputImage.RGBcols, frame.type());

			utilities::fromImageToMat(outImageMat, outputImage);
			namedWindow("Display window", WINDOW_AUTOSIZE);// Create a window for display.
			imshow("Display window", outImageMat); // Show our image inside it.
			waitKey(1);
			cout << "Frame " << frameCounter++ << " is being processed" << endl;

			outputVideo.write(outImageMat);

		}
		outputVideo.release();
	}

	return 1;
}
