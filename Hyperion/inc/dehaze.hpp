/**
 * Hyperion
 * dehaze.hpp
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_DEHAZE_HPP_
#define INC_DEHAZE_HPP_

#include <frame.hpp>
#include <inputParameters.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <tests.hpp>

using namespace std;
using namespace cv;

/**
 * This class contains all the functions needed for the dehazing process and some complementary data members
 * necessary for the successful implementation of the process.
 */
class Dehazing: public Frame {
private:
	int findAirlight(int);
	void quicksort(unsigned int **, int, int);
	void minRGB(unsigned int **, int);
	float *findMediumTransmission(int position, int size, InputParameters &inpParam);
	int brightestPix(unsigned int **, int size);
	void padding(InputParameters &inpParam);
	friend class Test;

public:
	friend void testFunc();
	void dehazeProc(InputParameters &inpParam);
	unsigned char *paddedR, *paddedG, *paddedB;
	int paddedRGBrows, paddedRGBcols;

	Dehazing();
	Dehazing(Mat);
	~Dehazing();
};

#endif /* INC_DEHAZE_HPP_ */
