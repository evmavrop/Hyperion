/**
 * Hyperion
 * global.hpp
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_GLOBAL_HPP_
#define INC_GLOBAL_HPP_

/**
 * A class that contains arrays that are used as storages for RGB images.
 */
class Frame {

public:
	int RGBrows, RGBcols;
	unsigned char *R, *G, *B;
	int frameNum;

};

#endif /* INC_GLOBAL_HPP_ */
