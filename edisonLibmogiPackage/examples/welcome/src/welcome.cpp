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

	std::string jsonFilePath = std::string(Mogi::getResourceDirectory()) +
							   "/configurations/nico.json";
	hex.setConfigurationFromJSONFilePath( jsonFilePath );

	hex.powerUp();                             // Initialize the motors and stand up.

	double loopTime = 0.01;   	               // Delay 10ms
	Vector location(3);
	while(location(1) < 200){                  // Walk the hexapod 200mm forward.
		usleep(loopTime * 1e6);
		location(1) += 0.1;

		hex.beginModifying();                  // Now modify the hexapod object.
		{
			// Updates the desired body location.
			hex.getWalkingDynamics()->setBody(location, 0, loopTime);
		}
		hex.endModifying();
	}

	hex.powerDown();	                       // Safely powers off the motors.
	while (!hex.isPoweredOff());               // Wait until safely powered down.

	return(0);
}