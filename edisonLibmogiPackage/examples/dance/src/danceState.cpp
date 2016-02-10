//
//  cpp
//  Project
//
//  Created by Matt Bunting on 12/23/14.
//
//

#include "danceState.h"

void DanceState::changeStates( State newState, Hexapod* hexapod ) {
	if (hexapod->isWalkable()) {
		readyToSwitchState = true;
	}

	if ((readyToSwitchState || (state == SOFT_SLEEP && newState == SOFT_WAKE))
		&& (state != newState)) {
		//if (state == fistpump && hexapod->WD_enabled) {
		if (state == fistpump && hexapod->isWalkable()) {
			return;
		}
		if (state == MEANDER) {
			//hexapod->enableLocationControl();
		} else {
			//hexapod->enableVelocityControl();
		}
		state = newState;
	}

	if (state == SOFT_SLEEP) {
		readyToSwitchState = false;
	}

}

void DanceState::setLightState( LightState newLightState ) {
	if (lightState == newLightState) {
		return;
	}

	int previousSize = lights.size();

	switch (newLightState) {
		case ALL:
			for (unsigned char i = 0; i < 8; i++) {
				lights.push_back(i);
			}
			break;

		case BLUE:
			lights.push_back(XL_BLUE);
			break;
		case RED:
			lights.push_back(XL_RED);
			break;
		case GREEN:
			lights.push_back(XL_GREEN);
			break;
		case OFF:
			lights.push_back(XL_LED_OFF);
			break;
		case XMAS:
			lights.push_back(XL_RED);
			lights.push_back(XL_RED);
			lights.push_back(XL_GREEN);
			lights.push_back(XL_GREEN);
			lights.push_back(XL_RED);
			lights.push_back(XL_GREEN);
			lights.push_back(XL_LED_OFF);
			lights.push_back(XL_RED);
			lights.push_back(XL_RED);
			lights.push_back(XL_GREEN);
			lights.push_back(XL_RED);
			lights.push_back(XL_GREEN);
			lights.push_back(XL_LED_OFF);
			break;

		default:
			return;
			break;
	}
	lightState = newLightState;

	for (int i = 0; i < previousSize; i++) {
		lights.erase(lights.begin());
	}
}

void DanceState::checkLimits() {
	double *value = &xLeft;
	for( int i = 0; i < 3; i++)
	if (value[i] < -1) {
		value[i] = -1;
	} else if(value[i] > 1) {
		value[i] = 1;
	}
}

void DanceState::updateStates( Hexapod* hexapod ) {
	time.update();

	if (controlMode == KEYBOARD) {
		double *value = &xLeft;
		for( int i = 0; i < 3; i++)
			value[i] *= .985;
	}
	

	switch(state)
	{
		case MEANDER:
		case walking:
		case WALK_BEFORE_SLEEP:
			walkingMagnitude += 2*time.dTime();
			break;

		case headbob:
			headBobMagnitude += 2*time.dTime();
			break;

		case quaternion:
			quaternionMagnitude += 2*time.dTime();
			break;

		case fistpump:
			if( hexapod->isIdle() )	// Walking dynamics really messes up control
			{
				fistPumpMagnitude += 2*time.dTime();
			}
			break;

		default:
			break;
	}

	walkingMagnitude	-= time.dTime();
	headBobMagnitude	-= time.dTime();
	quaternionMagnitude	-= time.dTime();
	fistPumpMagnitude	-= time.dTime();

	if(walkingMagnitude < 0)
		walkingMagnitude = 0;
	if(walkingMagnitude > 1)
		walkingMagnitude = 1;
	if(headBobMagnitude < 0)
		headBobMagnitude = 0;
	if(headBobMagnitude > 1)
		headBobMagnitude = 1;
	if(quaternionMagnitude < 0)
		quaternionMagnitude = 0;
	if(quaternionMagnitude > 1)
		quaternionMagnitude = 1;
	if(fistPumpMagnitude < 0)
		fistPumpMagnitude = 0;
	if(fistPumpMagnitude > 1)
		fistPumpMagnitude = 1;
}

bool DanceState::dealWithInput( char keyPressed, Hexapod* hexapod ) {
	switch(keyPressed)
	{
		case '1':
			changeStates(walking, hexapod);
			break;

		case '2':
			changeStates(quaternion, hexapod);
			break;

		case '3':
			changeStates(fistpump, hexapod);
			break;

		case '4':
			changeStates(headbob, hexapod);
			break;

		case '5':
			changeStates(MEANDER, hexapod);
			break;

		case '-':
		case '_':
			changeStates(WALK_BEFORE_SLEEP, hexapod);
			break;

		case '+':
		case '=':
			changeStates(SOFT_WAKE, hexapod);
			break;

		case 'g':
		case 'G':
			setLightState(GREEN);
			break;

		case 'b':
		case 'B':
			setLightState(BLUE);
			break;

		case 'r':
		case 'R':
			setLightState(RED);
			break;

		case 'c':
		case 'C':
			setLightState(XMAS);
			break;

		case 'm':
		case 'M':
			setLightState(ALL);
			break;

		case 'o':
		case 'O':
			setLightState(OFF);
			break;

		case 'v':
		case 'V':
			verbose ^= 1;
		//	clear();
			break;

		case 'w':
		case 'W':
			yLeft += .25;
			break;

		case 's':
		case 'S':
			yLeft -= .25;
			break;

		case 'd':
		case 'D':
			xLeft += .25;
			break;

		case 'a':
		case 'A':
			xLeft -= .25;
			break;

		case 'j':
		case 'J':
			xRight -= .25;
			break;

		case 'l':
		case 'L':
			xRight += .25;
			break;

		case 3: // ctrl-c
			std::cout << "Caught Ctrl-C in DanceState::dealWithInput" << std::endl;
		case 27: // escape
			changeStates(walking, hexapod);
			return true;
			break;

		default:
			break;
	}

	return false;
}


