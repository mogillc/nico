/*
 *  simple_walk.cpp
 *  
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>

#include <mogi/robot/hexapod.h>
#include <mogi.h>

using namespace Mogi;
using namespace Math;
using namespace Robot;
using namespace Dynamixel;

class StateListener : public Mogi::StateChart::StateChangeObserver {
private:
	void notify( Mogi::StateChart::Diagram* diagram ) {
		std::cout << " - New state: " << diagram->getCurrentState()->name() << std::endl;
	}
};

void updateAppInterface(Hexapod* hex, App::AppInterface* mAppHandler) {
	mAppHandler->setBodyOffset(hex->getBodyOffset());
	mAppHandler->setBody(hex->getBodyVector());
	Vector legAngles[6];
	for (int i = 0; i < hex->legs.size(); i++) {
		legAngles[i] = hex->legs[i]->getKinematics();
	}
	mAppHandler->setLegAngles( legAngles );
	mAppHandler->setBodyOrientation(hex->getBodyOrientation());
}

int main(int argc, char* argv[])
{

	App::AppInterface mAppHandler;
	mAppHandler.setPort(8888);
	mAppHandler.start();

	Node rootNode;
	StateListener listener;
    Hexapod mHexapod( &rootNode );                // hexapod class to operate hexapod
	mHexapod.getStateChart()->addObserver(&listener);

	// This is the dynamixel handler that the hexapod will use.
	Handler* dynamixelHandler = new Handler;
	int baudrate = 1000000;
	if(dynamixelHandler->openInterface(Interface::FTDI, baudrate) == Interface::NOERROR)
	{
		std::cerr << "  - Opened an FTDI device!\n";
	} else {
		if(dynamixelHandler->openInterface(Interface::COM, baudrate, "/dev/ttyACM0") == Interface::NOERROR)
		{
			std::cerr << "  - Opened an serial port!\n";
		} else {
			std::cerr << "  - ERROR! Unable to open serial port nor an FTDI device, disabling functionality.\n";
		}
	}
	dynamixelHandler->setPacketType(Packet::VER_2_0);
	//mHexapod.setDynamixelHandler( dynamixelHandler ); // must happen here otherwise calibration will not be set

	std::cout << "Parsing..." << std::endl;
	mHexapod.setConfigurationFromJSONFilePath( std::string(getResourceDirectory()) + "/configurations/nico.json" );
	std::cout << " - Done." << std::endl;

    bool terminate = false;     //to exit main loop
    
    Time mTime;                  // This keeps track of time, see main loop below

	MotionControl<Vector> forwardControl;
	forwardControl.enableVelocityControl();
	Vector forwardVelocity(3);
	forwardVelocity(1) = 50;
	forwardControl.setVelocity(forwardVelocity);
	forwardControl.setMaxSpeed(30);
	forwardControl.setAcceleration(10);

    // start hexapod:
    mHexapod.powerUp();              // This initializes the motors and gets it to stand up.  starts KINEMATICS thread

	while (!mHexapod.isWalkable()) {
		updateAppInterface(&mHexapod, &mAppHandler);
	}
	
    mTime.initialize();
    while(!terminate){
        // Perform timing information update:
		mTime.update();  // updated the timer, updates running time and dtime since last call
		usleep(5000);   // This loop runs really fast, I throw this in for 5ms delays
                        // The reason it is slowed down is to eliminate computational error of the timer.		
		
        // Terminate after 20 seconds
        if(mTime.runningTime()>20)
            terminate = true;
        
		// Now modify the hexapod structure for control:
        mHexapod.beginModifying();
        {
			mHexapod.getWalkingDynamics()->setBody(forwardControl.processLocationControl(mTime.dTime()),
												   0,
												   mTime.dTime());
			updateAppInterface(&mHexapod, &mAppHandler);
        }
        mHexapod.endModifying();
	}
    
    mHexapod.powerDown();

	while (!mHexapod.isPoweredOff()) {
		updateAppInterface(&mHexapod, &mAppHandler);
	}

	mHexapod.getStateChart()->removeObserver(&listener);
	return(0);
}
