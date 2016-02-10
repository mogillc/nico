/*
 *  armGL.cpp
 *  Implements a meandering hexapod and robotic arms in a kinematics simulator.
 *
 *  Updated by Matt Bunting on 12/29/15.
 *  Copyright 2015 Mogi LLC. All rights reserved.
 *
 */

#include <iostream>
#include <sstream>

#include <mogi.h>
#include <mogi/robot/hexapod.h>

#include "calibration.h"
#include "Arm6DOF.h"
#include "common.h"

using namespace Mogi::Math;

/******************************************
 *	Shared memory
 ******************************************/
// For UI/model updating:
UIhandler mUIhandler;
Time btime;

// Hexapod and control:
Robot::Hexapod *hexapod;
MotionControl<Vector> locationControl;
MotionControlAngular headingControl;

// For arm simulation:
std::vector<Arm6DOF*> arms6DOF;
double armScale;

/******************************************
 *	Functions
 ******************************************/
void init(int argc, char *argv[]);
void initArms();
void initHexapod();

void clearSharedMemory();

// The display callback function to update simulation models:
void displayCB(void*);

// Specific model updates:
void updateArmModels();
void updateHexapodControl();

// For walking the hexapod to randomly chosen locations and delay times:
Vector getNewLocation( const Vector& currentLocation );
double getNewHeading( const double& currentHeading );
double getNewTimeDelay( );

/******************************************
 *	Main
 ******************************************/
int main(int argc, char *argv[])
{
	// Set the appropriate callback function for simulation model updating:
	mUIhandler.displayCB = &displayCB;

	// Initialize SDL/OpenGL, default models for environment, camera, etc.
	mUIhandler.initialize();

	// THEN initialize models for this simulation session:
	init(argc, argv);

	// Print keyboard usage:
	UIhandler::printUsage();

	// Run until SDL is finished.
	mUIhandler.mainLoop();

	// Clean up.
	clearSharedMemory();

	return 0;
}

void init(int argc, char *argv[])
{
	if (argc != 1) {
		std::cerr << " Bad command! please try harder" << std::endl;
		exit(-1);
	}

	initArms();
	initHexapod();

	btime.initialize();

	//	std::cout << "Final node structure for mainScene:" << std::endl;
	//	std::cout << mUIhandler.mainScene->rootNode.structureToString() << std::endl;
}

void initArms() {
	armScale = MM_PER_METER;// / 3.0;// This is temoprary and should only be 1/MM_PER_METER

	Vector tempbase(3);

	for (int i = 0; i < 3; i++) {
		Arm6DOF *newArm = new Arm6DOF;
		newArm->build(&mUIhandler.mainScene->rootNode);
		newArm->setScene(mUIhandler.mainScene);
		tempbase(0)+=.01;
		newArm->setBase(tempbase, Quaternion());

		std::stringstream armName;
		armName.clear();
		armName << "launchpadArm_" << i;
		newArm->name = armName.str();

		arms6DOF.push_back(newArm);
	}

	// Creates a controlled platform, see updateArmModels() for explaination:
	Node* node = mUIhandler.mainScene->loadObject("launchpad.STL", Mogi::Simulation::Scene::getResourceDirectory().append("/models/arm").c_str());
	if(node)
	{
		node->name = "launchpad";
		node->setScale(2.0/MM_PER_METER);
	} else {
		std::cout << "Could not load launchpad.STL :(" << std::endl;
	}
}

void initHexapod() {
	locationControl.enableLocationControl();
	locationControl.setAcceleration(10);
	locationControl.setMaxSpeed(100);

	headingControl.enableLocationControl();
	headingControl.setAcceleration(3*MOGI_PI/180.0);
	headingControl.setMaxSpeed(15*MOGI_PI/180.0);

	hexapod = new Robot::Hexapod( &mUIhandler.mainScene->rootNode );
	std::cout << "Parsing...";
	hexapod->setConfigurationFromJSONFilePath(Mogi::getResourceDirectory() + std::string("/configurations/nico.json"));
	std::cout << "Done." << std::endl;

	// Once parameters are loaded, load the models for the scene:
	std::string directory = Simulation::Scene::getResourceDirectory();
	directory.append("/models/nico");
	hexapodToScene(mUIhandler.mainScene, hexapod, directory);

	hexapod->powerUp();
}

void clearSharedMemory() {
	std::cout << "Main Loop finished!  Shutting down hexapod." << std::endl;
	hexapod->powerDown();
	while (!hexapod->isPoweredOff());	// Wait until fully powered down.

}

void displayCB(void*)
{
	btime.update();

	updateArmModels();
	updateHexapodControl();
}

void updateArmModels() {
	// Here we have three robot arms connected to a rigid body called a "launchpad"
	// Each arm needs to connect operate the launchpad in a 6DOF workpsace.
	// The base of all arms are circulalry symmetric, likewise the end effector of each is attached to the launchpad in a circular pattern.
	// The end effector of each arm is determined by the desired pose and orientation of the launchpad.

	// Changing launchpad location over time:
	Vector launchpadLocation(3);
	launchpadLocation(0) = -200 + 50*sin(btime.runningTime()*.6);
	launchpadLocation(1) = 200 + 50*cos(btime.runningTime()*.2);
	launchpadLocation(2) = 350 + 100 * sin(btime.runningTime()*.55);

	// Changing launchpad orientation over time:
	Quaternion launchpadOrientation;
	Vector tempAxis(3);
	tempAxis(0) = sin(btime.runningTime()*.1);
	tempAxis(1) = cos(btime.runningTime()*.5);
	tempAxis(2) = cos(btime.runningTime()*.7);
	tempAxis.normalize();
	launchpadOrientation.makeFromAngleAndAxis(sin(btime.runningTime()*1.167) * MOGI_PI/6, tempAxis);

	// Update the model of the launchpad in the scene:
	Node *node = mUIhandler.mainScene->findNodeByName("launchpad");
	node->setOrientation(launchpadOrientation);
	node->setLocation(2 * launchpadLocation / MM_PER_METER);

	// For each leg, find the end effector location and compute IK:
	Quaternion initialOrientation;
	initialOrientation.makeFromAngleAndAxis( -M_PI/2, Vector::yAxis);

	Quaternion baseQuat;
	Vector tempLocation(7);
	Vector tempBase(3);
	Quaternion tempBaseOri;
	for (int i = 0; i < arms6DOF.size(); i++) {
		// Set the base location and orientation of each arm:
		tempBase(0) = 225*cos(2*MOGI_PI*(double)i/(double)arms6DOF.size()) + -200;
		tempBase(1) = 225*sin(2*MOGI_PI*(double)i/(double)arms6DOF.size()) + 200;
		tempBase(2) = 300;

		tempBaseOri.makeFromAngleAndAxis(-MOGI_PI/2.0*2, Vector::yAxis);
		baseQuat.makeFromAngleAndAxis(2*MOGI_PI*(double)i/(double)arms6DOF.size(), Vector::zAxis);
		tempBaseOri = baseQuat * tempBaseOri;

		arms6DOF[i]->setBase(tempBase, tempBaseOri);

		// Compute the end effector location based on the launchpad:
		Matrix EElocation;
		EElocation.makeI(4);
		EElocation(0,3) = 95*cos(2*MOGI_PI*(double)i/(double)arms6DOF.size());
		EElocation(1,3) = 95*sin(2*MOGI_PI*(double)i/(double)arms6DOF.size());
		EElocation(2,3) = 0;
		EElocation = launchpadOrientation.makeRotationMatrix4() * EElocation;

		// Compute the end effector orientation based on the launchpad:
		Quaternion EEorientation;
		EEorientation.makeFromAngleAndAxis(2*MOGI_PI*(double)i/(double)arms6DOF.size() + MOGI_PI, Vector::zAxis);
		EEorientation = launchpadOrientation * EEorientation * initialOrientation;

		// Compute the inverse kinematics:
		tempLocation(0) = EElocation(0,3) + launchpadLocation(0);
		tempLocation(1) = EElocation(1,3) + launchpadLocation(1);
		tempLocation(2) = EElocation(2,3) + launchpadLocation(2);
		tempLocation(3) = EEorientation(0);
		tempLocation(4) = EEorientation(1);
		tempLocation(5) = EEorientation(2);
		tempLocation(6) = EEorientation(3);

		arms6DOF[i]->computeAngles(tempLocation);
	}
}

void updateHexapodControl() {
	static int meanderState;
	static double timeTracker = 0;
	static double timeToWait = 0;

	if (meanderState == 0) {
		if (headingControl.getState() == MotionControlAngular::STOPPED &&
			locationControl.getState() == MotionControl<Vector>::STOPPED) {
			std::cout << "Setting a new position!" << std::endl;
			//				hexapod->setGoalHeading(getNewHeading(0));
			headingControl.setGoalPosition(getNewHeading(0));
			Vector currentLocation(3);
			//				hexapod->setGoalPosition(getNewLocation(currentLocation));
			locationControl.setGoalPosition(getNewLocation(currentLocation));
			meanderState = 1;
		}
	} else if (meanderState == 1) {
		if (headingControl.getState() == MotionControlAngular::STOPPED &&
			locationControl.getState() == MotionControl<Vector>::STOPPED) {
			timeTracker = 0;
			timeToWait = getNewTimeDelay();
			std::cout << "Going to wait " << timeToWait << " seconds" << std::endl;
			meanderState = 2;
		}
	} else if (meanderState == 2) {
		timeTracker += btime.dTime();
		if (timeTracker >= timeToWait) {
			meanderState = 0;
		}
	}

	hexapod->getWalkingDynamics()->setBody(locationControl.processLocationControl(btime.dTime()),
										   headingControl.processLocationControl(btime.dTime()),
										   btime.dTime());
}


Vector getNewLocation( const Vector& currentLocation ) {
	static Vector result(3);
	static Random randomNumber;

	result(2) = 0;
	do {
		result = currentLocation;
		result(0) += randomNumber.normal(0, 8000);
		result(1) += randomNumber.normal(0, 8000);
		std::cerr << "moving to location ( " << result(0) << ", " << result(1) << ")" << std::endl;
	} while (result.magnitude() > 1000);

	return result;
}

double getNewHeading( const double& currentHeading ) {
	static double result;
	static Random randomNumber;

	result = currentHeading;
	result += randomNumber.normal(0, MOGI_PI/8);
	result = fmod(result + MOGI_PI, 2*MOGI_PI) - MOGI_PI;

	return result;
}

double getNewTimeDelay( ) {
	static Random randomNumber;
	
	return randomNumber.uniform(3, 7);
}

