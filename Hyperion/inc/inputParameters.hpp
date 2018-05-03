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
		std::ifstream inputStream; /**<Input stream in case of video stream*/
		char *inputFileName; /**<Input file name*/
		char *outputFileName; /**<Output file name*/
		int typeOfFile; /**<Type of input file: JPEG = 0 YUV = 1*/

		int blockSizeX; /**<Number of rows in a block*/
		int blockSizeY; /**<Number of columns in a block*/
		int blockSize; /**<Size of block*/
		int boosting; /**<Use of boosting: YES = 1 NO = 0*/
		int lowLight; /**<The input is lowlight:  YES = 1 NO = 0*/

		InputParameters();
		~InputParameters();

		/**
		 * @brief Read config
		 * Reads the configuration file and saves the necessary values.
		 *
		 * @param configFile Configure file name
		 */
		void readConfig(const char* configFile);

		/**
		 * @brief Read config parameters
		 * Function responsible for reading and storing the input parameters and the input image.
		 *
		 * @param inpImage inputImage
		 * @param ac argument counter
		 * @param av argument vector
		 */
		void Configure(Mat& inpImage, const int ac, char** av);
		/**
		 * Reads a JPEG image and stores it in a Mat container.
		 *
		 * @param inpImage Input Image
		 */
		void readFromImageFile(Mat &inpImage);

};

#endif /* INC_INPUTPARAMETERS_HPP_ */
