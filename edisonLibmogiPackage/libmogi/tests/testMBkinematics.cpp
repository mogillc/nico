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

#include "kinematics.h"
#include "node.h"

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

bool testSetters();
bool test3DOF();
bool test6DOF();
bool testYYX();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning Setter tests:" << std::endl;
	allTestsPass = testSetters() ? allTestsPass : false;
	std::cout << " - Beginning 3DOF ZYY tests:" << std::endl;
	allTestsPass = test3DOF() ? allTestsPass : false;
	std::cout << " - Beginning 6DOF tests:" << std::endl;
	allTestsPass = test6DOF() ? allTestsPass : false;
	std::cout << " - Beginning YYX tests:" << std::endl;
	allTestsPass = testYYX() ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testSetters() {
	bool testPasses = true;

	InverseKinematics* myIK = InverseKinematics::create(InverseKinematics::ZYY);

	Vector link0(3);
	for (int i = 0; i < 4; i++) {
		std::cout << "Testing setting link " << i << " for 3DOF ... ";
		if (myIK->setLink(i, link0) != KINEMATICS_NO_ERROR) {
			testPasses = false;
			std::cout << "FAILED" << std::endl;
		} else {
			std::cout << "Passed" << std::endl;
		}
	}

	std::cout << "Testing setting link 4 for 3DOF ... ";
	if (myIK->setLink(4, link0) != KINEMATICS_OUT_OF_RANGE) {
		testPasses = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "Testing setting link -1 for 3DOF .. ";
	if (myIK->setLink(-1, link0) != KINEMATICS_OUT_OF_RANGE) {
		testPasses = false;
		std::cout << "FAILED" << std::endl;
	} else {
		std::cout << "Passed" << std::endl;
	}

	delete myIK;

	return testPasses;
}

bool test3DOF() {
	bool testPasses = true;

	InverseKinematics* myIK = InverseKinematics::create(InverseKinematics::ZYY);
//  myIK->barfExplode = 1;

	Vector baseLocation(3);
	baseLocation.name("baseLocation");
	Quaternion baseOrientation;
	baseOrientation.name("baseOrientation");

	myIK->setBase(baseLocation, baseOrientation);
	Vector link0(3), link1(3), link2(3), link3(3);

	// link0(0) = 0;	// base
	link1(2) = 1;  // coxa
	link2(0) = 1;  // femur
	link3(0) = 1;  // tibia

	// myIK->setLink(0, link0);
	myIK->setLink(1, link1);
	myIK->setLink(2, link2);
	myIK->setLink(3, link2);

	Vector eeLocation(3);
	Vector expectedResult(3);
	expectedResult.name("expectedResult");

	std::cout << "Testing IK sol 1 for 3DOF ......... ";
	eeLocation(0) = 2;
	eeLocation(1) = 0;
	eeLocation(2) = 1;

	if (myIK->compute(eeLocation) == KINEMATICS_NO_ERROR) {
		Vector result = myIK->getMotorAngles();
		result.name("result");

		if (!matricesEqual(&result, &expectedResult)) {
			std::cout << "FAILED: bad result" << std::endl;
			testPasses = false;
		} else {
			std::cout << "Passed" << std::endl;
		}

	} else {
		std::cout << "FAILED: compute() resturned error" << std::endl;
		testPasses = false;
	}

	std::cout << "Testing IK sol 2 for 3DOF ......... ";
	eeLocation(0) = 2.0 / sqrt(2);
	eeLocation(1) = 2.0 / sqrt(2);
	eeLocation(2) = 1;

	expectedResult(0) = MOGI_PI / 4.0;
	expectedResult(1) = 0;
	expectedResult(2) = 0;

	if (myIK->compute(eeLocation) == KINEMATICS_NO_ERROR) {
		Vector result = myIK->getMotorAngles();
		result.name("result");

		if (!matricesEqual(&result, &expectedResult)) {
			std::cout << "FAILED: bad result" << std::endl;
			testPasses = false;
		} else {
			std::cout << "Passed" << std::endl;
		}

	} else {
		std::cout << "FAILED: compute() resturned error" << std::endl;
		testPasses = false;
	}

	std::cout << "Testing IK sol 3 for 3DOF ......... ";
	eeLocation(0) = 0;
	eeLocation(1) = 2.0 / sqrt(2);
	eeLocation(2) = 1 + 2.0 / sqrt(2);

	expectedResult(0) = MOGI_PI / 2.0;
	expectedResult(1) = -MOGI_PI / 4.0;
	expectedResult(2) = 0;

	if (myIK->compute(eeLocation) == KINEMATICS_NO_ERROR) {
		Vector result = myIK->getMotorAngles();
		result.name("result");

		if (!matricesEqual(&result, &expectedResult)) {
			std::cout << "FAILED: bad result" << std::endl;
			testPasses = false;
		} else {
			std::cout << "Passed" << std::endl;
		}

	} else {
		std::cout << "FAILED: compute() resturned error" << std::endl;
		testPasses = false;
	}

	delete myIK;

	return testPasses;
}

bool test6DOF() {
	bool allTestsPass = true;

	InverseKinematics* ik = InverseKinematics::create(
			InverseKinematics::ZYYXZY);

	Vector link0(3), link1(3), link2(3), link3(3), link4(3), link5(3), link6(3);

	Vector baseLocation(3);
	Quaternion baseOrientation;
	ik->setBase(baseLocation, baseOrientation);

	// link0(0) = 0;	// base
	link1(2) = 100;  // coxa
	link2(0) = 100;  // femur
	link3(2) = 10;   // tibia
	link4(0) = 100;
	link5(0) = 100;
	link6(0) = 100;

	// myIK->setLink(0, link0);
	ik->setLink(1, link1);
	ik->setLink(2, link2);
	ik->setLink(3, link3);
	ik->setLink(4, link4);
	ik->setLink(5, link5);
	ik->setLink(6, link6);

	std::cout << "Testing 6DOF ...................... ";
	Quaternion orientation;
	orientation.makeFromAngleAndAxis(MOGI_PI / 2.0, Vector::yAxis);
	Vector location(3);
	location(0) = 110;
	location(2) = -200;

	Vector angles;
	Vector locationForIK = makeEndEffectorVector(location, orientation);
	KinematicsStatus status = ik->compute(locationForIK);
	if (status == KINEMATICS_NO_ERROR) {
		angles = ik->getMotorAngles();
		Vector correctAngles(6);
		correctAngles(2) = MOGI_PI / 2.0;
		if (matricesEqual(&angles, &correctAngles)) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED: bad result" << std::endl;
			allTestsPass = false;
		}
	} else {
		std::cout << "FAILED: ";
		switch (status) {
		case KINEMATICS_BAD_RESULT:
			std::cout << "KINEMATICS_BAD_RESULT" << std::endl;
			break;

		case KINEMATICS_BAD_CONFIGURATION:
			std::cout << "KINEMATICS_BAD_CONFIGURATION" << std::endl;
			break;

		case KINEMATICS_INVALID_PARAMETER:
			std::cout << "KINEMATICS_INVALID_PARAMETER" << std::endl;
			break;

		case KINEMATICS_OVER_MAX_ERROR:
			std::cout << "KINEMATICS_OVER_MAX_ERROR" << std::endl;
			break;

		case KINEMATICS_OUT_OF_RANGE:
			std::cout << "KINEMATICS_OUT_OF_RANGE" << std::endl;
			break;

		default:
			std::cout << "wtf..." << std::endl;
			break;
		}

		allTestsPass = false;
	}

	std::cout << "Testing 6DOF 2 .................... ";
	orientation.makeFromAngleAndAxis(0, Vector::yAxis);
	location(0) = 210;
	location(2) = -100;

	locationForIK = makeEndEffectorVector(location, orientation);
	status = ik->compute(locationForIK);
	if (status == KINEMATICS_NO_ERROR) {
		angles = ik->getMotorAngles();
		Vector correctAngles(6);
		correctAngles(2) = MOGI_PI / 2.0;
		correctAngles(5) = -MOGI_PI / 2.0;
		if (matricesEqual(&angles, &correctAngles)) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED: bad result" << std::endl;
			allTestsPass = false;
		}
	} else {
		std::cout << "FAILED: ";
		switch (status) {
		case KINEMATICS_BAD_RESULT:
			std::cout << "KINEMATICS_BAD_RESULT" << std::endl;
			break;

		case KINEMATICS_BAD_CONFIGURATION:
			std::cout << "KINEMATICS_BAD_CONFIGURATION" << std::endl;
			break;

		case KINEMATICS_INVALID_PARAMETER:
			std::cout << "KINEMATICS_INVALID_PARAMETER" << std::endl;
			break;

		case KINEMATICS_OVER_MAX_ERROR:
			std::cout << "KINEMATICS_OVER_MAX_ERROR" << std::endl;
			break;

		case KINEMATICS_OUT_OF_RANGE:
			std::cout << "KINEMATICS_OUT_OF_RANGE" << std::endl;
			break;

		default:
			std::cout << "wtf..." << std::endl;
			break;
		}

		allTestsPass = false;
	}

	delete ik;
	return allTestsPass;
}

class YYXtest {
public:
	Node base;
	Node* coxa;
	Node* femur;
	Node* tibia;
	Node* foot;

	Vector link0, link1, link2, link3;

	InverseKinematics* myIK;

	Vector expectedResult;

	YYXtest() {
		link0.setLength(3);
		link1.setLength(3);
		link2.setLength(3);
		link3.setLength(3);

		expectedResult.setLength(3);

		expectedResult.name("expectedResult");

		base.name = "base";
		coxa = base.addNode("coxa");
		femur = coxa->addNode("femur");
		tibia = femur->addNode("tibia");
		foot = tibia->addNode("foot");

		myIK = InverseKinematics::create(InverseKinematics::YYX);

		Vector baseLocation(3);
		baseLocation.name("baseLocation");
		Quaternion baseOrientation;
		baseOrientation.name("baseOrientation");

		myIK->setBase(baseLocation, baseOrientation);

	}

	bool test() {
		bool testPasses = true;

		myIK->setLink(0, link0);
		myIK->setLink(1, link1);
		myIK->setLink(2, link2);
		myIK->setLink(3, link3);

		base.setLocation(link0);
		coxa->setLocation(link1);
		femur->setLocation(link2);
		tibia->setLocation(link3);
		base.setOrientation(expectedResult(0), Vector::yAxis);
		coxa->setOrientation(expectedResult(1), Vector::yAxis);
		femur->setOrientation(expectedResult(2), Vector::xAxis);

		base.update();
		//foot->getModelMatrix()->print_stats();
		Vector eeLocation(3);
		eeLocation(0) = foot->getModelMatrix()->valueAsConst(0, 3);
		eeLocation(1) = foot->getModelMatrix()->valueAsConst(1, 3);
		eeLocation(2) = foot->getModelMatrix()->valueAsConst(2, 3);

		if (myIK->compute(eeLocation) == KINEMATICS_NO_ERROR) {
			Vector result = myIK->getMotorAngles();
			result.name("result");

			if (!matricesEqual(&result, &expectedResult)) {
				std::cout << "FAILED: bad result" << std::endl;
				testPasses = false;
			} else {
				std::cout << "Passed" << std::endl;
			}

		} else {
			std::cout << "FAILED: compute() returned error" << std::endl;
			testPasses = false;
		}

		return testPasses;
	}
};

bool testYYX() {
	bool testPasses = true;

	YYXtest yyxTest;

	std::cout << "Testing IK sol 1 for YYX .......... ";
	//link0(0) = 1;	// base
	yyxTest.link1(0) = 2;  // coxa
	yyxTest.link2(0) = 1;  // femur
	yyxTest.link3(2) = -1.1;  // tibia
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1a for YYX ......... ";
	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = .1;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1b for YYX ......... ";
	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = .1;
	yyxTest.expectedResult(2) = 0;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1c for YYX ......... ";
	yyxTest.expectedResult(0) = .1;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = 0;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1d for YYX ......... ";
	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = -.1;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1e for YYX ......... ";
	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = -.1;
	yyxTest.expectedResult(2) = 0;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 1f for YYX ......... ";
	yyxTest.expectedResult(0) = -.1;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = 0;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 2 for YYX .......... ";
	yyxTest.link0(0) = 1;	// base
	yyxTest.link0(1) = 1;	// base
	yyxTest.link0(2) = 1;	// base
	yyxTest.link1(0) = 1;  // coxa
	yyxTest.link1(1) = 1;  // coxa
	yyxTest.link1(2) = 1;  // coxa
	yyxTest.link2(0) = 1;  // femur
	yyxTest.link2(1) = 1;  // femur
	yyxTest.link2(2) = 1;  // femur
	yyxTest.link3(0) = 1;  // tibia
	yyxTest.link3(1) = 1;  // tibia
	yyxTest.link3(2) = -3;  // tibia

	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = 0;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK femur for YYX .......... ";
	yyxTest.link0(0) = 0;	// base
	yyxTest.link0(1) = 0;	// base
	yyxTest.link0(2) = 0;	// base
	yyxTest.link1(0) = 1;  // coxa
	yyxTest.link1(1) = 0;  // coxa
	yyxTest.link1(2) = 0;  // coxa
	yyxTest.link2(0) = 1;  // femur
	yyxTest.link2(1) = 0;  // femur
	yyxTest.link2(2) = 0;  // femur
	yyxTest.link3(0) = 1;  // tibia
	yyxTest.link3(1) = 1;  // tibia
	yyxTest.link3(2) = 0;  // tibia

	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = -MOGI_PI / 2;

	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK femur 2 for YYX ........ ";

	yyxTest.expectedResult(0) = 0;
	yyxTest.expectedResult(1) = 0;
	yyxTest.expectedResult(2) = -MOGI_PI / 2;

	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 3 for YYX .......... ";
	yyxTest.link0(0) = 1;	// base
	yyxTest.link0(1) = -1;	// base
	yyxTest.link0(2) = 1;	// base
	yyxTest.link1(0) = 1;  // coxa
	yyxTest.link1(1) = -1;  // coxa
	yyxTest.link1(2) = 1;  // coxa
	yyxTest.link2(0) = 1;  // femur
	yyxTest.link2(1) = -1;  // femur
	yyxTest.link2(2) = 1;  // femur
	yyxTest.link3(0) = 1;  // tibia
	yyxTest.link3(1) = -1;  // tibia
	yyxTest.link3(2) = 1;  // tibia

	yyxTest.expectedResult(0) = -MOGI_PI / 4.0;
	yyxTest.expectedResult(1) = MOGI_PI / 4.0;
	yyxTest.expectedResult(2) = -MOGI_PI;

	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 4 for YYX .......... ";
	yyxTest.link0(0) = 1;	// base
	yyxTest.link0(1) = -1;	// base
	yyxTest.link0(2) = 1;	// base
	yyxTest.link1(0) = 1;  // coxa
	yyxTest.link1(1) = -1;  // coxa
	yyxTest.link1(2) = 1;  // coxa
	yyxTest.link2(0) = 1;  // femur
	yyxTest.link2(1) = -1;  // femur
	yyxTest.link2(2) = 1;  // femur
	yyxTest.link3(0) = 1;  // tibia
	yyxTest.link3(1) = -1;  // tibia
	yyxTest.link3(2) = -1;  // tibia

	yyxTest.expectedResult(0) = -.1;  //-MOGI_PI/4.0;
	yyxTest.expectedResult(1) = 0.3;
	yyxTest.expectedResult(2) = -.5;

	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 5 for YYX .......... ";
	yyxTest.link0(0) = 0.1;	// base
	yyxTest.link0(1) = 0.2;	// base
	yyxTest.link0(2) = 0.3;	// base
	yyxTest.link1(0) = 5;  // coxa
	yyxTest.link1(1) = -0.1;  // coxa
	yyxTest.link1(2) = -0.2;  // coxa
	yyxTest.link2(0) = 10.3;  // femur
	yyxTest.link2(1) = -0.1;  // femur
	yyxTest.link2(2) = -1;  // femur
	yyxTest.link3(0) = 0.2;  // tibia
	yyxTest.link3(1) = 0.4;  // tibia
	yyxTest.link3(2) = -6.3;  // tibia

	yyxTest.expectedResult(0) = -.1;  //-MOGI_PI/4.0;
	yyxTest.expectedResult(1) = .1;
	yyxTest.expectedResult(2) = -.11;

	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 5a for YYX ......... ";
	yyxTest.expectedResult(0) = .4321;
	yyxTest.expectedResult(1) = -.051432;
	yyxTest.expectedResult(2) = .1;
	testPasses = yyxTest.test() ? testPasses : false;

	std::cout << "Testing IK sol 5b for YYX ......... ";
	yyxTest.expectedResult(0) = -.321;
	yyxTest.expectedResult(1) = -.1432;
	yyxTest.expectedResult(2) = -.001;
	testPasses = yyxTest.test() ? testPasses : false;

	return testPasses;
}

