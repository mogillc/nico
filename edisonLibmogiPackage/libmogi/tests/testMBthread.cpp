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

#include "mogi/thread.h"
#include <unistd.h>
#include <iostream>

#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

bool testConstruction();
bool testSingleThread();
bool testMultipleThreads();

bool didTerminate = false;

class Thread2: public Mogi::Thread {
public:
};

class testThread: public Mogi::Thread {
public:
	static int count;
	int index;

	void entryAction() {
		index = 0;
	}
	;

	void doAction() {
		index++;
		usleep(100);
	}

	void exitAction() {
		index = -1;
	}
	;

	testThread() {
		count++;
	}
	;

	~testThread() {
		count--;
	}
	;
};

int testThread::count = 0;

int main(int argc, char *argv[]) {
	// return EXIT_SUCCESS;
	bool allTestsPass = true;

	std::cout << " - Beginning Constructor tests:" << std::endl;
	allTestsPass = testConstruction() ? allTestsPass : false;
	std::cout << " - Beginning Single Thread tests:" << std::endl;
	allTestsPass = testSingleThread() ? allTestsPass : false;
	std::cout << " - Beginning Single Thread tests again:" << std::endl;
	allTestsPass = testSingleThread() ? allTestsPass : false;
	std::cout << " - Beginning Multiple Thread tests:" << std::endl;
	allTestsPass = testMultipleThreads() ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

bool testConstruction() {  // currently a very weak test
	bool testPasses = true;

	std::cout << "Testing Constructors ............... ";
	testThread *thread = new testThread();

	if (thread->running() || testThread::count != 1) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete thread;
	std::cout << "Testing Destructors ................ ";
	if (testThread::count != 0) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Simple Thread start ........ ";
	Thread2 *threadS = new Thread2;
	if (!threadS->start()) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Simple Thread stop ......... ";
	usleep(1000);
	threadS->stop();
	usleep(1000);
	if (threadS->running()) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Simple Thread delete ....... ";
	delete threadS;
	if (false) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return testPasses;
}

bool testSingleThread() {
	bool testPasses = true;

	std::cout << "Testing Single Thread start ........ ";
	Mogi::Thread *thread = new testThread;
	if (!thread->start()) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Single Thread running ...... ";
	int previous = ((testThread *) thread)->index;
	usleep(1000);
	int current = ((testThread *) thread)->index;
	if (current == previous) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Single Thread pause ........ ";
	thread->pause();
	previous = ((testThread *) thread)->index;
	usleep(1000);
	current = ((testThread *) thread)->index;
	if (current != previous) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Single Thread resume ....... ";
	thread->resume();
	usleep(1000);
	current = ((testThread *) thread)->index;
	if (current == previous) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Single Thread stop ......... ";
	thread->stop();
	usleep(1000);
	if (thread->running() || ((testThread *) thread)->index != -1) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Soft Thread wait to finish . ";
	thread->start();
	thread->WaitForInternalThreadToExit();
	if (thread->running() == true || ((testThread *) thread)->index != -1) {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete thread;
	return testPasses;
}

bool testMultipleThreads() {
	bool testPasses = true;

	std::cout << "Testing Multiple Threads ........... ";
	int numThreads = 20;
	testThread *threads = new testThread[numThreads];
	for (int i = 0; i < numThreads; i++) {
		threads[i].start();
		threads[i].pause();
	}
	if (testThread::count == numThreads) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	}

	std::cout << "Testing Multiple Thread destruct ... ";
	//	for (int i = 0; i < numThreads; i++) {
	//		threads[i].WaitForInternalThreadToExit();
	//	}
	delete[] threads;
	if (testThread::count == 0) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		testPasses = false;
	}
	return testPasses;
}
