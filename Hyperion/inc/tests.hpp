/**
 * Hyperion
 * tests.hpp
 * Purpose: Contains the functions required for the dehazing process
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 14/8/2016
 */

#ifndef TESTS_HPP_
#define TESTS_HPP_

#include <dehaze.hpp>
/**
 * This class contains member functions whose purpose is to test the output of every function.
 */
class Tests {
	friend class Dehazing;

public:
	void testFuncPadding();
	void testFuncFindAirlight();

};

#endif /* TESTS_HPP_ */
