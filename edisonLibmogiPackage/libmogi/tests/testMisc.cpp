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

#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string.h>

#include "mogi.h"

/*
 Test classes
 */

/*
 Global variables
 */

/*
 Global functions
 */

/*
 Test function prototypes
 */
bool testBasic(bool verbose);

int main(int argc, char* argv[]) {
	bool verbose = true;
	bool allTestsPass = true;

	std::cout << "Testing Basic" << std::endl;
	allTestsPass = testBasic(verbose) ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testBasic(bool verbose) {
	bool allTestsPass = true;

	Mogi::VersionInfo version = Mogi::getLibraryVersion();

	std::cout << "Checking version info .................. ";
	if (version.major < 0 || version.micro < 0 || version.minor < 0
			|| strlen(version.version_str) < 5) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	return allTestsPass;
}
