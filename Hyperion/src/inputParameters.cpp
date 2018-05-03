/**
 * Hyperion
 * inputParameters.cpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */
#include <inputParameters.hpp>

InputParameters::InputParameters() {
	boosting = 0;
	typeOfFile = -1;
	inputFileName = new char[100];
	outputFileName = new char[100];
	blockSizeX = 9;
	blockSizeY = 9;
	blockSize = 81;
	lowLight = 0;
}

InputParameters::~InputParameters() {
	delete[] inputFileName;
	delete[] outputFileName;
}

void InputParameters::readConfig(const char* configFile) {
	ifstream config;
	config.open(configFile, ios::in);
	char data[256];
	char numString[1];

	if (config.is_open()) {
		config >> this->inputFileName;
		config.getline(data, 256);

		config >> this->outputFileName;
		config.getline(data, 256);

		config >> numString;
		this->typeOfFile = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		this->blockSizeX = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		this->blockSizeY = atoi(numString);
		this->blockSize = this->blockSizeX * this->blockSizeY;
		config.getline(data, 256);

		config >> numString;
		this->boosting = atoi(numString);
		config.getline(data, 256);

		config >> numString;
		this->lowLight = atoi(numString);
		config.getline(data, 256);
	}
	else {
		cout << "Unable to open hyperion.cfg file" << endl;
		exit(-1);
	}
	config.close();
#if DEBUG
	cout << configFile << endl;
	cout << "Input file name:" << this->inputFileName << endl;
	cout << "Output file name:" << this->outputFileName << endl;
	cout << "Type of file: " << this->typeOfFile << "(JPEG = 0 Camera = 1)" << endl;
	cout << "Block size: " << this->blockSizeX << "X" << inpParam.blockSizeY << endl;
	cout << "Boosting: " << this->boosting << endl;
	cout << "Low light video: " << this->lowLight << endl;
#endif
}

void InputParameters::Configure(Mat &inpImage, const int ac, char** av) {
	if (ac < 1) {
		cout << "Input the configuration file name as the first argument" << endl;
		exit(-1);
	}
	else if (ac == 2) {
		this->readConfig(av[1]);
	}
	else {
		// TODO Create other input method for single images
	}

	if (this->typeOfFile == 0) {
		this->readFromImageFile(inpImage);
	}
	else if (this->typeOfFile == 1) {
		cout << "Wrong type of file" << endl;
		exit(-1);
	}
}

void InputParameters::readFromImageFile(Mat &inpImage) {
	inpImage = cv::imread(this->inputFileName, 1);
	if (inpImage.empty()) {
		std::cout << "Couldn't read the image " << this->inputFileName << std::endl << "Exiting..."
				<< std::endl;
		exit(-1);
	}
}
