/*
 *  head_test.cpp
 *
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <fstream>
#include <sstream>

#include <mogi/robot/hexapod.h>
#include <mogi.h>

using namespace Mogi;
using namespace Dynamixel;
using namespace Robot;
using namespace Math;

// we will cycle through these states of operating the head:
enum State {twist = 0, right_left, up_down, roll_yaw, roll_pitch, yaw_pitch, all, numStates };

int main(int argc, char* argv[])
{
    Hexapod hex( NULL);                // hexapod class to operate hexapod
  
  Handler dynamixelHandler;
  // This is the dynamixel handler that the hexapod will use.
  int baudrate = 1000000;
  if(dynamixelHandler.openInterface(Interface::FTDI, baudrate))
  {
    std::cerr << "  - Opened an FTDI device!\n";
  } else {
    if(dynamixelHandler.openInterface(Interface::COM, baudrate, "/dev/ttyACM0"))
    {
      std::cerr << "  - Opened an serial port!\n";
    } else {
      std::cerr << "  - ERROR! Unable to open serial port or an FTDI device, disabling functionality.\n";
    }
  }
  dynamixelHandler.setPacketType(Packet::VER_1_0);
  
  hex.setDynamixelHandler( &dynamixelHandler );

  
	std::string jsonFilePath = std::string(getResourceDirectory()) + "/configurations/dmitri.json";
	std::ifstream jsonConfigurationFile( jsonFilePath.c_str() );
	std::stringstream jsonConfiguration;
	jsonConfiguration << jsonConfigurationFile.rdbuf();
	std::cout << "Parsing...";
	hex.setConfigurationFromJSONString(jsonConfiguration.str());
	std::cout << "Done." << std::endl;
  
//  std::cout << "init kinematics" << std::endl;
//  hex.initializeKinematics();
//  std::cout << "done init'ing kinematics" << std::endl;

    bool terminate = false;     //to exit main loop

    State state = twist;        // stores current state
    
    Time time;
    double tint = 0;
  
  
    
    // Get the hexapod initialized:
    hex.powerUp();
    printf("Powered up!\n");
    
	printf("State %d\n", state);
    time.initialize();
  
  double timeComponent = 0;
  double period = 10;
  
	while(!terminate){
        // Perform timing information update:
		time.update();
        tint += time.dTime();
		if(tint >= period){   // repeat every 3 seconds
            printf("Controller frequency: %3.4f, Running time: %3.4f\n",time.frequencyAverage(),time.runningTime());
            tint -= period;
            
            if(state == twist)
                state = right_left;
            else if(state == right_left)
                state = up_down;
            else if(state == up_down)
                state = roll_yaw;
            else if(state == roll_yaw)
                state = roll_pitch;
            else if(state == roll_pitch)
                state = yaw_pitch;
            else if(state == yaw_pitch)
                state = all;
            printf("State %d\n", state);
        }
        
		usleep(5000);   // This loop runs really fast, I throw this in for 5ms delays
        // The reason it is slowed down is to eliminate computational error of the timer.
        
        // Terminate after 7 sates, 5 seconds each
        if(time.runningTime()>(period*numStates))
            terminate = true;
		
    timeComponent = time.runningTime() * 2*MOGI_PI / period;
		// Now modify the hexapod structure for control:
        hex.beginModifying();
        {
            switch (state) {
                case twist:
					hex.headMechanism.setAngles(0,
									  15*sin(timeComponent) * MOGI_PI/180.0,
									  0 );
                    break;
                    
                case right_left:
					hex.headMechanism.setAngles(0,
									  0,
									  15*sin(timeComponent)  * MOGI_PI/180.0);
                    break;
                    
                case up_down:
                    hex.headMechanism.setAngles(12.5*(1-cos(timeComponent))* MOGI_PI/180.0,
									  0,
									  0 );
                    break;
                    
                case roll_yaw:
					hex.headMechanism.setAngles(0,
                    				  15*sin(timeComponent) * MOGI_PI/180.0,
                    				  15*sin(timeComponent) * MOGI_PI/180.0 );
                    break;
                    
                case roll_pitch:
					hex.headMechanism.setAngles(12.5*(1-cos(timeComponent)) * MOGI_PI/180.0,
                    				  15*sin(timeComponent) * MOGI_PI/180.0,
                    				  0 );
                    break;
                    
                case yaw_pitch:
					hex.headMechanism.setAngles(12.5*(1-cos(timeComponent))* MOGI_PI/180.0,
									  0,
                    				  15*sin(timeComponent)* MOGI_PI/180.0 );
                    
                    break;
                    
                case all:
					hex.headMechanism.setAngles(12.5*(1-cos(timeComponent))* MOGI_PI/180.0,
                    				  15*sin(timeComponent)* MOGI_PI/180.0,
                    				  15*sin(timeComponent) * MOGI_PI/180.0);
                    break;
                    
                default:
                    break;
            }
        }
        hex.endModifying();
        
	}
    
    printf("Main Loop finished!  Shutting down hexapod...");
    hex.powerDown();
	printf("Done.\n");
    
	return(0);
}
