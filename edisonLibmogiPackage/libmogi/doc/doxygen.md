libmogi API Documentation                         {#mainpage}
============

## About

This software library was developed as a set of utilities for robotics, specifically for the Dmitri hexapod.

![Dmitri, the first hexapod iteration.](/doc/images/dmitri.tiff "Dmitri")

### Example:


~~~~~~~~~~~~~{.cpp}
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>

#include <mogi.h>
#include <mogi/robot/hexapod.h>

using namespace Mogi::Math;
using namespace Mogi::Robot;
using namespace Mogi::Dynamixel;

int main(int argc, char* argv[])
{
	Node rootNode;
	Hexapod hex( &rootNode );                  // Hexapod class to handle kinematics.

	Handler* dynamixelHandler = new Handler;   // The motor communicator.
	int baudrate = 1000000;
	dynamixelHandler->openInterface(Interface::FTDI, baudrate);
	dynamixelHandler->setPacketType(Packet::VER_2_0);
	hex.setDynamixelHandler( dynamixelHandler );

	std::string jsonFilePath = std::string(Mogi::getResourceDirectory()) + "/configurations/nico.json";
	hex.setConfigurationFromJSONFilePath( jsonFilePath );

	hex.powerUp();                             // Initialize the motors and stand up.

	Vector location(3);
	while(location(1) < 200){                  // Walk the hexapod 200mm forward.
		usleep(1000);	                       // Delay 10ms
		location(1) += 0.1;

		hex.beginModifying();                  // Now modify the hexapod object.
		{
			hex.setDesiredPosition(location);  // Updates the desired body location.
		}
		hex.endModifying();
	}

	hex.powerDown();	                       // Safely powers off the motors.
	while (!hex.isPoweredOff());               // Wait until safely powered down.

	return(0);
}
~~~~~~~~~~~~~