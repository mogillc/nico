/*
 *  inverse_kinematics_sim.cpp
 *
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <mogi/robot/hexapod.h>
#include <mogi.h>

#include <iostream>
#include <cstring>

#include <pthread.h>

#include <unistd.h>
#include <math.h>

#include <fstream>
#include <sstream>

#include "danceState.h"

#include "../osc/OscReceivedElements.h"
#include "../osc/OscPacketListener.h"
#include "../ip/UdpSocket.h"



//todo:  define in arg or config file
#define PORT 7000

// todo group should be in config , env, or arg
#define GROUP "m1s99"

#define DEBUG true

using namespace Mogi;
using namespace Robot;
using namespace Dynamixel;

void oscRun(UdpListeningReceiveSocket *s) {
    s->Run();
}

void *oscVoidRun(void *arg) {
    UdpListeningReceiveSocket *s= (UdpListeningReceiveSocket *)arg;
    s->Run();
    pthread_exit(NULL);
    return NULL;
}


class OscBehaviorListener : public osc::OscPacketListener {

    
private:
    DanceState *danceState;

public:
    OscBehaviorListener( DanceState* danceStateToModify ) {
        danceState = danceStateToModify;
    }

protected:
    OscBehaviorListener() {} // not used
    
    void handleCommand(char command) {
        
        switch (command) {
            case '1':
                danceState->setLightState(OFF);
                break;
            case '2':
                danceState->setLightState(BLUE);
                break;
                
                
        }
     
        
        // todo: handle incoming behavior event here
        
        return;
        
    }
    
    virtual void ProcessMessage( const osc::ReceivedMessage& m,
                                const IpEndpointName& remoteEndpoint )
    {
        (void) remoteEndpoint; // suppress unused parameter warning
        
        try{
     

            // example of parsing single messages. osc::OsckPacketListener
            // handles the bundle traversal.
            
            
            char * address = (char *)m.AddressPattern();
            
            
            std::cout << "ADDRESS:" << address << std::endl;
            
            // is MESSAGE FOR MY GROUP ??
            if( strstr(m.AddressPattern(), GROUP ) != NULL ){
                
                
                // get command char
                char command = address[strlen(address)-1];
                
                // todo: handle incoming behavior event here
                
                // output is in DEBUG
                if (DEBUG) {
                std::cout << "received " << GROUP << " '" << command << "' command\n";
                }
                
                handleCommand(command);

                
            }
            
            // is MESSAGE FOR ANOTHER BOT ??
            else if( std::strcmp( m.AddressPattern(), "/" ) == 0 ){
                // NOT GLOBAL MESSAGE
                //
                // NO OPT
                //
            }
            
            // is MESSAGE GLOBAL ??
            else {
             
                // get command char
                char command = address[strlen(address)-1];
                
                // output is in DEBUG
                if (DEBUG) {
                    std::cout << "received global'" << command << "' command\n";
                }
                
                handleCommand(command);
                
            }
            
                
          
        
        
        }catch( osc::Exception& e ){
            // any parsing errors such as unexpected argument types, or
            // missing arguments get thrown as exceptions.
            std::cout << "error while parsing message: "
            << m.AddressPattern() << ": " << e.what() << "\n";
        }
    
        return;
    }
    
    

    
};


class TimeDebug {
private:
	Time timer;
	std::map<std::string, double> times;

public:
	TimeDebug() {
		timer.initialize();
	}
	void reset() {
		times.erase(times.begin(), times.end());
	}
	void print() {
		std::cout << "Times:" << std::endl;
		for (std::map<std::string,double>::iterator it = times.begin(); it != times.end(); it++) {
			std::cout << " - " << it->first << ", time: " << it->second << std::endl;
		}
	}

	void update(const std::string& label) {
		timer.update();
		times[label] = timer.dTime();
	}

};

class JsonStringObserver : public App::JsonValueObserver {
private:
	DanceState *dancestate;
	void update( App::JsonValueInterface& newValue ) {
		if (newValue.isString()) {
			std::string value = newValue.asString();
			std::cout << "Cool! Got the string: " << value << std::endl;
			if (value == "R") {
				dancestate->setLightState(RED);
			} else if (value == "A") {
				dancestate->setLightState(ALL);
			} else if (value == "B") {
				dancestate->setLightState(BLUE);
			} else if (value == "X") {
				dancestate->setLightState(XMAS);
			} else if (value == "G") {
				dancestate->setLightState(GREEN);
			} else if (value == "Off") {
				dancestate->setLightState(OFF);
			}
		}
	}
public:
	JsonStringObserver( DanceState* danceStateToModify ) {
		dancestate = danceStateToModify;
	}
};


void printUsage( char *argv[] )
{
	std::cout << "Robotics version: " << getLibraryVersion().version_str << std::endl;
	printf("Usage:\n");
	printf("   %s [-d]\n", argv[0]);
	printf("\n");
	//printf("  -s:    Simulation enable (true hexapod control)\n");
	printf("  -d:    daemon mode (will not use ncurses) \n");
	printf("\n");
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


int main(int argc, char* argv[])
{
	bool daemonMode = false;
	// Determine the input commands:
	if ( argc < 1 || argc > 2 ) {
		printUsage(argv);
		exit(EXIT_FAILURE);
	}

	if (argc == 2) {
		if (argv[1][0] != '-' || argv[1][1] != 'd' || argv[1][2] != '\0') {
			printUsage(argv);
			exit(EXIT_FAILURE);
		}
		daemonMode = true;
	}

	std::cout << "Robotics version: " << getLibraryVersion().version_str << std::endl;

	// This is the dynamixel handler that the hexapod will use.
	int baudrate = 1000000;
	Handler dynamixelHandler;
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
	dynamixelHandler.setPacketType(Packet::VER_2_0);


	Node rootNode;
	Hexapod hex( &rootNode );                // hexapod class to operate hexapod

	hex.setDynamixelHandler( &dynamixelHandler );	// must happen here otherwise calibration will not be set

	std::string jsonConfigurationPath = std::string(getResourceDirectory()) + "/configurations/nico.json";
	std::cout << "Loading configuration file: " << jsonConfigurationPath << std::endl;
	std::ifstream jsonConfigurationFile( jsonConfigurationPath.c_str() );
	std::string jsonConfiguration((std::istreambuf_iterator<char>(jsonConfigurationFile)),
								  std::istreambuf_iterator<char>());
	std::cout << " - Parsing ... ";
	hex.setConfigurationFromJSONString(jsonConfiguration);
	std::cout << "Done." << std::endl;



	//hex.initializeKinematics();
	////////////////////////////////////////////////////////////////
	// Done configuring legs
	////////////////////////////////////////////////////////////////


	// For walking around:
	MotionControlAngular headingControl;
	headingControl.enableLocationControl();
	headingControl.setAcceleration(3*MOGI_PI/180.0);
	headingControl.setMaxSpeed(15*MOGI_PI/180.0);

	MotionControl<Vector> locationControl;
	locationControl.enableLocationControl();
	locationControl.setAcceleration(10);
	locationControl.setMaxSpeed(100);

	Vector zeroVector(3);
	locationControl.setGoalPosition(zeroVector);
	headingControl.setGoalPosition(0);

	DanceState danceState;
	
	// Wifi stuff:
	App::AppInterface hexapodSocketServer;
	//hexapodSocketServer.initialize();
	hexapodSocketServer.setPort( 3456 );
	hexapodSocketServer.start(  );

	App::AppOption ledOptions;
	ledOptions.optionType = App::APP_SEGMENTED_CONTROL;
	ledOptions.segments.push_back("Off");
	ledOptions.segments.push_back("R");
	ledOptions.segments.push_back("G");
	ledOptions.segments.push_back("B");
	ledOptions.segments.push_back("X");
	ledOptions.segments.push_back("A");
	ledOptions.title = "LED";
	hexapodSocketServer.addOptionMenuOption(&ledOptions);

	JsonStringObserver LEDStringObserver( &danceState );
	hexapodSocketServer.addJsonValueListener("LED", &LEDStringObserver);
    
    // OSC SETUP
    OscBehaviorListener oscListener(&danceState);
    UdpListeningReceiveSocket s(
                                IpEndpointName( IpEndpointName::ANY_ADDRESS, PORT ),
                                &oscListener );
    std::cout << "press ctrl-c to end\n";
    
    //blocks
    //s.RunUntilSigInt();
   // std::thread oscThread(oscRun, &s);
    
  //  std::vector<UdpListeningReceiveSocket> listener(1);
    
    pthread_t oscPThread;
    pthread_create(&oscPThread, NULL, &oscVoidRun, (void *)&s);
    

	Time time, timeTest;                  // This keeps track of time, see main loop below

	double currentFrequency = 128. /(60. * 2. * 16.);    // The current estimated frequency from input


	double chritmasTimeTracker = 0;
	double christmasUpdateDelay = 0.25;
	int christmasPreviousIndex = 0;

	// Start hexapod:
	//hex.WD_enabled = false; // BUT FIRST!  disable all walking dynamics

	time.initialize();
	timeTest.initialize();
	double desiredLoopTime = 1.0/60.0;
	double sleepTime = 0;
	double loopTime = 0;
	double sleepTimeOffset = 0;

	Vector footSave = hex.foot[1];
	Vector bodyAngularVelocity(3);
	bool needToSaveFoot = true;

	LowPassFilter<Vector> userControlLPF, userControlLPF2;
	userControlLPF.setTimeConstant(0.5);
	userControlLPF2.setTimeConstant(0.5);
	LowPassFilter<double> standingHeightLPF;
	standingHeightLPF.setTimeConstant(0.5);

	bool terminate = false;
	double cursesUpdateTracker = 0;
	int loopCounter = 0;

	TimeDebug mTimeDebug;
	while(!terminate){
		loopCounter++;
		// This ensures a desireable thread update rate:
		timeTest.update();
		loopTime = timeTest.dTime();
		if (loopTime > desiredLoopTime * 1.05) {
			std::cerr << "WARING! Excessive loop time:" << loopTime << ", d:" << desiredLoopTime << ", c:" << loopCounter << std::endl;
			mTimeDebug.print();
		}
		mTimeDebug.reset();
		sleepTime = desiredLoopTime - loopTime + sleepTimeOffset;
		if (sleepTime > 0) {
			usleep( sleepTime * 1000000.0 );
		}
		timeTest.reset();
		sleepTimeOffset += 0.001 * (desiredLoopTime - 1.0/time.frequency());
		if ((fabs(sleepTimeOffset) >= 10000000000) || (sleepTimeOffset != sleepTimeOffset)) {
			sleepTimeOffset = 0;
		}


	
		danceState.checkLimits();
		cursesUpdateTracker += time.dTime();

		if(hexapodSocketServer.isOpen())
		{
			danceState.controlMode = SOCKET;
		} else {
			danceState.controlMode = KEYBOARD;
		}

		switch (danceState.controlMode) {
			case SOCKET:
				danceState.xLeft = hexapodSocketServer.xLeft();
				danceState.yLeft = hexapodSocketServer.yLeft();
				danceState.xRight = hexapodSocketServer.xRight();
				break;

			default:
				break;
		}



		// Perform timing information update:
//		static double tint = 0;
		time.update();  // updated the timer, updates running time and dtime since last call
//				tint += time.dTime();
//				if(tint >= 3){   // repeat every 3 seconds
//					printf("Controller frequency: %3.4f, Running time: %3.4f\n",time.frequencyAverage(),time.runningTime());
//					tint -= 3;
//				}

		{ //Xmas mode stuff:
			chritmasTimeTracker += time.dTime();

			if (chritmasTimeTracker > christmasUpdateDelay) {
				int christmasIndex = christmasPreviousIndex;
//				std::vector<Motor*> dynamixels = hex.dyn;
				for (std::map<unsigned char, Motor*>::iterator it = hex.dyn.begin(); it != hex.dyn.end(); it++) {
					if (christmasIndex >= danceState.lights.size()) {
						christmasIndex = 0;
					}

					it->second->setByte(REG_LED, danceState.lights[christmasIndex++]);
				}

				dynamixelHandler.pushInstruction(new Instruction(Instruction::SYNC_WRITE, DataRange(REG_LED, 1)));
				christmasPreviousIndex++;
				if (christmasPreviousIndex >= danceState.lights.size()) {
					christmasPreviousIndex = 0;
				}

				chritmasTimeTracker -= christmasUpdateDelay;
			}
		}


		Vector userControlAngles(3);


		danceState.updateStates(&hex);

		if ( danceState.state == SOFT_WAKE) {
			std::cout << "Waking up" << "\r\n";
			hex.powerUp();
			time.initialize();
			danceState.changeStates(walking, &hex);
			std::cout << "Done!" << "\r\n";
			continue;
		}

		if( danceState.state == WALK_BEFORE_SLEEP )
		{
			std::cout << "Sleeping..." << "\r\n";
			static double timeTrackerForSleep = 0;
			//static bool waitForWD = false;
			if(danceState.walkingMagnitude == 1) {
//				hex.WD_enabled = true;	// need to walk into pre-rest position
				hex.enableWalking();
				//				waitForWD = false;
				//				waitForWD = true;
				timeTrackerForSleep = 0;	// reset timer for the next time
				danceState.changeStates(SOFT_SLEEP, &hex);	// the state is now in soft sleep mode
				hex.powerDown();
			} else {

			}

			//timeTrackerForSleep += time.dTime();

			//			if ( !waitForWD ) {
			//				waitForWD = true;
			//				timeTrackerForSleep = 0;	// reset timer for the next time
			//				danceState.changeStates(SOFT_SLEEP, &hex);	// the state is now in soft sleep mode
			//				hex.powerDown();
			//			}
			danceState.setLightState(OFF);
		}

		if (danceState.state == SOFT_SLEEP) {

		}


		if (danceState.controlMode == SOCKET) {

			if((hexapodSocketServer.slider1() <= 0.5) && (hexapodSocketServer.slider2() <= 0.5))
				//state = walking;
				danceState.changeStates(walking, &hex);
			if((hexapodSocketServer.slider1() > 0.5) && (hexapodSocketServer.slider2() <= 0.5))
				//state = headbob;
				danceState.changeStates(headbob, &hex);
			if((hexapodSocketServer.slider1() <= 0.5) && (hexapodSocketServer.slider2() > 0.5))
				///state = quaternion;
				danceState.changeStates(quaternion, &hex);
			if((hexapodSocketServer.slider1() > 0.5) && (hexapodSocketServer.slider2() > 0.5))
				danceState.changeStates(fistpump, &hex);
			//state = fistpump;
		}


		/////////////////////////////////////////////////////////////
		// Walking control
		/////////////////////////////////////////////////////////////
		if(danceState.walkingMagnitude == 1)
		{
			//hex.leglifttime = 1.0/(currentFrequency * 8.0 * 3);
//			hex.WD_enabled = true;
			hex.enableWalking();
			needToSaveFoot = true;

			//static int meanderState = 0;

			/////////////////////////////////////////////////////////////
			// Meandering control
			/////////////////////////////////////////////////////////////
			if (danceState.state == MEANDER ) {

				static double timeTracker = 0;
				static double timeToWait = 0;



				//				hex.enableLocationControl();
				//				hex.setMaxVelocities(15*MOGI_PI/180.0, 100);
				//				hex.setAccelerations( 3*MOGI_PI/180.0, 10);

			//	danceState.currentLocation = hex.bodyLocationDesired;

				if (danceState.meanderState == SETTING_LOCATION) {
					if (headingControl.getState() == MotionControlAngular::STOPPED &&
						locationControl.getState() == MotionControl<Vector>::STOPPED) {	// this if is probably not needed
						std::cout << "Setting a new position!" << std::endl;
						double newHeading = getNewHeading(0);
						headingControl.setGoalPosition(newHeading);
						//hex.setGoalHeading(newHeading);
						Vector currentLocation(3);
						Vector newLocation = getNewLocation(currentLocation);
						danceState.desiredLocation(0) = newLocation(0);
						danceState.desiredLocation(1) = newLocation(1);
						danceState.desiredLocation(5) = newHeading;
						locationControl.setGoalPosition(newLocation);
						//hex.setGoalPosition(newLocation);
						danceState.meanderState = MOVING;
					}
				} else if (danceState.meanderState == MOVING) {
					if (headingControl.getState() == MotionControlAngular::STOPPED &&
						locationControl.getState() == MotionControl<Vector>::STOPPED) {
						timeTracker = 0;
						timeToWait = getNewTimeDelay();
						std::cout << "Going to wait " << timeToWait << " seconds" << std::endl;
						danceState.meanderState = WAITING;
					}
				} else if (danceState.meanderState == WAITING) {
					timeTracker += time.dTime();
					if (timeTracker >= timeToWait) {
						danceState.meanderState = SETTING_LOCATION;
					}
				}

			} else {
				/////////////////////////////////////////////////////////////
				// User walking control
				/////////////////////////////////////////////////////////////

				danceState.meanderState = SETTING_LOCATION;
				//				hex.enableVelocityControl();
				headingControl.enableVelocityControl();
				locationControl.enableVelocityControl();
				//				hex.bodyVelocity(0) = danceState.xLeft * 25 * 3;
				//				hex.bodyVelocity(1) = danceState.yLeft * 25 * 3;
				Vector bodyVelocity(3);
				bodyVelocity(0) = danceState.xLeft * 25 * 3;
				bodyVelocity(1) = danceState.yLeft * 25 * 3;
				locationControl.setVelocity(bodyVelocity);
				//				bodyAngularVelocity(2) = danceState.xRight * 15.0 * MOGI_PI / 180. * 2;
				headingControl.setVelocity(-danceState.xRight * 15.0 * MOGI_PI / 180. * 2);
			}

		} else if (danceState.walkingMagnitude == 0) {
			double totalHeight = 0;
			//hex.bodyVelocity *= 0;
			//bodyAngularVelocity *= 0;
			for(int i = 0; i < 6; i ++)
			{
				totalHeight += hex.foot[i](2);
			}
			if(totalHeight == 0 )
			{
//				hex.WD_enabled = false;
				hex.disableWalking();
				if(needToSaveFoot)
				{
					needToSaveFoot = false;
					footSave = hex.foot[1];
				}
			}

		}


		Vector headAngles(3);
		Vector bodyOffset(3);
		Vector bodyAngleAdjust(3);

		/////////////////////////////////////////////////
		// InverseKinematics (quaternion) control
		/////////////////////////////////////////////////
		if (danceState.quaternionMagnitude > 0) {

			headAngles(0) =  (0*(26.506 - 3.7601)/2.0 + .5 *(26.506 - 3.7601)*sin(27.0/40.0 * MOGI_PI*time.runningTime())) * MOGI_PI/180.0;
			headAngles(1) = .5*0.9 * 20.0 * sin(30.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;
			headAngles(2) = .5*.6 * 13.0 * sin(24.0/40.0*MOGI_PI*time.runningTime()) * MOGI_PI/180.0;

			// With the desired head angles, now apply motions to the body:
			Matrix xRot(3,3), yRot(3,3), zRot(3,3), totalRotation;
			xRot.makeXRotation( headAngles(0) );
			yRot.makeYRotation( headAngles(1) );
			zRot.makeZRotation( headAngles(2) );
			totalRotation = xRot * zRot * yRot;

			Quaternion headQuaternion;
			headQuaternion = totalRotation.rotationToQuaternion();

			bodyAngleAdjust = headQuaternion.eulerAngles();

			// Now that the body is rotated, need to shift the body so that the head is stationary
			Matrix camOrigin(3,1);

			camOrigin(0,0) = 0.0;
			camOrigin(1,0) = 40;
			//camOrigin(1,0) = 133.48;
			camOrigin(2,0) = 23.6;

			xRot.makeXRotation( -bodyAngleAdjust(0) );
			yRot.makeYRotation( -bodyAngleAdjust(1) );
			zRot.makeZRotation( -bodyAngleAdjust(2) );

			Matrix camRotate = xRot*yRot*zRot*camOrigin;

			Matrix camOffset = camOrigin - camRotate;

			bodyOffset(0) = camOffset(0,0);
			bodyOffset(1) = camOffset(1,0);
			bodyOffset(2) = camOffset(2,0);// + bodyheight;

			headAngles *= danceState.quaternionMagnitude;
			bodyOffset *= danceState.quaternionMagnitude;
			if(bodyAngleAdjust(0) > MOGI_PI)
				bodyAngleAdjust(0) -= 2*MOGI_PI;
			if(bodyAngleAdjust(0) < -MOGI_PI)
				bodyAngleAdjust(0) += 2*MOGI_PI;
			bodyAngleAdjust *= danceState.quaternionMagnitude;
			//printf("bodyAngleAdjust(0) = %3f\n", bodyAngleAdjust(0));
		}

		double timePass;

		double bpm = 128;
		double bFreq = bpm / (60 * 2);
		//double timeComponent = time.runningtime*MOGI_PI*bFreq;
		static double globalOffset = 0;
		double timeComponent = (time.runningTime())*MOGI_PI*currentFrequency - globalOffset;
		if(timeComponent > (2*MOGI_PI))
		{
			globalOffset += 2*MOGI_PI;
			timeComponent -= 2*MOGI_PI;
		}
		//		timeComponent -= currentPhase;
		timePass = timeComponent;

		/////////////////////////////////////////////////
		// headBob control
		/////////////////////////////////////////////////
		if (danceState.headBobMagnitude > 0) {
			timeComponent *= 16.0;

			double bodyZangle = (13.5 * 2./sqrt(3)*( sin(timeComponent/2.) + sin(3.*timeComponent/2.)/6. + sin(5.*timeComponent/2.)/60. ))* MOGI_PI/180.0;
			double bodyXangle = (4.5/bFreq*cos(2*timeComponent) + 3.5*(1 - 2*pow(sin(timeComponent), 8) + .2*cos(2*timeComponent) + .1*sin(timeComponent +.2)))* MOGI_PI/180.0;

			headAngles(2) += danceState.headBobMagnitude*bodyZangle;
			headAngles(0) += danceState.headBobMagnitude*bodyXangle;

			bodyOffset(0) += danceState.headBobMagnitude*.75*bodyZangle;

			bodyAngleAdjust(0) -= danceState.headBobMagnitude*(2.0/bFreq*cos(2*timeComponent)* MOGI_PI/180.0 + bodyXangle/6.);
			bodyAngleAdjust(2) -= danceState.headBobMagnitude*-.70*bodyZangle;
		}


		/////////////////////////////////////////////////
		// Fist Pump control
		/////////////////////////////////////////////////
		if (danceState.fistPumpMagnitude != 0) {


			if(hex.isIdle())
			{
				hex.beginModifying();
				{
					hex.foot[1](2) = 175;//200;

					Quaternion hexapodOrientation = hex.getBodyOrientation();
					Quaternion additionalRotation;
					additionalRotation.makeFromAngleAndAxis(MOGI_PI/4.0, Vector::zAxis);
					Vector footXYCenter = 57.5 *sqrt(2)/2 *((const Quaternion&)(additionalRotation*hexapodOrientation)).makeRotationMatrix() * Vector::xAxis;
					hex.foot[1](0) = footXYCenter(0) + 40*cos(timePass*32.0);
					hex.foot[1](1) = footXYCenter(1) + 40*sin(timePass*32.0);

					hex.foot[1] *= danceState.fistPumpMagnitude;
					hex.foot[1] += footSave;
				}
				hex.endModifying();
			}
		}

		mTimeDebug.update("behavior done");

		// Now modify the hexapod structure for control:
		hex.beginModifying();
		{
			mTimeDebug.update("hex.beginModifying");
//			if (hex.walking())
			{
				hex.getWalkingDynamics()->setBody(locationControl.processLocationControl(time.dTime()), headingControl.processLocationControl(time.dTime()), time.dTime());
				// Set the head angles:
				hex.headMechanism.setAngles( headAngles );

				// Set the body angles opposite to the head angles
				Quaternion xRotation, yRotation, zRotation;

				xRotation.makeFromAngleAndAxis(-1.0 * bodyAngleAdjust(0), Vector::xAxis);
				yRotation.makeFromAngleAndAxis(-1.0 * bodyAngleAdjust(1), Vector::yAxis);
				zRotation.makeFromAngleAndAxis(-1.0 * bodyAngleAdjust(2), Vector::zAxis);
				hex.setBodyOrientation(zRotation * yRotation * xRotation);

				// Set the body offset:
				hex.setBodyLocation( bodyOffset );
			}
			mTimeDebug.update("hex.walking()");
			hexapodSocketServer.setBody( hex.getBodyVector());
			hexapodSocketServer.setBodyOrientation(hex.getBodyOrientation());
			Vector legAngles[6];
			for (int i = 0; i < hex.legs.size(); i++) {
				legAngles[i] = hex.legs[i]->getKinematics();
			}
			hexapodSocketServer.setLegAngles( legAngles );
			hexapodSocketServer.setHeadAngles( hex.headMechanism.getAngles() );
		}
		hex.endModifying();

		mTimeDebug.update("End of loop");
		
	}
	
	printf("Main Loop finished!  Shutting down hexapod...");
	hexapodSocketServer.stop();
	hex.powerDown();
	printf("Done.\n");
	
	return(0);
}
