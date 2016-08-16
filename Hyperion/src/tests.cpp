/**
 * Hyperion
 * tests.cpp
 * Purpose: Contains the functions testing
 *
 * @author: Mavropoulos Evangelos
 * @version: 1.0 14/8/2016
 */

#include <tests.hpp>
#include <iostream>

void Tests::testFuncPadding() {
	Dehazing testObj;
	InputParameters params;

	testObj.padding(params);

	cout << endl << "Test padding" << endl;
	for (int i = 0; i < testObj.paddedRGBrows; i++) {
		for (int j = 0; j < testObj.paddedRGBcols; j++) {
			cout << static_cast<unsigned int>(testObj.paddedR[i * testObj.paddedRGBrows + j])
					<< "\t";
		}
		cout << endl;
	}
	cout << endl << endl;
	for (int i = 0; i < testObj.paddedRGBrows; i++) {
		for (int j = 0; j < testObj.paddedRGBcols; j++) {
			cout << static_cast<unsigned int>(testObj.paddedG[i * testObj.paddedRGBrows + j])
					<< "\t";
		}
		cout << endl;
	}
	cout << endl << endl;
	for (int i = 0; i < testObj.paddedRGBrows; i++) {
		for (int j = 0; j < testObj.paddedRGBcols; j++) {
			cout << static_cast<unsigned int>(testObj.paddedB[i * testObj.paddedRGBrows + j])
					<< "\t";
		}
		cout << endl;
	}
	cout << endl << endl;
	// TODO Save the arrays in a txt file
}

void Tests::testFuncFindAirlight() {
	int A[3];
	Dehazing testObj;

	int position = testObj.findAirlight(testObj.RGBrows * testObj.RGBcols);

	A[0] = testObj.R[position];
	A[1] = testObj.G[position];
	A[2] = testObj.B[position];

	cout << endl << "Test findAirlight" << endl;
	cout << "Correct Position = " << testObj.RGBrows * testObj.RGBcols - 1 << endl;
	cout << "Position = " << position << endl;
	cout << "Correct Airlight Red = " << testObj.RGBrows * testObj.RGBcols - 1 << " Airlight Red = "
			<< A[0] << endl;
	cout << "Correct Airlight Green = " << testObj.RGBrows * testObj.RGBcols - 1
			<< " Airlight Green = " << A[1] << endl;
	cout << "Correct Airlight Blue = " << testObj.RGBrows * testObj.RGBcols - 1
			<< " Airlight Blue = " << A[2] << endl;

	cout << endl << endl;
}
