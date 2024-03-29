/*
 *  dynamixel_examples.cpp
 *  
 *
 *  Created by Mogi LLC on 12/27/15.
 *  Copyright 2015 Mogi LLC All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <iostream>

#include <mogi/dynamixel/dynamixel.h> 	// Dynamixel
#include <mogi/math/systems.h>		// Time
#include <mogi/statechart/statechart.h>	// State

#define PI (3.14159265358979)

using namespace Mogi;
using namespace Dynamixel;
using namespace Math;
using namespace StateChart;

/*
 * A simple function demonstrate the use of dynamixel.h API
*/
void simpleSinMovement(Handler &dynamixelHandler,std::map<unsigned char,Motor*> &motorMap){
	double omega = 2 * 3.1415926535 * 1;
	int deltaT_us = 10000;
	//double timeT = 0; 
	Time timer;
	
	// init timer
	timer.initialize();
	while(1){
		// update timer	
		timer.update();
		std::cout << "time:" << timer.runningTime() << std::endl;
		for (std::map<unsigned char,Motor*>::iterator it = motorMap.begin();
			it != motorMap.end(); it++) {
			// set goal position. The value passed to the setGoalPosition method should
			// be in the unit of radians
			// The goal position is generated by this formula:
			// position (in degree) = 90 * sin(omega * T) where omega is the 
			// angular speed of the sin wave (not to be confused with the motor angular
			// speed, and T is the time
			it->second->setGoalPosition(90*sin(omega*timer.runningTime())*3.1415926535/180);
			std::cout << "angel:" <<  90*sin(omega*timer.runningTime()) << std::endl;
			dynamixelHandler.pushInstruction(new Instruction(
				Instruction::WRITE, DataRange(REG_Goal_Pos,2),it->second));
		}	
		//dynamixelHandler.pushInstruction(new Instruction(
		//	Instruction::SYNC_WRITE, DataRange(REG_Goal_Pos,2),NULL,&motorMap));
  		while (dynamixelHandler.busy())
    		;
		usleep(deltaT_us);
	}
}


/*
 * \brief Base class for motor movement profile
 *
 * A concrete movement class should be derived from this class
*/
class MotorMovementProfile
{
	protected:
		double timeT;
		double currentPosition;
		double freq;
		double range;
		Handler *pDynamixelHandler;
		std::map<unsigned char,Motor*> *pMotorMap;
	public:
		MotorMovementProfile(Handler *handler = NULL,std::map<unsigned char,Motor*> *map = NULL, const double freq = 1, const double range = 90 )
		: freq(freq), range(range), pDynamixelHandler(handler),pMotorMap(map){};

		virtual ~MotorMovementProfile() {}

		virtual void applyMovement(double timeT) = 0;	

		void setFreq(double freq) {this->freq = freq;}
		void setRange(double range) {this->range = range;}
		void setHandler(Handler *handler) { this->pDynamixelHandler = handler; }
		void setMotorMap(std::map<unsigned char,Motor*> *map) {this->pMotorMap = map;}
};

/*
 * A motor movement class derived from MotorMovementProfile providing a Sin wave movement profile
*/
class SimpleSinMovementProfile : public MotorMovementProfile
{
	public:
		void applyMovement(double timeT){
			if(this->pDynamixelHandler == NULL || this->pMotorMap == NULL)
				return;
			for (std::map<unsigned char,Motor*>::iterator it = pMotorMap->begin();
				it != pMotorMap->end(); it++) {
				// set goal position. The value passed to the setGoalPosition method should
				// be in the unit of radians
				// The goal position is generated by this formula:
				// position (in degree) = 90 * sin(omega * T) where omega is the 
				// angular speed of the sin wave (not to be confused with the motor angular
				// speed, and T is the time
				it->second->setGoalPosition(this->range*sin(2*PI*this->freq*timeT)*PI/180);
				pDynamixelHandler->pushInstruction(new Instruction(
					Instruction::WRITE, DataRange(REG_Goal_Pos,2),it->second));
			}	
  			while (pDynamixelHandler->busy())
    			;
		}
};

/*
 * A motor movement class derived from MotorMovementProfile providing a linear movement profile
*/
class SimpleLinearMovementProfile : public MotorMovementProfile
{
	private:
		double pos;
	public:
		void applyMovement(double timeT){
			if(this->pDynamixelHandler == NULL || this->pMotorMap == NULL)
				return;
			for (std::map<unsigned char,Motor*>::iterator it = pMotorMap->begin();
				it != pMotorMap->end(); it++) {
				pos = this->freq*timeT;
				pos = fmod(pos,1.0);
				if(pos<0.5){
					pos = 4*pos - 1;		
				}else{
					pos = 3 - 4*pos;
				}
				it->second->setGoalPosition(this->range*pos*PI/180);
				pDynamixelHandler->pushInstruction(new Instruction(
					Instruction::WRITE, DataRange(REG_Goal_Pos,2),it->second));
			}	
  			while (pDynamixelHandler->busy())
    			;
		}
};


class MotorControlState : public State 
{
	private:
		MotorMovementProfile *profile;	
		Time *timer;
	public:
		std::string lable;
		MotorControlState(MotorMovementProfile *p = NULL,Time *t = NULL)
		: profile(p),timer(t) {};
		void setMotorMovementProfile(MotorMovementProfile *movementProfile){
			this->profile = movementProfile;
		}
		void setTimer(Time *timer){
			this->timer = timer;
		}
		void actionEntry(){
			std::cout << "Entering State:" << lable << std::endl;
		}
		// Overwritten actionDo to advance the motor position with its movement profile
		// Motor position gets updated with every call to the containing diagram class's update()
		// method, which calls this function.
		void actionDo(){
			if(this->profile != NULL && this->timer != NULL){
				this->profile->applyMovement(timer->runningTime());
			}	
		}
}; 

/*
 * The state machine class containing a state chart diagram with two motor movement
 * profile states
*/
class MotorMovementExampleStateMachine
{
	private:
		Time timer;
		SimpleSinMovementProfile sinProfile;	
		SimpleLinearMovementProfile linearProfile;

		Event eventBegin,event1,event2,eventEnd;

		Diagram stateDiagram;
		MotorControlState* sinMovementState;
		MotorControlState* linearMovementState;

		bool simpleFlag;
	public:
		MotorMovementExampleStateMachine(Handler *handler, std::map<unsigned char,Motor*> *map ){
			// Set up movement profile and state
			sinProfile.setHandler(handler);
			sinProfile.setMotorMap(map);
			sinMovementState = (MotorControlState*) stateDiagram.addState(new MotorControlState(&sinProfile,&timer));
			sinMovementState->lable = "Sin Movement";

			linearProfile.setHandler(handler);
			linearProfile.setMotorMap(map);
			linearMovementState = (MotorControlState*) stateDiagram.addState(new MotorControlState(&linearProfile,&timer));
			linearMovementState->lable = "Linear Movement";

			// Add state transitions
			Transition *tran = stateDiagram.getInitialState()->addTransition(sinMovementState);
			tran->setEvent(&eventBegin);

			tran = sinMovementState->addTransition(linearMovementState);
			tran->setEvent(&event1);
			tran = sinMovementState->addTransition(stateDiagram.getFinalState());
			tran->setEvent(&eventEnd);

			tran = linearMovementState->addTransition(sinMovementState);
			tran->setEvent(&event2);
			tran = linearMovementState->addTransition(stateDiagram.getFinalState());
			tran->setEvent(&eventEnd);

			simpleFlag = true;
		}
		void start(){
			timer.initialize();
			stateDiagram.update();
			eventBegin.trigger();
		}
		void stop(){
			eventEnd.trigger();
		}
		// This function should be called periodically
		void update(){
			timer.update();
			stateDiagram.update();
		}

		void toggleState(){
			if(simpleFlag){
				event1.trigger();
				simpleFlag = false;
			}
			else{
				event2.trigger();
				simpleFlag = true;
			}	
		}

};

int main(int argc, char* argv[])
{
	int baudrate = 1000000;
	Handler  dynamixelHandler;
	std::map<unsigned char,Motor*> motorMap;

	// setup interface
	if(dynamixelHandler.openInterface(Interface::FTDI, baudrate) == Interface::NOERROR)
	{
		std::cerr << "  - Opened an FTDI device!\n";
	} else {
		if(dynamixelHandler.openInterface(Interface::COM, baudrate, "/dev/ttyACM0") == Interface::NOERROR)
		{
			std::cerr << "  - Opened an serial port!\n";
		} else {
			std::cerr << "  - ERROR! Unable to open serial port nor an FTDI device, disabling functionality.\n";
		}
	}
	// set protocol version
	dynamixelHandler.setPacketType(Packet::VER_2_0);

	// add motor ID
	// TODO:make it availble in commandline args?
 	dynamixelHandler.forceAddDynamixel(1);

	// get motor map
	motorMap = dynamixelHandler.getDynamixels();	

	// read out motor type information
	for (std::map<unsigned char, Motor*>::iterator it = motorMap.begin();
  	     it != motorMap.end(); it++) {
    		dynamixelHandler.pushInstruction(
        	new Instruction(Instruction::READ, DataRange(0, 2), it->second));
  	}
  	while (dynamixelHandler.busy())
    	;

	// Optional: read out current register values
  	for (std::map<unsigned char, Motor*>::iterator it = motorMap.begin();
       		it != motorMap.end(); it++) {
    		dynamixelHandler.pushInstruction(new Instruction(
        	Instruction::READ, DataRange(2, it->second->getNumberOfRegisters() - 2),
        		it->second));
  	}
  	while (dynamixelHandler.busy())
    	;

	// set init angle to 0, init speed to max 
	for (std::map<unsigned char,Motor*>::iterator it = motorMap.begin();
		it != motorMap.end(); it++) {
		it->second->setGoalPosition(0);
		it->second->setMovingSpeed(0);
	}	
	dynamixelHandler.pushInstruction(new Instruction(
		Instruction::SYNC_WRITE, DataRange(REG_Goal_Pos,4),NULL,&motorMap));

	// perform a simple sin wave movement profile
	//simpleSinMovement(dynamixelHandler,motorMap);

	MotorMovementExampleStateMachine stateMachine(&dynamixelHandler,&motorMap);
	stateMachine.start();

	double time = 0, lastTime = 0;
	while(1){
		stateMachine.update();
		usleep(10000);
		time += 10000;
		if( time - lastTime >= 5 * 1000000 ){
			lastTime = time;
			stateMachine.toggleState();
		}
	}
  	return 0;
}


