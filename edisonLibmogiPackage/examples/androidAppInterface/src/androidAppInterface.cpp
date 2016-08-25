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
#include <iostream> // for std::cout (added by Adrian)
#include <cmath>

#include <mogi.h>
#include <mogi/app/appcomm.h>
#include <mogi/robot/hexapod.h>
#include <mogi/remoteapp/ApplicationCommunicator.h>
#include <mogi/remoteapp/ApplicationServer.h>
#include <mogi/network/ClientMulticastReceiver.h>
#include <mogi/remoteapp/HexapodControlComm.h>

using namespace Mogi;
using namespace App;
using namespace Math;
using namespace Robot;
using namespace Dynamixel;

class StateListener : public Mogi::StateChart::StateChangeObserver {
private:
	void notify( Mogi::StateChart::Diagram* diagram ) {
		std::cout << " - New state: " << diagram->getCurrentState()->name() << std::endl;
	}
};

void updateAppInterface(Hexapod* hex, RemoteApp::ApplicationCommunicator* mAppHandler) {
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
	// Start the app interface with a certain port:
	//AppInterface mAppInterface;
	//mAppInterface.setPort(10098);
	//mAppInterface.start();

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


	std::cout << "Ctrl-c to quit." << std::endl;
	
	double xLeft = 0, xLeftPrev = 0;
	double yLeft = 0, yLeftPrev = 0;
	double xRight = 0, xRightPrev = 0;
	double yRight = 0, yRightPrev = 0;
	Vector location(3);
	double orientation = 0;
	const double controlScaling = 0.2;

    // start hexapod:
    mHexapod.powerUp();              // This initializes the motors and gets it to stand up.  starts KINEMATICS thread

	RemoteApp::ApplicationServer appInterface;
	if (appInterface.start() != 0) {
		std::cout << "Problem starting app interface thread" << std::endl;
		return -1;
	}

	RemoteApp::ApplicationCommunicator appComm(appInterface);
	if (!appComm.start()) {
		std::cerr << "Problem starting appcomm thread" << std::endl;
		return -1;
	}

	while (!mHexapod.isWalkable()) {
		updateAppInterface(&mHexapod, &appComm);
	}

	Time mTime;                  // This keeps track of time, see main loop below

	while (1) {
		//usleep(1000000.0 * loopTimeInSeconds);
		//time += loopTimeInSeconds;
        // Perform timing information update:
		mTime.update();  // updated the timer, updates running time and dtime since last call
		usleep(5000);   // This loop runs really fast, I throw this in for 5ms delays
                        // The reason it is slowed down is to eliminate computational error of the timer.

		xLeft = appComm.xLeft();
		yLeft = appComm.yLeft();
		xRight = appComm.xRight();
		yRight = appComm.yRight();

		location(0) += xLeft * controlScaling;
		location(1) += yLeft * controlScaling;
		location(2) = 0;

		if (xRight != 0) {
			orientation = atan(yRight / xRight);
		}

		// Now modify the hexapod structure for control:
        mHexapod.beginModifying();
        {
			mHexapod.getWalkingDynamics()->setBody(location,
												   orientation,
												   mTime.dTime());
			updateAppInterface(&mHexapod, &appComm);
        }
        mHexapod.endModifying();



		if (xLeft != xLeftPrev || 
			yLeft != xLeftPrev ||
			xRight != xRightPrev ||
			yRight != yRightPrev) {

			std::cout << xLeft << ":" << yLeft <<
					 " " << xRight << ":" << yRight << std::endl;

		}

		xLeftPrev = xLeft;
		yLeftPrev = yLeft;
		xRightPrev = xRight;
		yRightPrev = yRight;
	}

	appComm.stop();
	appInterface.stop();

	return(0);
}
