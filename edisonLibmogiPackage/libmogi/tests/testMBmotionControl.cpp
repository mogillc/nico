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

#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>
#include <iomanip>
#include "systems.h"

#include <math.h>

#include <float.h>

using namespace Mogi::Math;

class testMotionControl: public MotionControl<double> {
public:
	void setValuesForTesting() {
		setMaxSpeed(100);
		maxAcceleration = 10.000000000000000000000000000000;
		speed = 0.302916903840337703446294881360;
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

	MotionControl<double> mc;

	std::cout << "Testing Velocity control ........... ";
	mc.enableVelocityControl();
	mc.setVelocity(2);
	double result = mc.processLocationControl(.1);
	if (result != .2 && mc.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 2 ......... ";
	mc.setVelocity(.2);
	result = mc.processLocationControl(4);
	if (result != 1 && mc.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 3 ......... ";
	mc.setVelocity(-1);
	result = mc.processLocationControl(1);
	if (result != 0 && mc.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 4 ......... ";
	mc.setVelocity(0);
	result = mc.processLocationControl(100);
	if (result != 0 && mc.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: stp->mov ..... ";
	MotionControl<double> mc1;
	mc1.enableLocationControl();
	mc1.setMaxSpeed(1);
	mc1.setAcceleration(1);
	mc1.setGoalPosition(4);
	result = mc1.processLocationControl(.75);
	result = mc1.processLocationControl(.75);
	if (result != 1.0 && mc1.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: mov->mov ..... ";
	result = mc1.processLocationControl(2);
	if (result != 3.0 && mc1.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 3.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc1: mov->stp ..... ";
	result = mc1.processLocationControl(1.5);
	if (result != 4.0 && mc1.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: stp->acc ..... ";
	MotionControl<double> mc2;
	mc2.enableLocationControl();
	mc2.setMaxSpeed(1);
	mc2.setAcceleration(1);
	mc2.setGoalPosition(-4);
	result = mc2.processLocationControl(.5);
	if (result != -0.125
			&& mc1.getState() != MotionControl<double>::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -0.125, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: acc->dec ..... ";
	result = mc2.processLocationControl(4);
	if (result != -3.875
			&& mc1.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -3.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc2: dec->stp ..... ";
	result = mc2.processLocationControl(41234);
	if (result != -4 && mc1.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// This is a test that puts in times equal to the transition points:
	std::cout << "Testing Location mc3: stp->mov ..... ";
	MotionControl<double> mc3;
	mc3.enableLocationControl();
	mc3.setMaxSpeed(1);
	mc3.setAcceleration(1);
	mc3.setGoalPosition(4);
	result = mc3.processLocationControl(1);
	if (result != 0.5 && mc1.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: mov->dec ..... ";
	result = mc3.processLocationControl(3);
	if (result != 3.5
			&& mc1.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 3.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: dec->stp ..... ";
	result = mc3.processLocationControl(1);
	if (result != 4.0 && mc1.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc3: stp->stp ..... ";
	result = mc3.processLocationControl(1);
	if (result != 4.0 && mc1.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 4.0, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// This tests short goal position
	std::cout << "Testing Location mc4: acc->dec ..... ";
	MotionControl<double> mc4;
	mc4.enableLocationControl();
	mc4.setMaxSpeed(1);
	mc4.setAcceleration(1);
	mc4.setGoalPosition(1);
	result = mc4.processLocationControl(1.5);
	if (result != 0.875
			&& mc1.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	// Tests deceleration before moving
	// Goal switch during deceleration state
	std::cout << "Testing Location mc5: acc->dec ..... ";
	MotionControl<double> mc5;
	mc5.enableLocationControl();
	mc5.setMaxSpeed(1);
	mc5.setAcceleration(1);
	mc5.setGoalPosition(1);
	result = mc5.processLocationControl(1.5);
	if (result != 0.875
			&& mc1.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc5: dec->acc ..... ";
	mc5.setGoalPosition(-1);
	result = mc5.processLocationControl(1);
	if (result != 0.875
			&& mc1.getState() != MotionControl<double>::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location mc8: acc->dec ..... "; // result of regression
														  // testsing!
	MotionControl<double> mc8;
	mc8.enableLocationControl();
	mc8.setMaxSpeed(2);
	mc8.setAcceleration(1);
	mc8.setGoalPosition(1);
	result = mc8.processLocationControl(1.5);
	if (result != 0.875
			&& mc1.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 0.875, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing bad max speed .............. ";
	MotionControl<double> mc6;
	mc6.enableLocationControl();
	mc6.setGoalPosition(200);
	mc6.setMaxSpeed(-1);
	mc6.setAcceleration(10000);
	result = mc.processLocationControl(27846);
	if (result != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing bad acceleration ........... ";
	MotionControl<double> mc7;
	mc7.enableLocationControl();
	mc7.setMaxSpeed(2345);
	mc.setAcceleration(-100);
	mc7.setGoalPosition(200);
	result = mc.processLocationControl(27846);
	if (result != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing corner case ................ "
			<< std::setprecision(30) << std::fixed;
	testMotionControl mc9;
	mc9.setValuesForTesting();
	result = mc9.processLocationControl(0.134070968627889403056873849837);
	if (std::string(MotionControl<double>::stateToString(mc9.getState())).compare(
			"DECELERATING") != 0) {
		std::cout << "FAILED" << std::endl;
		std::cout << "state = "
				<< MotionControl<double>::stateToString(mc9.getState())
				<< std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

//	std::cout << "current state: " << MotionControl<double>::stateToString( mc9.getState()) << std::endl;
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

bool testVector() {
	bool allTestsPass = true;

	MotionControl<Vector> mc;

	std::cout << "Testing Velocity control ........... ";
	mc.enableVelocityControl();
	Vector location(3);
	location(0) = .1;
	location(1) = 100.1;
	location(2) = -.05;
	Vector velocity(3);
	velocity(0) = 1;
	velocity(1) = 1001;
	velocity(2) = -.5;
	mc.setVelocity(velocity);
	Vector result = mc.processLocationControl(.1);
	if (!matricesEqual(&location, &result)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Velocity control 2 ......... ";
	location(0) = 0;
	location(1) = 0;
	location(2) = 0;
	velocity(0) = -1;
	velocity(1) = -1001;
	velocity(2) = .5;
	mc.setVelocity(velocity);
	result = mc.processLocationControl(.1);
	if (!matricesEqual(&location, &result)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control ........... ";
	mc.enableLocationControl();
	location(0) = 10;
	location(1) = -10;
	location(2) = 10;
	mc.setMaxSpeed(1);
	mc.setAcceleration(1);
	mc.setGoalPosition(location);
	result = mc.processLocationControl(1);
	location(0) = 0.5 / sqrt(3);
	location(1) = -0.5 / sqrt(3);
	location(2) = 0.5 / sqrt(3);
	if (!matricesEqual(&location, &result)
			&& mc.getState() != MotionControl<Vector>::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 2 ......... ";
	location(0) = 10;
	location(1) = -10;
	location(2) = 10;
	result = mc.processLocationControl(20);
	if (!matricesEqual(&location, &result)
			&& mc.getState() != MotionControl<Vector>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 3 ......... ";
	location(0) = 0;
	location(1) = 0;
	location(2) = 0;
	mc.setGoalPosition(location);
	result = mc.processLocationControl(20);
	if (!matricesEqual(&location, &result)
			&& mc.getState() != MotionControl<Vector>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	MotionControl<Vector> mc2;
	std::cout << "Testing Location control 4 ......... ";
	location(0) = 2.0 / sqrt(3);
	location(1) = 2.0 / sqrt(3);
	location(2) = -2.0 / sqrt(3);
	mc2.setGoalPosition(location);
	mc2.setMaxSpeed(1);
	mc2.setAcceleration(1);
	mc2.enableLocationControl();
	for (int i = 0; i < 9999; i++) {
		result = mc2.processLocationControl(.0001);
		if (mc2.getState() != MotionControl<Vector>::ACCELERATION) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	location(0) = .5 / sqrt(3);
	location(1) = .5 / sqrt(3);
	location(2) = -.5 / sqrt(3);
	result = mc2.processLocationControl(.0001);
	if (!matricesEqual(&location, &result)
			|| mc2.getState() != MotionControl<Vector>::MOVING) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 5 ......... ";
	for (int i = 0; i < 9999; i++) {
		result = mc2.processLocationControl(.0001);
		if (mc2.getState() != MotionControl<Vector>::MOVING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	location(0) = 1.5 / sqrt(3);
	location(1) = 1.5 / sqrt(3);
	location(2) = -1.5 / sqrt(3);
	result = mc2.processLocationControl(.0001);
	if (!matricesEqual(&location, &result)
			|| mc2.getState() != MotionControl<Vector>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 6 ......... ";
	for (int i = 0; i < 99; i++) {
		result = mc2.processLocationControl(.01);
		if (mc2.getState() != MotionControl<Vector>::DECELERATING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	location(0) = 2 / sqrt(3);
	location(1) = 2 / sqrt(3);
	location(2) = -2 / sqrt(3);
	result = mc2.processLocationControl(.01);
	if (!matricesEqual(&location, &result)
			|| mc2.getState() != MotionControl<Vector>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 7 ......... ";
	location(0) = 1 / sqrt(3);
	location(1) = 1 / sqrt(3);
	location(2) = -1 / sqrt(3);
	mc2.setGoalPosition(location);
	for (int i = 0; i < 99; i++) {
		result = mc2.processLocationControl(.01);
		if (mc2.getState() != MotionControl<Vector>::ACCELERATION) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	location(0) = 1.5 / sqrt(3);
	location(1) = 1.5 / sqrt(3);
	location(2) = -1.5 / sqrt(3);
	result = mc2.processLocationControl(.01);
	if (!matricesEqual(&location, &result)
			|| mc2.getState() != MotionControl<Vector>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing Location control 8 ......... ";
	location(0) = 1 / sqrt(3);
	location(1) = 1 / sqrt(3);
	location(2) = -1 / sqrt(3);
	mc2.setGoalPosition(location);
	for (int i = 0; i < 99; i++) {
		result = mc2.processLocationControl(.01);
		if (mc2.getState() != MotionControl<Vector>::DECELERATING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	location(0) = 1. / sqrt(3);
	location(1) = 1. / sqrt(3);
	location(2) = -1. / sqrt(3);
	result = mc2.processLocationControl(.01);
	if (!matricesEqual(&location, &result)
			|| mc2.getState() != MotionControl<Vector>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

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
			&& mc.getState() != MotionControl<double>::STOPPED) { // some floating point errors
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -1, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control ........... ";  // should be at rest.
	mc.processLocationControl(100);
	if (mc.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 2 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	result = mc.processLocationControl(.0001);
	if (mc.getState() != MotionControl<double>::ACCELERATION) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing isMoving control 3 ......... ";
	mc.setGoalPosition((double) 2 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl<double>::ACCELERATION) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) -0.4999) > 0.0000001
			|| mc.getState() != MotionControl<double>::MOVING) {
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
		if (mc.getState() != MotionControl<double>::MOVING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) 1.5) > 0.0000001) {  //||
		// mc.getState() != MotionControl<double>::DECELERATING) {
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
		if (mc.getState() != MotionControl<double>::DECELERATING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) 2) > 0.00000001
			|| mc.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing short dist control 1 ....... ";
	mc.setGoalPosition((double) 1 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl<double>::ACCELERATION) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) 1.5) > 0.0000001
			|| mc.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 1.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing short dist control 2 ....... ";
	mc.setGoalPosition((double) 1 + (double) 2 * (double) MOGI_PI);
	for (int i = 0; i < 9999; i++) {
		// std::cout << "i = " << i << std::endl;
		result = mc.processLocationControl(.0001);
		if (mc.getState() != MotionControl<double>::DECELERATING) {
			std::cout << "FAILED at i = " << i << std::endl;
			allTestsPass = false;
		}
	}
	result = mc.processLocationControl(.0001);
	if (fabs(result - (double) 1) > 0.000000000001
			|| mc.getState() != MotionControl<double>::STOPPED) {
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
			|| mc2.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -2, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading -2 to +2, p=-2.5 ... ";
	mc2.setGoalPosition(2);
	result = mc2.processLocationControl(1);
	if (fabs(result - (-2.5)) > 0.000001
			|| mc2.getState() != MotionControl<double>::MOVING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: -2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading -2 to +2, p=2.5 .... ";
	result = mc2.processLocationControl(
			((double) MOGI_PI - (double) 3.0) * (double) 2.0 + (double) 1.0);
	if (fabs(result - (2.5)) > 0.000001
			|| mc2.getState() != MotionControl<double>::DECELERATING) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing heading -2 to +2, p=2 ...... ";
	result = mc2.processLocationControl(1.0);
	if (fabs(result - (2)) > 0.000001
			|| mc2.getState() != MotionControl<double>::STOPPED) {
		std::cout << "FAILED" << std::endl;
		std::cout << " - Expected: 2.5, actual:" << result << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}
