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

#include <fstream>
#include <iostream>
#include <sstream>

#include <math.h>    // sin,cos
#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

#include "hexapod.h"
#include "mogi.h"

using namespace Mogi;
using namespace Math;
using namespace Robot;
using namespace Dynamixel;

bool matricesEqual(Matrix* A, Matrix* B);

class StateListener : public Mogi::StateChart::StateChangeObserver {
private:
	void notify( Mogi::StateChart::Diagram* diagram ) {
		output << " - New state: " << diagram->getCurrentState()->name() << std::endl;
	}
public:
	std::stringstream output;
};


class TestHexapod: public Robot::Hexapod {
public:
};

bool testConfiguration();
bool testConstructor();
bool testBalanceGestures();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning Contructor tests:" << std::endl;
	//allTestsPass = testConstructor() ? allTestsPass : false;
	std::cout << " - Beginning Statechart tests:" << std::endl;
	allTestsPass = testConfiguration() ? allTestsPass : false;
	std::cout << " - Beginning Balance Gesture tests:" << std::endl;
	allTestsPass = testBalanceGestures() ? allTestsPass : false;

//	std::cout << " - Beginning Decorator tests:" << std::endl;
	//testDecorator();

//	testConfiguration();

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testConfiguration() {
	bool allTestsPass = true;

	std::cout << "Testing nico powerUp and powerDown ....... ";
	Node rootNode;
	StateListener listener;
	Robot::Hexapod mHexapod( &rootNode );                // hexapod class to operate hexapod
	mHexapod.getStateChart()->addObserver(&listener);


	mHexapod.setConfigurationFromJSONFilePath( std::string(getResourceDirectory()) + "/configurations/nico.json" );

	// start hexapod:
	mHexapod.powerUp();              // This initializes the motors and gets it to stand up.  starts KINEMATICS thread

	while (!mHexapod.isWalkable()); // dangerous for a unit test...

	//usleep(100000);
	mHexapod.powerDown();

	while (!mHexapod.isPoweredOff()); // bleh

	std::stringstream expected("");
	expected << " - New state: poweringUp" << std::endl;
	expected << " - New state: powered:initial" << std::endl;
	expected << " - New state: powered:Starting" << std::endl;
	expected << " - New state: powered:Walking" << std::endl;
	expected << " - New state: adjustingStance" << std::endl;
	expected << " - New state: poweringDown" << std::endl;
	expected << " - New state: final" << std::endl;


	if (listener.output.str().compare(expected.str()) != 0) {
		std::cout << "FAILED" << std::endl;
		std::cout << "output:" << listener.output.str() << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Pass" << std::endl;
	}

	mHexapod.getStateChart()->removeObserver(&listener);

	return allTestsPass;
}

bool testDecorator() {
	bool allTestsPass = true;

	std::string configuration =
			"{\n\
	\"hexapod\" : { \n\
	\"name\": \"nico\",\n\
	\"startAngles\" : [\n\
	[0.0, -2.2116812281, 2.3561944902],\n\
	[0.0, -2.2116812281, 2.3561944902],\n\
	[0.0, -2.2116812281, 2.3561944902],\n\
	[0.0, -2.2116812281, 2.3561944902],\n\
	[0.0, -2.2116812281, 2.3561944902],\n\
	[0.0, -2.2116812281, 2.3561944902]\n\
	],\n\
	\"body\": {\n\
	\"legs\":[\n\
	{\n\
	\"location\": [42.0, 0.0, 18.0],\n\
	\"orientation\": [ 1.0, 0.0, 0.0, 0.0],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	},{\n\
	\"location\": [34.18, 53.2, 18.0],\n\
	\"orientation\": [ 0.924878, 0.0, 0.0, 0.380263],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	},{\n\
	\"location\": [-34.18, 53.2, 18.0],\n\
	\"orientation\": [ 0.380263, 0.0, 0.0, 0.924878],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	},{\n\
	\"location\": [-42.0, 0.0, 18.0],\n\
	\"orientation\": [ 0.0, 0.0, 0.0, 1.0],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	},{\n\
	\"location\": [-34.18, -53.2, 18.0],	\n\
	\"orientation\": [ -0.380263, 0.0, 0.0, 0.924878],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	},{\n\
	\"location\": [34.18, -53.2, 18.0],\n\
	\"orientation\": [ 0.924878, 0.0, 0.0, -0.380263],\n\
	\"links\": {\n\
	\"base\": [0.0, 0.0, 15.0],\n\
	\"coxa\": [29.0, 0.0, -24.0],\n\
	\"femur\": [60.0, 0.0, 0.0],\n\
	\"tibia\": [95.0, 0.0, -31.0] \n\
	}\n\
	}\n\
	]\n\
	}\n\
	}	\n\
	}";

	//	Node rootNode;
	//	HexapodBasic hexapod(&rootNode);
	////	hexapod.build(&rootNode);
	//	hexapod.setConfiguration(configuration);
	//	rootNode.printStructure();
	//	hexapod.update( 0 );
	//	std::cout << std::endl;
	//
	//	Handler dynamixelHandler;
	//	HexapodMotorDecorator *hexapodMotorController = new
	//HexapodMotorDecorator(&hexapod, &dynamixelHandler);
	//	hexapodMotorController->update( 0 );
	//
	//	std::cout << std::endl;
	//
	//
	//	HexapodWalkingDynamicsDecorator *walkingDynamics = new
	//HexapodWalkingDynamicsDecorator(hexapodMotorController);
	//	walkingDynamics->update( 0 );
	//	std::cout << std::endl;
	//	hexapodMotorController->update( 0 );
	//	std::cout << std::endl;
	//	hexapod.update( 0 );

	return allTestsPass;
}

bool testConstructor() {
	bool allTestsPass = true;
	Hexapod* hexapod = new Hexapod(NULL);

	delete hexapod;

	return allTestsPass;
}

bool testBalanceGestures() {
	bool allTestsPass = true;

	//	Vector *footLocations, *coxaLocations;
	//	footLocations = new Vector[6];
	//	coxaLocations = new Vector[6];
	//
	//	for(int i = 0; i < 6; i++) {
	//		footLocations[i].set_size(3);
	//		coxaLocations[i].set_size(3);
	//	}
	//
	//	std::cout << "Testing zero vectors ................... ";
	//	Vector result = balanceGestures(footLocations, coxaLocations);
	//	Vector correctResult(6);
	//	correctResult.name("correctResult");
	//	result.name("result");
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing zero coxa, foot translation1 ... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = 45;
	//		footLocations[i](1) = -100;
	//		footLocations[i](2) = MOGI_PI;
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = 0;
	//	correctResult(5) = 0;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing zero coxa, foot translation2 ... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = 45 + MOGI_PI*cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = -100 + sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = MOGI_PI;
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = 0;
	//	correctResult(5) = 0;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot translation2 ....... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		coxaLocations[i](0) = .1 * cos(MOGI_PI/3 * (float)i);
	//		coxaLocations[i](1) = .87 * sin(MOGI_PI/3 * (float)i);
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = 0;
	//	correctResult(5) = 0;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot rotation1 .......... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = MOGI_PI*cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = 0;
	//		footLocations[i] = Quaternion::createFromAngleAndAxis(MOGI_PI/2,
	//zAxis).makeRotationMatrix() * footLocations[i];
	//		footLocations[i](0) += 45;
	//		footLocations[i](1) += -100;
	//		footLocations[i](2) += MOGI_PI;
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = 0;
	//	correctResult(5) = MOGI_PI/2;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot rotation Z ......... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = MOGI_PI*cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = 0;
	//		footLocations[i] = Quaternion::createFromAngleAndAxis(3*MOGI_PI/2,
	//zAxis).makeRotationMatrix() * footLocations[i];
	//		footLocations[i](0) += 45;
	//		footLocations[i](1) += -100;
	//		footLocations[i](2) += MOGI_PI;
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = 0;
	//	correctResult(5) = -MOGI_PI/2;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot rotation X ......... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = MOGI_PI*cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = 0;
	//		footLocations[i] = Quaternion::createFromAngleAndAxis(3*MOGI_PI/2,
	//xAxis).makeRotationMatrix() * footLocations[i];
	//	}
	//	correctResult(0) = 0;
	//	correctResult(1) = 0;
	//	correctResult(2) = 0;
	//	correctResult(3) = -MOGI_PI/2;
	//	correctResult(4) = 0;
	//	correctResult(5) = 0;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot rotation Y ......... ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = 67*sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = 0;
	//		footLocations[i] = Quaternion::createFromAngleAndAxis(MOGI_PI/3,
	//yAxis).makeRotationMatrix() * footLocations[i];
	//		footLocations[i](0) += 45;
	//		footLocations[i](1) += -100;
	//		footLocations[i](2) += MOGI_PI;
	//	}
	//	correctResult(0) = 45;
	//	correctResult(1) = -100;
	//	correctResult(2) = MOGI_PI;
	//	correctResult(3) = 0;
	//	correctResult(4) = MOGI_PI/3;
	//	correctResult(5) = 0;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	//	std::cout << "Testing coxa1, foot rotation YZ ........ ";
	//	for(int i = 0; i < 6; i++)
	//	{
	//		footLocations[i](0) = 2.0 * cos(MOGI_PI/3 * (float)i);
	//		footLocations[i](1) = 9. * sin(MOGI_PI/3 * (float)i);
	//		footLocations[i](2) = 0;
	//		footLocations[i] =  Quaternion::createFromAngleAndAxis(MOGI_PI/2,
	//zAxis).makeRotationMatrix() *
	//							Quaternion::createFromAngleAndAxis(MOGI_PI/3,
	//yAxis).makeRotationMatrix() *
	//							footLocations[i];
	//	}
	//	correctResult(0) = 0;
	//	correctResult(1) = 0;
	//	correctResult(2) = 0;
	//	correctResult(3) = 0;
	//	correctResult(4) = MOGI_PI/3;
	//	correctResult(5) = MOGI_PI/2;
	//	result = balanceGestures(footLocations, coxaLocations);
	//	if (!matricesEqual(&result, &correctResult)) {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	} else {
	//		std::cout << "Passed" << std::endl;
	//	}
	//
	////	std::cout << "Testing coxa1, foot rotation XYZ ....... ";
	////	Quaternion correctQuaternion =
	///Quaternion::createFromAngleAndAxis(MOGI_PI/2, zAxis) *
	////					Quaternion::createFromAngleAndAxis(MOGI_PI/3, yAxis)
	///*
	////					Quaternion::createFromAngleAndAxis(2*MOGI_PI/3,
	///xAxis);
	////	for(int i = 0; i < 6; i++)
	////	{
	////		footLocations[i](0) = 2.0 * cos(MOGI_PI/3 * (float)i);
	////		footLocations[i](1) = 9. * sin(MOGI_PI/3 * (float)i);
	////		footLocations[i](2) = 0;
	////		footLocations[i] =  correctQuaternion.makeRotationMatrix() *
	///footLocations[i];
	////	}
	////	result = balanceGestures(footLocations, coxaLocations);
	////	Quaternion resultQuaternion =
	///Quaternion::createFromAngleAndAxis(result(5), zAxis) *
	////	Quaternion::createFromAngleAndAxis(result(4), yAxis) *
	////	Quaternion::createFromAngleAndAxis(result(3), xAxis);
	////	if (!matricesEqual(&resultQuaternion, &correctQuaternion)) {
	////		std::cout << "FAILED" << std::endl;
	////		allTestsPass = false;
	////	} else {
	////		std::cout << "Passed" << std::endl;
	////	}
	//

	return allTestsPass;
}

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
