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
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include "systems.h"

#include <math.h>

#include <float.h>

using namespace Mogi::Math;

class testMotionControl: public MotionControl {//<double> {
public:
	void setValuesForTesting() {
		setMaxSpeed(100);
		maxAcceleration = 10.000000000000000000000000000000;
//		speed = 0.302916903840337703446294881360;
		locationState = ACCELERATION;
		goalPosition = 0.004587932531610821630663199500;
		position = 0;
	}
};

bool testDouble();
bool testVector();
bool testAngular();

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

	std::cout << " - Beginning Double tests:" << std::endl;
	allTestsPass = testDouble() ? allTestsPass : false;
	std::cout << " - Beginning Vector tests:" << std::endl;
	allTestsPass = testVector() ? allTestsPass : false;
	std::cout << " - Beginning Angular tests:" << std::endl;
	allTestsPass = testAngular() ? allTestsPass : false;

	if (allTestsPass != true) {
		std::cout << std::endl << "Result: FAILED" << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << std::endl << "Result: Passed" << std::endl;

	return EXIT_SUCCESS;
}

bool testDouble() {
	bool allTestsPass = true;

	MotionControl mc;

	std::cout << "Testing Velocity control ........... ";
	mc.enableVelocityControl();
	mc.setVelocity(2);
	double result = mc.processLocationControl(.1);
	if (result != .2 && mc.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 2 ......... ";
	mc.setVelocity(.2);
	result = mc.processLocationControl(4);
	if (result != 1 && mc.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 3 ......... ";
	mc.setVelocity(-1);
	result = mc.processLocationControl(1);
	if (result != 0 && mc.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 4 ......... ";
	mc.setVelocity(0);
	result = mc.processLocationControl(100);
	if (result != 0 && mc.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: stp->mov ..... ";
	MotionControl mc1;
	mc1.enableLocationControl();
	mc1.setMaxSpeed(1);
	mc1.setAcceleration(1);
	mc1.setGoalPosition(4);
//	result = mc1.processLocationControl(.75);
	result = mc1.processLocationControl(1.5);
	if (result != 1.0 && mc1.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: mov->mov ..... ";
	result = mc1.processLocationControl(2);
	if (result != 3.0 && mc1.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 3.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: mov->stp ..... ";
	result = mc1.processLocationControl(1.5);
	if (result != 4.0 && mc1.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: stp->acc ..... ";
	MotionControl mc2;
	mc2.enableLocationControl();
	mc2.setMaxSpeed(1);
	mc2.setAcceleration(1);
	mc2.setGoalPosition(-4);
	result = mc2.processLocationControl(.5);
	if (result != -0.125
			&& mc1.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -0.125, actual:" << result << std::endl;
		std::cout << " - Vel:" << mc2.getCurrentVelocity() << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: acc->dec ..... ";
	result = mc2.processLocationControl(4);
	if (result != -3.875
			&& mc1.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -3.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: dec->stp ..... ";
	result = mc2.processLocationControl(41234);
	if (result != -4 && mc1.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// This is a test that puts in times equal to the transition points:
	std::cout << "Testing Location mc3: stp->mov ..... ";
	MotionControl mc3;
	mc3.enableLocationControl();
	mc3.setMaxSpeed(1);
	mc3.setAcceleration(1);
	mc3.setGoalPosition(4);
	result = mc3.processLocationControl(1);
	if (result != 0.5 && mc1.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: mov->dec ..... ";
	result = mc3.processLocationControl(3);
	if (result != 3.5
			&& mc1.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 3.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: dec->stp ..... ";
	result = mc3.processLocationControl(1);
	if (result != 4.0 && mc1.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: stp->stp ..... ";
	result = mc3.processLocationControl(1);
	if (result != 4.0 && mc1.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// This tests short goal position
	std::cout << "Testing Location mc4: acc->dec ..... ";
	MotionControl mc4;
	mc4.enableLocationControl();
	mc4.setMaxSpeed(1);
	mc4.setAcceleration(1);
	mc4.setGoalPosition(1);
	result = mc4.processLocationControl(1.5);
	if (result != 0.875
			&& mc1.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// Tests deceleration before moving
	// Goal switch during deceleration state
	std::cout << "Testing Location mc5: acc->dec ..... ";
	MotionControl mc5;
	mc5.enableLocationControl();
	mc5.setMaxSpeed(1);
	mc5.setAcceleration(1);
	mc5.setGoalPosition(1);
	result = mc5.processLocationControl(1.5);
	if (result != 0.875
			&& mc1.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc5: dec->acc ..... ";
	double lastVel = mc5.getCurrentVelocity();
	mc5.setGoalPosition(-1);
	result = mc5.processLocationControl(0.1);
	if (result != 0.875
			&& mc5.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << "\tstate:" << MotionControl::stateToString(mc1.getState()) << std::endl;
		std::cout << " - Velocity: " << mc5.getCurrentVelocity() << "\tprior:" << lastVel << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

//	exit(0);

	std::cout << "Testing Location mc8: acc->dec ..... "; // result of regression
														  // testsing!
	MotionControl mc8;
	mc8.enableLocationControl();
	mc8.setMaxSpeed(2);
	mc8.setAcceleration(1);
	mc8.setGoalPosition(1);
	result = mc8.processLocationControl(1.5);
	if (result != 0.875
			&& mc8.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing bad max speed .............. ";
	MotionControl mc6;
	mc6.enableLocationControl();
	mc6.setGoalPosition(-200);
	mc6.setMaxSpeed(-1);
	mc6.setAcceleration(10000);
	result = mc6.processLocationControl(27846);
	if (result != 0) {
		std::cout << "FAILED: result:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing bad acceleration ........... ";
	MotionControl mc7;
	mc7.enableLocationControl();
	mc7.setMaxSpeed(2345);
	mc7.setAcceleration(-100);
	mc7.setGoalPosition(200);
	result = mc7.processLocationControl(27846);
	if (result != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing inverse acceleration setup . ";
	mc7.enableLocationControl();
	mc7.setMaxSpeed(2);
	mc7.setAcceleration(1);
	mc7.setGoalPosition(10);
	result = mc7.processLocationControl(2);
	if (mc7.getCurrentVelocity() != 2) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	std::cout << "Testing inverse acceleration ....... ";
	mc7.enableLocationControl();
	mc7.setMaxSpeed(1);
	mc7.setAcceleration(1);
	mc7.setGoalPosition(10);
	result = mc7.processLocationControl(0.5);
	if (mc7.getCurrentVelocity() != 1.5) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	std::cout << "Testing inverse acceleration 2 ..... ";
	mc7.enableLocationControl();
	mc7.setMaxSpeed(1);
	mc7.setAcceleration(1);
	mc7.setGoalPosition(10);
	result = mc7.processLocationControl(0.5);
	if (mc7.getCurrentVelocity() != 1.0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}


//	std::cout << "Testing corner case ................ "
//			<< std::setprecision(30) << std::fixed;
//	testMotionControl mc9;
//	mc9.setValuesForTesting();
//	result = mc9.processLocationControl(0.134070968627889403056873849837);
//	if (std::string(MotionControl::stateToString(mc9.getState())).compare(
//			"DECELERATING") != 0) {
//		std::cout << "FAILED" << std::endl;
//		std::cout << "state = "
//				<< MotionControl::stateToString(mc9.getState())
//				<< std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}

//	std::cout << "current state: " << MotionControl::stateToString( mc9.getState()) << std::endl;
//	std::cout << "Testing corner case part 2 ......... ";
//
//	mc9.setValuesForTesting();
//	//mc9.setGoalPosition(122.879760858075371743325376883149 + 122.879760858075357532470661681145);
//
//	result = mc9.processLocationControl(0.032784281633135492482788216767);
//	if (result <= 12.5) {
//		std::cout << "FAILED" << std::endl;
//		std::cout << "mc9.processLocationControl(10) = " << result << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}

	return allTestsPass;
}

std::string ExePath() {
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		fprintf(stdout, "Current working dir: %s\n", cwd);
	return cwd;
	}
return "";
}

bool testVector() {
	bool allTestsPass = true;

	std::cout << "Testing zero case .................. ";
	Vector goal(3), result, zero(3);
	MotionControlMultiple mc(goal.size());
	mc.setMaxSpeed(1);
	mc.setMaxAcceleration(1);
	mc.setGoal(goal);
	result = mc.process(1);
	if(result.size() != goal.size() ||
	   !matricesEqual(&result, &zero)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
	std::cout << "Testing isStopped .................. ";
	if(!mc.isStopped()) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

//	std::ofstream data;
//	data.open("data.txt");
//	goal(0) = 1;
//	goal(1) = 2;
//	goal(2) = 10;
//	mc.setMaxSpeed(1);
//	mc.setMaxAcceleration(1);
//	mc.setGoal(goal);
//	double dt = 0.01, t;
//	for ( t = 0; t <3; t += dt) {
////		std::cout << " - time: " << t << std::endl;
//		result = mc.process(dt);
//		result = mc.getCurrentVelocity();
//		data << t << " " << result(0) << " " << result(1) << " " << result(2) << std::endl;
//	}
//	goal(0) = -2;
//	goal(1) = 4;
//	goal(2) = 1;
//	mc.setGoal(goal);
//	for (; t < 10; t += dt) {
////		std::cout << " - time: " << t << std::endl;
//		result = mc.process(dt);
//		result = mc.getCurrentVelocity();
//		data << t << " " << result(0) << " " << result(1) << " " << result(2) << std::endl;
//	}
//
//	goal(0) = -2;
//	goal(1) = 4;
//	goal(2) = 10;
//	mc.setGoal(goal);
//	for (; t < 25; t += dt) {
//		//		std::cout << " - time: " << t << std::endl;
//		result = mc.process(dt);
//		result = mc.getCurrentVelocity();
//		data << t << " " << result(0) << " " << result(1) << " " << result(2) << std::endl;
//	}
//
//	goal(0) = -2;
//	goal(1) = 4;
//	goal(2) = 15;
//	mc.setGoal(goal);
//	for (; t < 35; t += dt) {
//		//		std::cout << " - time: " << t << std::endl;
//		result = mc.process(dt);
//		result = mc.getCurrentVelocity();
//		data << t << " " << result(0) << " " << result(1) << " " << result(2) << std::endl;
//	}
//
//	goal(0) = -2;
//	goal(1) = 6;
//	goal(2) = 13;
//	mc.setGoal(goal);
//	for (; t < 45; t += dt) {
//		//		std::cout << " - time: " << t << std::endl;
//		result = mc.process(dt);
//		result = mc.getCurrentVelocity();
//		data << t << " " << result(0) << " " << result(1) << " " << result(2) << std::endl;
//	}
//
//	data.close();
////	std::cout << "Saved to :" << ExePath() <<std::endl;

	std::cout << "Testing motion case ................ ";
	goal(0) = 1;
	goal(1) = 2;
	goal(2) = 10;
	mc.setMaxSpeed(1);
	mc.setMaxAcceleration(1);
	mc.setGoal(goal);
	result = mc.process(11.3);
	if(result.size() != goal.size() ||
	   !matricesEqual(&result, &goal)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}



	return allTestsPass;
}
//
//	MotionControl<Vector> mc;
//
//	std::cout << "Testing Velocity control ........... ";
//	mc.enableVelocityControl();
//	Vector location(3);
//	location(0) = .1;
//	location(1) = 100.1;
//	location(2) = -.05;
//	Vector velocity(3);
//	velocity(0) = 1;
//	velocity(1) = 1001;
//	velocity(2) = -.5;
//	mc.setVelocity(velocity);
//	Vector result = mc.processLocationControl(.1);
//	if (!matricesEqual(&location, &result)) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Velocity control 2 ......... ";
//	location(0) = 0;
//	location(1) = 0;
//	location(2) = 0;
//	velocity(0) = -1;
//	velocity(1) = -1001;
//	velocity(2) = .5;
//	mc.setVelocity(velocity);
//	result = mc.processLocationControl(.1);
//	if (!matricesEqual(&location, &result)) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control ........... ";
//	mc.enableLocationControl();
//	location(0) = 10;
//	location(1) = -10;
//	location(2) = 10;
//	mc.setMaxSpeed(1);
//	mc.setAcceleration(1);
//	mc.setGoalPosition(location);
//	result = mc.processLocationControl(1);
//	location(0) = 0.5 / sqrt(3);
//	location(1) = -0.5 / sqrt(3);
//	location(2) = 0.5 / sqrt(3);
//	if (!matricesEqual(&location, &result)
//			&& mc.getState() != MotionControl<Vector>::ACCELERATION) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 2 ......... ";
//	location(0) = 10;
//	location(1) = -10;
//	location(2) = 10;
//	result = mc.processLocationControl(20);
//	if (!matricesEqual(&location, &result)
//			&& mc.getState() != MotionControl<Vector>::STOPPED) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 3 ......... ";
//	location(0) = 0;
//	location(1) = 0;
//	location(2) = 0;
//	mc.setGoalPosition(location);
//	result = mc.processLocationControl(20);
//	if (!matricesEqual(&location, &result)
//			&& mc.getState() != MotionControl<Vector>::STOPPED) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	MotionControl<Vector> mc2;
//	std::cout << "Testing Location control 4 ......... ";
//	location(0) = 2.0 / sqrt(3);
//	location(1) = 2.0 / sqrt(3);
//	location(2) = -2.0 / sqrt(3);
//	mc2.setGoalPosition(location);
//	mc2.setMaxSpeed(1);
//	mc2.setAcceleration(1);
//	mc2.enableLocationControl();
//	for (int i = 0; i < 9999; i++) {
//		result = mc2.processLocationControl(.0001);
//		if (mc2.getState() != MotionControl<Vector>::ACCELERATION) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			allTestsPass = false;
//		}
//	}
//	location(0) = .5 / sqrt(3);
//	location(1) = .5 / sqrt(3);
//	location(2) = -.5 / sqrt(3);
//	result = mc2.processLocationControl(.0001);
//	if (!matricesEqual(&location, &result)
//			|| mc2.getState() != MotionControl<Vector>::MOVING) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 5 ......... ";
//	for (int i = 0; i < 9999; i++) {
//		result = mc2.processLocationControl(.0001);
//		if (mc2.getState() != MotionControl<Vector>::MOVING) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			allTestsPass = false;
//		}
//	}
//	location(0) = 1.5 / sqrt(3);
//	location(1) = 1.5 / sqrt(3);
//	location(2) = -1.5 / sqrt(3);
//	result = mc2.processLocationControl(.0001);
//	if (!matricesEqual(&location, &result)
//			|| mc2.getState() != MotionControl<Vector>::DECELERATING) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 6 ......... ";
//	for (int i = 0; i < 99; i++) {
//		result = mc2.processLocationControl(.01);
//		if (mc2.getState() != MotionControl<Vector>::DECELERATING) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			allTestsPass = false;
//		}
//	}
//	location(0) = 2 / sqrt(3);
//	location(1) = 2 / sqrt(3);
//	location(2) = -2 / sqrt(3);
//	result = mc2.processLocationControl(.01);
//	if (!matricesEqual(&location, &result)
//			|| mc2.getState() != MotionControl<Vector>::STOPPED) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 7 ......... ";
//	location(0) = 1 / sqrt(3);
//	location(1) = 1 / sqrt(3);
//	location(2) = -1 / sqrt(3);
//	mc2.setGoalPosition(location);
//	for (int i = 0; i < 99; i++) {
//		result = mc2.processLocationControl(.01);
//		if (mc2.getState() != MotionControl<Vector>::ACCELERATION) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			allTestsPass = false;
//		}
//	}
//	location(0) = 1.5 / sqrt(3);
//	location(1) = 1.5 / sqrt(3);
//	location(2) = -1.5 / sqrt(3);
//	result = mc2.processLocationControl(.01);
//	if (!matricesEqual(&location, &result)
//			|| mc2.getState() != MotionControl<Vector>::DECELERATING) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	std::cout << "Testing Location control 8 ......... ";
//	location(0) = 1 / sqrt(3);
//	location(1) = 1 / sqrt(3);
//	location(2) = -1 / sqrt(3);
//	mc2.setGoalPosition(location);
//	for (int i = 0; i < 99; i++) {
//		result = mc2.processLocationControl(.01);
//		if (mc2.getState() != MotionControl<Vector>::DECELERATING) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			allTestsPass = false;
//		}
//	}
//	location(0) = 1. / sqrt(3);
//	location(1) = 1. / sqrt(3);
//	location(2) = -1. / sqrt(3);
//	result = mc2.processLocationControl(.01);
//	if (!matricesEqual(&location, &result)
//			|| mc2.getState() != MotionControl<Vector>::STOPPED) {
//		std::cout << "FAILED" << std::endl;
//		allTestsPass = false;
//	} else {
//		std::cout << "Passed" << std::endl;
//	}
//
//	return allTestsPass;
//}

bool testAngular() {
	bool allTestsPass = true;

	MotionControlAngular mc;

	std::cout << "Testing Location control ........... ";
	mc.enableLocationControl();
	mc.setMaxSpeed(1);
	mc.setAcceleration(1);
	mc.setGoalPosition((double) -1 + (double) 2 * (double) MOGI_PI);
	double result = mc.processLocationControl(2);
	if (fabs(result - (double) -1) > 0.000000000001
			&& mc.getState() != MotionControl::STOPPED) { // some floating point errors
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -1, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control ........... ";  // should be at rest.
	mc.processLocationControl(100);
	if (mc.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 2 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	result = mc.processLocationControl(.0001);
	if (mc.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - state: " << MotionControl::stateToString(mc.getState()) << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 3 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl::ACCELERATION) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) -0.4999) > 0.0000001
			|| mc.getState() != MotionControl::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -0.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 4 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 19998; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl::MOVING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) 1.5) > 0.0000001) {  //||
		// mc.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.5, actual: " << result << std::endl;
		std::cout << " - Expected: "
				<< MotionControlAngular::stateToString(
						MotionControlAngular::DECELERATING) << ", actual: "
				<< MotionControlAngular::stateToString(mc.getState())
				<< std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 5 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl::DECELERATING) {
//			std::cout << "FAILED at i = " << i << std::endl;
//			std::cout << " - Expected: "
//			<< MotionControlAngular::stateToString(
//												   MotionControlAngular::DECELERATING) << ", actual: "
//			<< MotionControlAngular::stateToString(mc.getState())
//			<< std::endl;

			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.001);
	if (fabs(result - (double) 2) > 0.00000001
			|| mc.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing short dist control 1 ....... ";
	mc.setGoalPosition((double) 1 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
//		std::cout << "i = " << i;
		result = mc.processLocationControl(.0001);
//		std::cout << "\tstate:" << MotionControl::stateToString(mc.getState());
//		std::cout << "\tvel:" << mc.getCurrentVelocity();
//		std::cout << "\tpos:" << mc.getCurrentPosition() << std::endl;
		if (mc.getState() != MotionControl::ACCELERATION) {
			std::cout << "FAILED at i = " << i << "\tpos:" << mc.getCurrentPosition() << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001001);
	if (fabs(result - (double) 1.5) > 0.0000001
			|| mc.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.5, actual:" << result << ", actual: "
		<< MotionControlAngular::stateToString(mc.getState())
		<< std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing short dist control 2 ....... ";
	mc.setGoalPosition((double) 1 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl::DECELERATING) {
//			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.001);
	if (fabs(result - (double) 1) > 0.000000000001
			|| mc.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	MotionControlAngular mc2;
	mc2.enableLocationControl();
	mc2.setAcceleration(1);
	mc2.setMaxSpeed(1);
	mc2.setGoalPosition(-2);
	result = mc2.processLocationControl(10);

	std::cout << "Testing heading reversed setup ..... ";
	if (fabs(result - (-2.0)) > 0.000001
			|| mc2.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	mc2.setGoalPosition(2);
	std::ofstream data;
	data.open("data.txt");
	double dtime = 0.01;
	double time = 0;
	for (; time < 1.6; time += dtime) {
		result = mc2.processLocationControl(dtime);
		data << time << " " << result << " " << mc2.getCurrentVelocity() << std::endl;
	}

	mc2.setGoalPosition(-2);
	for (; time < 10; time += dtime) {
		result = mc2.processLocationControl(dtime);
		data << time << " " << result << " " << mc2.getCurrentVelocity() << std::endl;
	}


	data.close();

	mc2.setGoalPosition(-2);
	mc2.processLocationControl(100);

	std::cout << "Testing heading -2 to +2, p=-2.5 ... ";
	mc2.setGoalPosition(2);
	result = mc2.processLocationControl(1);
	if (fabs(result - (-2.5)) > 0.000001
			|| mc2.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading -2 to +2, p=2.5 .... ";
	std::cout << "time: " << ((double) MOGI_PI - (double) 3.0) * (double) 2.0 + (double) 1.0 << std::endl;
	std::cout << "vel " << mc2.getCurrentVelocity() << std::endl;
	std::cout << "pos " << mc2.getCurrentPosition() << std::endl;
	std::cout << "stop " << mc2.getSoonestStoppingLocation() << std::endl;
	std::cout << "time " << mc2.getTimeToStop() << std::endl;
	result = mc2.processLocationControl( ((double) MOGI_PI - (double) 3.0) * (double) 2.0 + (double) 1.0);
	if (fabs(result - (2.5)) > 0.000001
			|| mc2.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading -2 to +2, p=2 ...... ";
	result = mc2.processLocationControl(1.0);
	if (fabs(result - (2)) > 0.000001
			|| mc2.getState() != MotionControl::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}
mc2.processLocationControl(10);
	std::cout << "Testing heading +2 to +1, p=1.5 .... ";
	mc2.setGoalPosition(1);
	result = mc2.processLocationControl(1);
	if (fabs(result - (1.5)) > 0.000001
		|| mc2.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading +2 to +1, p=1.125 .. ";
	result = mc2.processLocationControl(.5);
	if (fabs(result - (1.125)) > 0.000001) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.125, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing goal +1.125@1.125, p=1.125 . ";
	mc2.setGoalPosition(result);
	result = mc2.processLocationControl(0);
	if (fabs(result - (1.125)) > 0.000001
		|| mc2.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing goal set +1.5, p=1.0 ....... ";
	mc2.setGoalPosition(1.5);
	result = mc2.processLocationControl(0.5);
	if (fabs(result - (1.0)) > 0.000001) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.75, actual:" << result << "\tstate:" << MotionControlAngular::stateToString( mc2.getState()) << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing goal set +1.5, p=1.125 ..... ";
	result = mc2.processLocationControl(0.5);
	if (fabs(result - (1.125)) > 0.000001) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.125, actual:" << result << "\tstate:" << MotionControlAngular::stateToString( mc2.getState()) << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	MotionControlAngular mc3;
	mc3.enableLocationControl();
	mc3.setAcceleration(MOGI_PI);
	mc3.setMaxSpeed(MOGI_PI);
	mc3.setGoalPosition(-1);

	std::cout << "Testing goal set -1.0, p=1.0 ....... ";
	result = mc3.processLocationControl(0.7);
	if (fabs(result - (-0.711745191138210664405505667673)) > 0.000001
		|| mc3.getState() != MotionControl::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -0.71174, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing goal set -1.0, p=1.0 ....... ";
	mc3.setGoalPosition(result);
	result = mc3.processLocationControl(0.0001);
	if (fabs(result - (-0.711879786130602099092357093468)) > 0.000001
		|| mc3.getState() != MotionControl::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.125, actual:" << result << "\tstate:" << MotionControlAngular::stateToString( mc3.getState()) << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}



	return allTestsPass;
}
