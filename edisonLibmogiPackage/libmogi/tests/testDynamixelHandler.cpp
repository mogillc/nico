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

#include "dynamixel.h"
#include <unistd.h>
#include <iostream>
#include "mmath.h"

#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

using namespace Mogi;
using namespace Dynamixel;

bool testConstruction();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning construction tests:" << std::endl;
	allTestsPass = testConstruction() ? allTestsPass : false;

	std::cout << " - Done." << std::endl;
	if (allTestsPass) {
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

bool testConstruction() {
	bool allTestsPass = true;

	std::cout << "Testing Constructor/Destructor .......... ";
	Handler* handler = new Handler;

	delete handler;
	if (true) {  // what to test here?
		std::cout << "Pass" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Testing Interface with destructor ....... ";
	handler = new Handler;

	handler->openInterface(Interface::FTDI, 100000);
	delete handler;
	if (true) {  // what to test here?
		std::cout << "Pass" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Testing Instruction with destructor ..... ";
	handler = new Handler;

	handler->openInterface(Interface::COM, 100000);

	handler->pushInstruction(new Instruction(Instruction::WRITE));
	delete handler;
	if (true) {  // what to test here?
		std::cout << "Pass" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Testing force add ....................... ";
	handler = new Handler;

	handler->openInterface(Interface::COM, 100000);

	handler->forceAddDynamixel(1);
	while (handler->busy())
		;

	if (handler->getDynamixels().size() == 1) {
		std::cout << "Pass" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Testing failed add ...................... ";
	handler->addDynamixel(254);
	while (handler->busy())
		;

	if (handler->getDynamixels().size() == 1) {
		std::cout << "Pass" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}
