/*
 *  hexapodGL.cpp
 *  Implements an app-controlled hexapod in a simulated environment
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
#include "common.h"

using namespace Mogi::Math;

/******************************************
 *	Classes
 ******************************************/
// Simple class just to print current hexapod state.
class StateListener : public Mogi::StateChart::StateChangeObserver {
private:
	void notify( Mogi::StateChart::Diagram* diagram ) {
		std::cout << " - New state: " << diagram->getCurrentState()->name() << std::endl;
	}
};

/******************************************
 *	Shared memory
 ******************************************/
// For UI/model updating:
UIhandler mUIhandler(640,480);
Time btime;

// Hexapod and control:
Robot::Hexapod *hexapod;
StateListener listener;
MotionControl<Vector> forwardControl;
MotionControlAngular headingControl;

// For app control:
App::AppInterface *appHandler;

/******************************************
 *	Functions
 ******************************************/
void init(int argc, char *argv[]);
void initHexapod();
void initApp();

void clearSharedMemory();

// The display callback function to update simulation models:
void displayCB(void*);

// Specific model updates:
void updateHexapodControl();
void updateAppInterface();

/******************************************
 *	Main
 ******************************************/
int main(int argc, char *argv[])
{
	// Set the appropriate callback function for simulation model updating:
	mUIhandler.displayCB = &displayCB;

	// Initialize SDL/OpenGL, default models for environment, camera, etc.
	mUIhandler.initialize();
	mUIhandler.initTestMeshesAndMaterials();

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
	initApp();
	initHexapod();

	btime.initialize();

	std::cout << "Final node structure for paverstonemodel" << std::endl;
	std::cout << mUIhandler.mainScene->rootNode.structureToString() << std::endl;
}

void initApp() {
	appHandler = new App::AppInterface;
	appHandler->setPort(8888);
	appHandler->start();
}

void initHexapod() {
	forwardControl.enableVelocityControl();
	forwardControl.setMaxSpeed(30);
	forwardControl.setAcceleration(10);

	headingControl.enableVelocityControl();
	headingControl.setMaxSpeed(1);
	headingControl.setAcceleration(1);

	// Startup the hexapod:
	hexapod = new Robot::Hexapod( &mUIhandler.mainScene->rootNode );
	hexapod->getStateChart()->addObserver(&listener);

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

	int ret;
	if( (ret = appHandler->stop()) )
	{
		std::cerr << "WARNING: appHandler->stop() returned %d" << std::endl;
	}

}


void updateAppInterface() {
	hexapod->beginModifying();

	appHandler->setBodyOffset(hexapod->getBodyOffset());
	appHandler->setBody(hexapod->getBodyVector());
	Vector legAngles[6];
	for (int i = 0; i < hexapod->legs.size(); i++) {
		legAngles[i] = hexapod->legs[i]->getKinematics();
	}
	appHandler->setLegAngles( legAngles );
	appHandler->setBodyOrientation(hexapod->getBodyOrientation());
	appHandler->setHeadAngles(hexapod->headMechanism.getAngles());

	hexapod->endModifying();
}


void displayCB(void*)
{
	btime.update();

	updateHexapodControl();
	updateAppInterface();
}

void updateHexapodControl() {
	hexapod->beginModifying();       // call this before modifying the hexapod attributes
	{
		Vector velocity(3);
		velocity(0) = appHandler->xLeft() * 50.0;
		velocity(1) = appHandler->yLeft() * 50.0;
		forwardControl.setVelocity(velocity);

		headingControl.setVelocity(appHandler->xRight() * -1.0 );
		hexapod->getWalkingDynamics()->setBody(forwardControl.processLocationControl(btime.dTime()), headingControl.processLocationControl(btime.dTime()), btime.dTime());
	}
	hexapod->endModifying();
}
