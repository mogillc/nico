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

/*
 * This is a very hacky and peculiar test to both	test the dynamixel
 * executable and get library coverage simultaneously.
 *
 * This is a hardware specific test as well, so these very well may fail if 
 * they are not run on Mogi's servers.
 *
 *
 * Current configuration:
 *  USB -> USB2AX ->|-> XL320 ID 1
 *                  |-> XL320 ID 2
 *
 */

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include "dynamixel.h"
#include <iomanip>
#include <iostream>

#include <unistd.h>

#include <cerrno>

#include <getopt.h>

#include <regex.h>
#include <string.h>

bool equal(const std::vector<unsigned char>&, const std::vector<unsigned char>&);
//bool testBasic();
bool testBadArgs();
bool testSearch();
bool testWriteRead();
bool testSync();
bool testBulk();
bool testCommandFailures();


int mainDynamixel(int argc, char* argv[]);	// I override the main function in dyanmixel.cpp

std::string performTest(std::vector<const char*> args, int* exitCode) {
	// remember cout's old streambuf
	std::streambuf * cout_strbuf(std::cout.rdbuf());
	std::streambuf * cerr_strbuf(std::cerr.rdbuf());

	// create the new stream to "redirect" cout's output to
	std::ostringstream output;
	std::ostringstream outputcerr;

	// set cout's streambuf to be the ostringstream's streambuf
	std::cout.rdbuf(output.rdbuf());
	std::cerr.rdbuf(output.rdbuf());

	optind = 1;
	*exitCode = mainDynamixel(args.size(), (char**)args.data());

	// replace cout's original streambuf when we're done.  It will
	// crash otherwise, since cout and stringstream will both
	// destroy the same streambuf.  Not truly safe, since a return
	// can bypass this call...
	std::cout.rdbuf(cout_strbuf);
	std::cerr.rdbuf(cerr_strbuf);

	return output.str();
}

// returns count of non-overlapping occurrences of 'sub' in 'str'
int countSubstring(const std::string& str, const std::string& sub)
{
	if (sub.length() == 0) return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos;
		 offset = str.find(sub, offset + sub.length()))
	{
		++count;
	}
	return count;
}

//std::string exec(const char* cmd) { // this fails miserably for gcov, oh well
////	std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
////	if (!pipe) return "ERROR";
////	char buffer[128];
////	std::string result = "";
////	while (!feof(pipe.get())) {
////		if (fgets(buffer, 128, pipe.get()) != NULL)
////			result += buffer;
////	}
//
//	FILE *fp;
//	char path[1000000];
//	std::string result = "";
//
//	/* Open the command for reading. */
//	fp = popen(cmd, "r");
//	if (fp == NULL) {
//		printf("Failed to run command\n" );
//		exit(1);
//	}
//
//	/* Read the output a line at a time - output it. */
//	while (fgets(path, sizeof(path)-1, fp) != NULL) {
//		//printf("%s", path);
//		result += path;
//	}
//
//	/* close */
//	pclose(fp);
//
//	return result;
//}

int main(int argc, char* argv[]) {
	bool allTestsPassed = true;
	//	Handler* dynamixelHandler = new Handler;

	std::cout << "Beginning bad argument tests" << std::endl;
	allTestsPassed = testBadArgs() ? allTestsPassed : false;
	std::cout << "Beginning Search tests (PING)" << std::endl;
	allTestsPassed = testSearch() ? allTestsPassed : false;
	std::cout << "Beginning read/write tests (READ, WRITE)" << std::endl;
	allTestsPassed = testWriteRead() ? allTestsPassed : false;
	std::cout << "Beginning sync tests (SYNC_READ, SYNC_WRITE)" << std::endl;
	allTestsPassed = testSync() ? allTestsPassed : false;
	std::cout << "Beginning bulk tests (BULK_READ, BULK_WRITE)" << std::endl;
	allTestsPassed = testBulk() ? allTestsPassed : false;
	std::cout << "Beginning command failure tests" << std::endl;
	allTestsPassed = testCommandFailures() ? allTestsPassed : false;

	if (allTestsPassed) {
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
}

//bool testBasic() {
//	bool allTestsPass = true;
//
//	std::cout << "Testing motor search ....................... ";
//	std::string result = exec("dynamixel -s -p 2 -d 0,20 -f /dev/ttyACM0");
//	if (countSubstring(result, "XL320") != 2) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Pass" << std::endl;
//	}
//
//	std::cout << "Testing motor search 2 ..................... ";
//	result = exec("dynamixel -s -p 2 -d 0,1 -f /dev/ttyACM0");
//	if (countSubstring(result, "XL320") != 1) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Pass" << std::endl;
//	}
//
//
//	return allTestsPass;
//}
bool testBadArgs() {
	bool allTestsPass = true;

	std::cout << "Testing no arguments .................... ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a fialure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing help argument ................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-h");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_SUCCESS) {						// Command is correct, should NOT be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad datarange ................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--datarange=2,3,4");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Error: Range format of") != 1 &&
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad datarange 2 ................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--datarange=2,");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Error: Range format of") != 1 &&
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad search ...................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--search=2,3,4");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad search 2 .................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--search=2,");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad write ....................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--write=25,");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad write 2 ..................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--write=2,56,8");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

//	std::cout << "Testing bad bulk ........................ ";
//	args.clear();
//	args.push_back("testDynamixel");
//	args.push_back("--bulkread=2,3,4,5,6");
//
//	output = performTest(args, &exitCode);
//	if (countSubstring(output, "Error: Arguments should be in the format") != 1 ||
//		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
//		std::cout << "FAILED" << std::endl;
//		std::cout << "output:" << output << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Pass" << std::endl;
//	}
//
//	std::cout << "Testing bad bulk 2 ...................... ";
//	args.clear();
//	args.push_back("testDynamixel");
//	args.push_back("--bulkread=5,6,7,8:9,10,11");
//
//	output = performTest(args, &exitCode);
//	if (countSubstring(output, "Error: Arguments should be in the format") != 1 ||
//		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
//		std::cout << "FAILED" << std::endl;
//		std::cout << "output:" << output << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Pass" << std::endl;
//	}

	std::cout << "Testing missing protocol ................ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-s");
	args.push_back("-p");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad protocol number ............. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-s");
	args.push_back("-p");
	args.push_back("3");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing missing ID ...................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-s");
	args.push_back("-i");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing bad protocol number ............. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-s");
	args.push_back("-i");
	args.push_back("three");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing unknown argument ................ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-X");
	args.push_back("three");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing unknown argument 2 .............. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("three");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing compteting arguments 1 .......... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-r");
	args.push_back("--write=2");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing compteting arguments 2 .......... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--write=2");
	args.push_back("-r");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing compteting arguments 3 .......... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-r");
	args.push_back("-R=.");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing compteting arguments 4 .......... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-r");
	args.push_back("-W=h");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Usage") > 0 &&			// Usage stament should be printed
		countSubstring(output, "testDynamixel") > 0 &&	// Executable name should be printed in usaage statement ( pretty weak test)
		exitCode != EXIT_FAILURE) {						// Command is incorrect, should be a failure
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	return allTestsPass;
}

bool testSearch() {
	bool allTestsPass = true;

	std::cout << "Testing motor search 1 .................. ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search");
	args.push_back("--protocol=2");
	args.push_back("--datarange=0,20");
	args.push_back("--file=/dev/ttyACM0");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	if (countSubstring(output, "XL320") != 2 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search 2 .................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=0,1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "XL320") != 1 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search 3 .................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--baud=1000000");
	args.push_back("--search=2,253");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "XL320") != 1 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search 4 .................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--baud=115200");		// No motors at this baudrate
	args.push_back("-s");	// default should be all IDs
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (output.length() > 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "output:" << output << std::endl;
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;


	}std::cout << "Testing motor search 5 .................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--baud=115200");		// No motors at this baudrate
	args.push_back("--protocol=2");
	args.push_back("--search=0,253");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (output.length() > 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "output:" << output << std::endl;
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search 6 .................. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=0,1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "XL320") != 1 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search bad range .......... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=1,-1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Error: negative numbers invalid in range") != 1) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search bad range 2 ........ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=-1,2");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Error: negative numbers invalid in range") != 1) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search bad range 3 ........ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=254,256");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Invalid Search parameters: Searchable IDs must be in the range of 0-253.") != 1) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search bad range 4 ........ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=1,254");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "Invalid Search parameters: Searchable IDs must be in the range of 0-253.") != 1) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor search bad range 5 ........ ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("-v");
	args.push_back("--search=2,2");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	output = performTest(args, &exitCode);
	if (countSubstring(output, "first ID must be smaller than second ID.") != 1) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}


	return allTestsPass;
}

bool testWriteRead() {
	bool allTestsPass = true;

	std::cout << "Testing motor write 1 ................... ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");
	args.push_back("--id=1");
	args.push_back("--write=25,1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	std::stringstream expected;
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor read 1 .................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--read");
	args.push_back("--protocol=2");
	args.push_back("--id=1");
	args.push_back("--datarange=25,1");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "25	1" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor write 2 ................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--id=1");
	args.push_back("--write=25,0");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "Expected = " << expected.str() << std::endl;
		std::cout << "output   = " << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor read 2 .................... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--read");
	args.push_back("--protocol=2");
	args.push_back("--id=1");
	args.push_back("--datarange=25,1");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "25	0" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	return allTestsPass;
}


bool testSync() {
	bool allTestsPass = true;

	std::cout << "Testing motor sync write 1 .............. ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");
	args.push_back("--id=1,2");
	args.push_back("--write=25,1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	std::stringstream expected;
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor sync read 1 ............... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--read");
	args.push_back("--protocol=2");
	args.push_back("--id=1,2");
	args.push_back("--datarange=25,1");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "Motor ID=1--25	1" << std::endl;
	expected << "Motor ID=2--25	1" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor sync write 2 .............. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--id=1,2");
	args.push_back("--write=25,0");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "Expected = " << expected.str() << std::endl;
		std::cout << "output   = " << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor sync read 2 ............... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--read");
	args.push_back("--protocol=2");
	args.push_back("--id=1,2");
	args.push_back("--datarange=25,1");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "Motor ID=1--25	0" << std::endl;
	expected << "Motor ID=2--25	0" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	return allTestsPass;
}

bool testBulk() {
	bool allTestsPass = true;

	std::cout << "Testing motor bulk write 1 .............. ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");
	args.push_back("--bulkwrite=1,25,1,1:2,11,1,1");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	std::stringstream expected;
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor bulk read 1 ............... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--bulkread=1,25,1,0:2,11,1,0");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "Motor ID:1	Reg:25	Length:1" << std::endl;
	expected << "25	1" << std::endl;
	expected << "Motor ID:2	Reg:11	Length:1" << std::endl;
	expected << "11	1" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor bulk write 2 .............. ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--bulkwrite=1,25,1,0:2,11,1,2");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	if (output.compare(expected.str()) != 0 &&	// should be no output from this
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "Expected = " << expected.str() << std::endl;
		std::cout << "output   = " << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing motor bulk read 2 ............... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--bulkread=1,25,1,0:2,11,1,0");
	args.push_back("--protocol=2");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	expected.str("");
	expected << "Motor ID:1	Reg:25	Length:1" << std::endl;
	expected << "25	0" << std::endl;
	expected << "Motor ID:2	Reg:11	Length:1" << std::endl;
	expected << "11	2" << std::endl;
	if (output.compare(expected.str()) != 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	return allTestsPass;
}

bool testCommandFailures() {
	bool allTestsPass = true;

	std::cout << "Testing read command missing motor ...... ";
	std::vector<const char*> args;
	args.push_back("testDynamixel");
	args.push_back("-r");
	args.push_back("--datarange=25,2");
	args.push_back("--file=/dev/ttyACM0");

	int exitCode = EXIT_FAILURE;
	std::string output = performTest(args, &exitCode);
	if (countSubstring(output, "Failure") == 0 &&
		countSubstring(output, "READ command") == 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	std::cout << "Testing write command missing motor ..... ";
	args.clear();
	args.push_back("testDynamixel");
	args.push_back("--write=25,1");
	args.push_back("--protocol=2");
	args.push_back("--id=100");
	args.push_back("--file=/dev/ttyACM0");

	exitCode = EXIT_FAILURE;
	output = performTest(args, &exitCode);
	if (countSubstring(output, "Failure") == 0 &&
		countSubstring(output, "WRITE command") == 0 &&
		exitCode != EXIT_FAILURE) {			// This may be wierd, but we want to make sure there is an available port to test algorithms rather than connected hardware
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << output << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}


	return allTestsPass;
}

bool equal(const std::vector<unsigned char>& a,
		const std::vector<unsigned char>& b) {
	if (a.size() != b.size()) {
		std::cout << "Size Mismatch " << a.size() << " != " << b.size() << ": ";
		return false;
	}

	for (unsigned int i = 0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			std::cout << "Element mismatch at " << i << ", " << (int) a[i]
					<< " != " << (int) b[i] << ": ";
			return false;
		}
	}

	return true;
}

// Here comes the hacks...
#define main mainDynamixel			// NASTY!
#include "../install/dynamixel.cpp"	// WOAH!
