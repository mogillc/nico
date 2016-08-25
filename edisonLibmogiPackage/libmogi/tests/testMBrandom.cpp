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

using namespace Mogi;

bool testUniform();
bool testNormal();

int main(int argc, char *argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning Uniform tests:" << std::endl;
	allTestsPass = testUniform() ? allTestsPass : false;
	std::cout << " - Beginning Normal tests:" << std::endl;
	allTestsPass = testNormal() ? allTestsPass : false;

	if (allTestsPass != true) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

bool testUniform() {
	bool allTestsPass = true;

	Math::Random random;

	std::cout << "Testing Uniform distribution ......... ";
	for (int i = -100; i < 100; i++) {
		for (int j = i + 1; j < 100; j++) {
			double result = random.uniform(i, j);
			if (result < i || result > j) {
				allTestsPass = false;
			}
		}
	}
	if (allTestsPass != true) {
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testNormal() {
	bool allTestsPass = true;

	Math::Random random;

	std::cout << "Testing Normal distribution ......... ";

	double result = random.normal(10, 3);
	result = random.normal(10, 3);

	return allTestsPass;
}
