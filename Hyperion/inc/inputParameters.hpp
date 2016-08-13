/**
 * Hyperion
 * imageParameters.hpp
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_IMAGEPARAMETERS_HPP_
#define INC_IMAGEPARAMETERS_HPP_

#include <frame.hpp>
#include <fstream>

/**
 * A class that contains the input parameters necessary for the correct configuration of the dehazing process.
 */
class InputParameters {
private:
public:
	std::ifstream inputStream; // Input stream in case of video stream
	char *inputFileName, *outputFileName;
	int typeOfFile, blockSizeX, blockSizeY, blockSize, boosting, lowLightVideo; //JPEG = 0 YUV = 1

	InputParameters() {
		boosting = 0;
		typeOfFile = -1;
		inputFileName = new char[100];
		outputFileName = new char[100];
		blockSizeX = 16;
		blockSizeY = 16;
		blockSize = 256;
		lowLightVideo = 0;
	}
	~InputParameters() {
		delete[] inputFileName;
		delete[] outputFileName;
	}
};

#endif /* INC_IMAGEPARAMETERS_HPP_ */
