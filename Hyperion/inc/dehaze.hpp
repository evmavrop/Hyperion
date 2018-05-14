/**
 * Hyperion
 * dehaze.hpp
 *
 * @author: Evangelos Mavropoulos
 * @date: 12/8/2016
 */

#ifndef INC_DEHAZE_HPP_
#define INC_DEHAZE_HPP_

#include <image.hpp>
#include <inputParameters.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

/**
 * @brief Dehazing process class
 * This class contains all the functions needed for the dehazing process and some complementary data members
 * necessary for the successful implementation of the process.
 */
class Dehazing {
	private:
		int findAirlight(Image&);
		void quicksort(unsigned int **, int, int);
		void minRGB(Image&, unsigned int **);
		float *findMediumTransmission(Image&, int, InputParameters &);
		int brightestPix(Image&, unsigned int **);
		void padding(InputParameters &);
		void negative(Image &);
		friend class Tests;

	public:
		void dehazeProc(InputParameters &, Image &, Image &);

		Dehazing();
		~Dehazing();
};

#endif

