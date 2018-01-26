/**
 * Hyperion
 * inputParameters.hpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_INPUTPARAMETERS_HPP_
#define INC_INPUTPARAMETERS_HPP_

#include <list>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/**
 * A class that contains the input parameters necessary for the correct configuration of the dehazing process.
 */
class InputParameters {
private:
public:
	std::ifstream inputStream; // Input stream in case of video stream
	char *inputFileName, *outputFileName;
	int typeOfFile, blockSizeX, blockSizeY, blockSize, boosting, lowLightVideo; //JPEG = 0 YUV = 1

	InputParameters();
	~InputParameters();

	void readConfig(const char*);
	void Configure(Mat &, const int, char**);
	void readFromImageFile(Mat &);

};

#endif /* INC_INPUTPARAMETERS_HPP_ */
