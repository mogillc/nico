/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include <math.h>
#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>

#include "mmath.h"

bool testLinear();
bool testQuadratic();
bool testCubic();

using namespace Mogi::Math;

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
		for (unsigned int i = 0; i < A->numRows(); i++) {
			for (unsigned int j = 0; j < A->numColumns(); j++) {
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

	std::cout << " - Beginning Linear tests:" << std::endl;
	allTestsPass = testLinear() ? allTestsPass : false;
	std::cout << " - Beginning Quadratic tests:" << std::endl;
	allTestsPass = testQuadratic() ? allTestsPass : false;
	std::cout << " - Beginning Cubic tests:" << std::endl;
	allTestsPass = testCubic() ? allTestsPass : false;

	if (allTestsPass != true) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

bool testLinear() {
	bool allTestsPass = true;

	Bezier bezier(2);

	Vector P(3);
	P(0) = 8;
	P(1) = 1;
	P(2) = 1500;
	bezier.setControlPoint(P, 0);

	Vector P2(3);
	P2(0) = 10;
	P2(1) = -1;
	P2(2) = 1000;
	bezier.setControlPoint(P2, 1);

	std::cout << "Checking start ..................... ";
	Matrix result = bezier.compute(0);
	if (matricesEqual(&result, &P)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking end ....................... ";
	result = bezier.compute(1);
	if (matricesEqual(&result, &P2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	Vector correct(3);
	correct(0) = 9;
	correct(1) = 0;
	correct(2) = 1250;

	std::cout << "Checking middle .................... ";
	result = bezier.compute(0.5);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking bad t (t<0) ............... ";
	result = bezier.compute(-0.001);
	if (matricesEqual(&result, &P)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking bad t (t>1) ............... ";
	result = bezier.compute(1.00001);
	if (matricesEqual(&result, &P2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testQuadratic() {
	bool allTestsPass = true;

	Bezier bezier(3);

	Vector P(3);
	P(0) = 10;
	P(1) = 0;
	P(2) = 1500;
	bezier.setControlPoint(P, 0);

	Vector P2(3);
	P2(0) = 0;
	P2(1) = 0;
	P2(2) = 1000;
	bezier.setControlPoint(P2, 1);

	Vector P3(3);
	P3(0) = 10;
	P3(1) = 100;
	P3(2) = 1000;
	bezier.setControlPoint(P3, 2);

	std::cout << "Checking start ..................... ";
	Matrix result = bezier.compute(0);
	if (matricesEqual(&result, &P)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking end ....................... ";
	result = bezier.compute(1);
	if (matricesEqual(&result, &P3)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	Vector correct(3);
	correct(0) = 5;
	correct(1) = 25;
	correct(2) = 1125;

	std::cout << "Checking middle 1 .................. ";
	result = bezier.compute(0.5);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	correct(0) = 6.25;
	correct(1) = 6.25;
	correct(2) = 1281.25;

	std::cout << "Checking middle 2 .................. ";
	result = bezier.compute(0.25);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testCubic() {
	bool allTestsPass = true;

	Bezier bezier(4);

	Vector P(4);
	P(0) = 0;
	P(1) = 0;
	P(2) = 0;
	P(3) = 0;
	bezier.setStartPoint(P);

	Vector P2(4);
	P2(0) = 0;
	P2(1) = 0;
	P2(2) = 100;
	P2(3) = 100.0 / 3.0;
	bezier.setControlPoint(P2, 1);

	Vector P3(4);
	P3(0) = 0;
	P3(1) = 100;
	P3(2) = 0;
	P3(3) = 100.0 * 2.0 / 3.0;
	bezier.setControlPoint(P3, 2);

	Vector P4(4);
	P4(0) = 100;
	P4(1) = 100;
	P4(2) = 100;
	P4(3) = 100;
	bezier.setFinalPoint(P4);

	std::cout << "Checking start ..................... ";
	Matrix result = bezier.compute(0);
	if (matricesEqual(&result, &P)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking end ....................... ";
	result = bezier.compute(1);
	if (matricesEqual(&result, &P4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	Vector correct(4);
	correct(0) = 12.5;
	correct(1) = 50;
	correct(2) = 50;
	correct(3) = 50;

	std::cout << "Checking middle 1 .................. ";
	result = bezier.compute(0.5);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	correct(0) = 1.5625;
	correct(1) = 15.625;
	correct(2) = 43.75;
	correct(3) = 25;

	std::cout << "Checking middle 2 .................. ";
	result = bezier.compute(0.25);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	correct(0) = 42.1875;
	correct(1) = 84.375;
	correct(2) = 56.25;
	correct(3) = 75;

	std::cout << "Checking middle 3 .................. ";
	result = bezier.compute(0.75);
	if (matricesEqual(&result, &correct)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}
