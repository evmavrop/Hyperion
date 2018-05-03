/**
 * Hyperion
 * frame.hpp
 *
 * @author: Evangelos Mavropoulos
 * @version: 1.0 12/8/2016
 */

#ifndef INC_IMAGE_HPP_
#define INC_IMAGE_HPP_

#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>

using namespace cv;
/**
 * @brief Image class
 * A class that contains arrays that are used as storages for RGB images.
 */
class Image {

public:
	int RGBrows; /**<Image rows*/
	int RGBcols;  /**<Image columns*/
	unsigned char *R; /**<Red value of pixels*/
	unsigned char *G;  /**<Green value of pixels*/
	unsigned char *B;  /**<Blue value of pixels*/
	unsigned char channels;  /**<Channels of the image*/

	Image();
	/**
	 * @brief From Mat to Image
	 *
	 * @param imageMat Image to be copied
	 */
	Image(const Mat& imageMat);
	/**
	 * @brief Copy constructor
	 *
	 * @param originalImage Image to be copied
	 */
	Image(const Image& originalImage);
	/**
	 * @brief Padding
	 * Padding of the image
	 *
	 * @param originalImage The initial image
	 * @param padSizeY Padding of rows
	 * @param padSizeX Padding of cols
	 */
	Image(const Image& originalImage, int padSizeY, int padSizeX);
	/**
	 * @brief Empty contrainer constructor
	 *
	 * @param rows Rows
	 * @param cols Columns
	 * @param channels Channels of the image
	 */
	Image(int, int, unsigned char);
	~Image();
};

#endif
