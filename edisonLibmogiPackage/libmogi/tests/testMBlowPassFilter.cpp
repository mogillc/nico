/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>
#include "systems.h"

#include <math.h>

#include <float.h>

using namespace Mogi::Math;

bool test();
bool testMatrix();
bool testQuaternion();

bool matricesEqual(Matrix* A, Matrix* B) {
	bool testPassed = true;

	if (A->numRows() != B->numRows()) {
		printf(" - %s rows != %s rows, %d != %d\n", A->getName(), B->getName(),
				A->numRows(), B->numRows());
		testPassed = false;
	}

	if (A->numColumns() != B->numColumns()) {
		printf(" - %s cols != %s cols, %d != %d\n", A->getName(), B->getName(),
				A->numColumns(), B->numColumns());
		testPassed = false;
	}

	if (testPassed)
		for (int i = 0; i < A->numRows(); i++) {
			for (int j = 0; j < A->numColumns(); j++) {
				if (fabs((*A)(i, j) - (*B)(i, j)) > 0.000001) {
					testPassed = false;
					printf(" - %s(%d,%d) != %s(%d,%d), %f != %f\n",
							A->getName(), i, j, B->getName(), i, j, (*A)(i, j),
							(*B)(i, j));
				}
			}
		}

	return testPassed;
}

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning lpf tests:" << std::endl;
	allTestsPass = test() ? allTestsPass : false;
	std::cout << " - Beginning matrix lpf tests:" << std::endl;
	allTestsPass = testMatrix() ? allTestsPass : false;
	std::cout << " - Beginning quaternion lpf tests:" << std::endl;
	allTestsPass = testQuaternion() ? allTestsPass : false;

	if (allTestsPass != true) {
		std::cout << std::endl << "Result: FAILED" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << std::endl << "Result: Passed" << std::endl;

	return EXIT_SUCCESS;
}

bool test() {
	bool allTestsPass = true;

	LowPassFilter<double> lpf;
	lpf.setTimeConstant(1);

	std::cout << "Testing lpf1 initial condition .......... ";
	double result = lpf.filter(0, 1);
	if (result != 0) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0, actual:" << result << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing lpf1 step response 1*tau ........ ";
	result = lpf.filter(1, 1);
	if (result != (1.0 - exp(-1.0))) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: " << (1.0 - exp(-1.0)) << ", actual:"
				<< result << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing lpf1 step response 2*tau ........ ";
	result = lpf.filter(1, 1);
	if (result != (1.0 - exp(-2.0))) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: " << (1.0 - exp(-2.0)) << ", actual:"
				<< result << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing lpf1 step response 3*tau ........ ";
	result = lpf.filter(1, 1);
	if (result != (1.0 - exp(-3.0))) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: " << (1.0 - exp(-3.0)) << ", actual:"
				<< result << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	LowPassFilter<float> lpf2;
	lpf2.setTimeConstant(2);

	std::cout << "Testing lpf2 float response 1.0*tau ..... ";
	result = lpf2.filter(1, 1);
	if (result != (float) (1.0 - exp(-1.0 / 2.0))) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: " << (float) (1.0 - exp(-1.0 / 2.0))
				<< ", actual:" << result << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	LowPassFilter<int> lpf3;
	lpf3.setTimeConstant(1);

	std::cout << "Testing lpf3 int response 1.0*tau ....... ";
	int intResult = lpf3.filter(1, 1);
	if (intResult != (int) (1.0 - exp(-1.0 / 1.0))) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: " << (int) (1.0 - exp(-1.0 / 1.0))
				<< ", actual:" << intResult << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing setting output .................. ";
	lpf.setCurrentOutput(5.1234);
	if (lpf.output() != 5.1234) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testMatrix() {
	bool allTestsPass = true;

	LowPassFilter<Matrix> lpf;

	lpf.setCurrentOutput(Matrix(4, 1));

	Quaternion input;
	input(0) = 0;
	input(1) = 0;
	input(2) = 1;
	input(3) = 0;

	Quaternion expected;
	expected(0) = 0;
	expected(1) = 0;
	expected(2) = (1.0 - exp(-1.0));
	expected(3) = 0;

	std::cout << "Testing lpf response 1.0*tau ............ ";
	Matrix result = lpf.filter(input, 1);
	if (!matricesEqual(&expected, &result)) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testQuaternion() {
	bool allTestsPass = true;

	LowPassFilter<Quaternion> lpf;
	std::cout << "Testing lpf response 1.0*tau ............ ";

	Quaternion input;
	input(0) = 0;
	input(1) = 0;
	input(2) = 1;
	input(3) = 0;

	Quaternion expected;
	expected(0) = 1 / sqrt(2);
	expected(1) = 0;
	expected(2) = 1 / sqrt(2);
	expected(3) = 0;

	double time = log(0.5) * -1.0;
	Quaternion result = lpf.filter(input, time);
	if (!matricesEqual(&expected, &result)) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	expected(0) = 0.382683;
	expected(1) = 0;
	expected(2) = 0.923880;
	expected(3) = 0;

	std::cout << "Testing lpf response 2.0*tau ............ ";
	result = lpf.filter(input, time);
	if (!matricesEqual(&expected, &result)) {
		allTestsPass = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}
