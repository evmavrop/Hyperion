/**
 * Hyperion
 * frame.hpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_FRAME_HPP_
#define INC_FRAME_HPP_

#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
/**
 * A class that contains arrays that are used as storages for RGB images.
 */
class Image {

public:
	int RGBrows, RGBcols;
	unsigned char *R, *G, *B, channels;

	Image();
	Image(const Mat&);
	Image(const Image&);
	Image(const Image&, int, int);
	Image(int, int, unsigned char);
	~Image();
};

#endif /* INC_FRAME_HPP_ */
