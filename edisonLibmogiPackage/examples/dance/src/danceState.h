//
//  danceState.h
//  Project
//
//  Created by Matt Bunting on 12/23/14.
//
//

#ifndef __Project__danceState__
#define __Project__danceState__

#include <stdio.h>
#include <mogi/robot/hexapod.h>

using namespace Mogi;
using namespace Robot;
using namespace Math;

enum State {
	walking,
	quaternion,
	headbob,
	fistpump,
	MEANDER,
	WALK_BEFORE_SLEEP,
	SOFT_SLEEP,
	SOFT_WAKE
};
enum LightState {
	OFF,
	BLUE,
	RED,
	GREEN,
	XMAS,
	ALL
};

enum ControlMode {
	KEYBOARD,
	SOCKET
};

enum MeanderState {
	SETTING_LOCATION,
	MOVING,
	WAITING
};

class DanceState {
public:
	bool verbose;
	double headBobMagnitude;
	double walkingMagnitude;
	double quaternionMagnitude;
	double fistPumpMagnitude;

	std::vector<unsigned char> lights;

	State state;
	ControlMode controlMode;
	MeanderState meanderState;

	double xLeft,yLeft,xRight;

	//Vector currentLocation;
	Vector desiredLocation;

	void updateStates( Hexapod* hexapod );
	void changeStates( State newState, Hexapod* hexapod );

	bool dealWithInput( char value, Hexapod* hexapod );

	void setLightState( LightState );
	LightState getLightState() const { return lightState; }

	void checkLimits();

	DanceState()
	: verbose(true), headBobMagnitude(0), walkingMagnitude(1), quaternionMagnitude(0), fistPumpMagnitude(0), state(SOFT_WAKE),   controlMode(SOCKET), meanderState(SETTING_LOCATION), xLeft(0),yLeft(0),xRight(0), lightState(OFF), readyToSwitchState(false)
	{
		lights.push_back(XL_LED_OFF);
		time.initialize();
		//currentLocation.setLength(6);
		desiredLocation.setLength(6);
	}

private:
	LightState lightState;
	bool readyToSwitchState;
	Time time;
	
};

#endif /* defined(__Project__danceState__) */
