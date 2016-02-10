/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#ifdef IDENT_C
static const char* const MotionControl_C_Id = "$Id$";
#endif

#include "systems.h"

#include <math.h>
#include <iostream>

#include <iomanip>

#include <float.h>

//#define DEBUGSTATE

#define method(return, signature)					\
template return MotionControl<double>::signature;	\
template return MotionControl<Vector>::signature;	\
template <class T>									\
return MotionControl<T>::signature

#define methodReturnT(signature)                    \
template double MotionControl<double>::signature;	\
template Vector MotionControl<Vector>::signature;	\
template <class T>									\
T MotionControl<T>::signature

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

template<>
MotionControl<Vector>::MotionControl() :
		controlMode(LOCATION), tolerance(0.000000001), maxSpeed(1), locationState(
				STOPPED), speed(0), maxAcceleration(1) {
}

template MotionControl<double>::MotionControl();
template<class T>
MotionControl<T>::MotionControl() :
		controlMode(LOCATION), tolerance(0.000000001), maxSpeed(1), locationState(
				STOPPED), velocity(0), speed(0), maxAcceleration(1), goalPosition(
				0), position(0) {

}

method(void, enableLocationControl()){
if (controlMode != LOCATION) {
	controlMode = LOCATION;
	locationState = STOPPED;
#ifdef DEBUGSTATE
	std::cout << " - state is now: STOPPED" << std::endl;
#endif
	velocity *= 0;
	speed = 0;
}
}

method(void, enableVelocityControl()){
if (controlMode != VELOCITY) {
	controlMode = VELOCITY;
	acceleration *= 0;
}
}

method(void, setMaxSpeed(double newSpeed)){
if (newSpeed < DBL_MIN) {
	maxSpeed = DBL_MIN;
} else {
	maxSpeed = newSpeed;
}
}

method(void, setAcceleration(double newAcceleration)){
if (newAcceleration < DBL_MIN) {
	maxAcceleration = DBL_MIN;
} else {
	maxAcceleration = newAcceleration;
}
}

template<>
long double MotionControl<Vector>::getMagnitude(const Vector& value) const {
	return value.magnitude();
}
template long double MotionControl<double>::getMagnitude(
		const double& value) const;
template<class T>
long double MotionControl<T>::getMagnitude(const T& value) const {
	return fabs(value);
}

template<>
void MotionControl<Vector>::setVelocity(const Vector& newVelocity) {
	velocity = newVelocity;
	if (position.size() != newVelocity.size()) {
		position.setLength(newVelocity.size());
		// velocity.set_size(newVelocity.size());
		acceleration.setLength(newVelocity.size());
	}
}
template void MotionControl<double>::setVelocity(const double& velocity);
template<class T>
void MotionControl<T>::setVelocity(const T& velocity) {
	this->velocity = velocity;
}

template<>
long double MotionControl<Vector>::getDistanceLeft() {
	Vector vectorToGo = goalPosition - position;
	return vectorToGo.magnitude();
}
template long double MotionControl<double>::getDistanceLeft();
template<class T>
long double MotionControl<T>::getDistanceLeft() {
	return fabs(goalPosition - position);
}

method(void, updateState()){
// This determines the stopping distance based on acceleration and current
// speed.
long double currentStoppingDistance = square(speed) / (2.0 * maxAcceleration);
T stopLocation = position + currentStoppingDistance * velocity / speed;
if (speed < tolerance) {
	stopLocation = position;
} else {
	stopLocation = position + currentStoppingDistance * velocity / speed;
}

long double magnitudeStoppingLocationToGoalPosition =
getMagnitude(stopLocation - goalPosition);

//  			std::cout << " - dtimeForNow:  " << dtimeForNow <<
//  std::endl;
//	std::cout << " - wvelocity:     " << getMagnitude(velocity) << std::endl;
//	std::cout << " - acceleration: " << getMagnitude(acceleration) << std::endl;
//	std::cout << " - goal pos:     " << getMagnitude(goalPosition) << std::endl;
//	std::cout << " - position:     " << getMagnitude(position) << std::endl;
//	std::cout << " - stopLocation: " << getMagnitude(stopLocation) << std::endl;

// So I could implement a recursive method here but then all of the above
// needs to be recomputed every time.
bool repeat = true;
while (repeat) {  // repeat every time we switch states to ensure that we
	// don't need to switch again
	repeat = false;

	switch (locationState) {
		case STOPPED:
		//		std::cout << "state is STOPPED" << std::endl;
		if (magnitudeStoppingLocationToGoalPosition >
				tolerance) {  // Goal is not equal to position
			locationState = ACCELERATION;
#ifdef DEBUGSTATE
			std::cout << " - state is now: ACCELERATION" << std::endl;
#endif
			repeat = true;
		}
		break;

		case ACCELERATION:
		//				std::cout << "state is ACCELERATION" <<
		//std::endl;
		if (magnitudeStoppingLocationToGoalPosition <= tolerance) {
			locationState = DECELERATING;
#ifdef DEBUGSTATE
			std::cout << " - state is now: DECELERATING" << std::endl;
#endif
			repeat = true;
		} else if (speed == maxSpeed) {  // Speed is at the max, begin moving
			locationState = MOVING;
#ifdef DEBUGSTATE
			std::cout << " - state is now: MOVING" << std::endl;
#endif
			repeat = true;
		}
		break;

		case MOVING:
		//			std::cout << "state is MOVING" << std::endl;
		if (magnitudeStoppingLocationToGoalPosition <=
				tolerance) {  // Equal at the point when we need to begin
			// deceleration
			locationState = DECELERATING;
#ifdef DEBUGSTATE
			std::cout << " - state is now: DECELERATING" << std::endl;
#endif
			repeat = true;
		}
		break;

		case DECELERATING:
		if (magnitudeStoppingLocationToGoalPosition >
				tolerance) {  // Goal position was changed
			locationState = ACCELERATION;
#ifdef DEBUGSTATE
			std::cout << " - state is now: ACCELERATION" << std::endl;
#endif
			repeat = true;
		} else if (getDistanceLeft() < tolerance) {  // We are complete
			position = goalPosition;
			locationState = STOPPED;
#ifdef DEBUGSTATE
			std::cout << " - state is now: STOPPED" << std::endl;
#endif
			repeat = true;
		}

		break;
	}
}
}

template<>
void MotionControl<Vector>::setGoalPosition(const Vector& location) {
	goalPosition = location;
	if (position.size() != location.size()) {
		position.setLength(location.size());
		velocity.setLength(location.size());
		acceleration.setLength(location.size());
	}
	updateState();
}
template void MotionControl<double>::setGoalPosition(const double& location);
template<class T>
void MotionControl<T>::setGoalPosition(const T& location) {
	goalPosition = location;
	updateState();
}

methodReturnT(getDirectionToGoal() const){
T locationVector = goalPosition - position;
return locationVector / getMagnitude(locationVector);
}

method(void, calculateAcceleration()){
switch (locationState) {
	case STOPPED:
	case MOVING:
	acceleration *= 0;
	break;

	case ACCELERATION:
	// Acceleration = desired acceleration * unit vector to goal location
	acceleration = maxAcceleration * getDirectionToGoal();
	break;

	case DECELERATING:
	//Acceleration = -1 * desired acceleration * unit vector to goal location
	acceleration = maxAcceleration * getDirectionToGoal() * (long double)(-1.0);
	break;
}
}

  // There is never a need to call this when in the STOPPED state
  // THE ACCURACY OF THIS METHOD IS CRITICAL FOR updateState() TO WORK PROPERLY
method(long double, getNextStateChangeTime(double dtime)){
long double result = 0;

if (locationState == ACCELERATION) {
	// double magSpeed = getMagnitude(velocity);
	// simple case, time from current to max speed based on acc:
	result = (maxSpeed - speed) / maxAcceleration;

	//	Ok so what if maxSpeed is too high? Let's solve for the time to accelerate then decelerate to a stop:
	// some models for derivation:
	// distA = 0 + v*t + 1/2 * maxA*t*t
	// vs = v + maxA*t						// velocity at switch point
	// td = vs/maxA							// time for deceleration once at vs
	// dT = distA + dD						// total distance (known)
	// dD = vs*vs/(2*maxA)					// distance for deceleration
	//
	// want to know t
	// dT = vs*vs/(2*maxA) + v*t + 1/2maxA *t*t
	// dT = v*v/(2*maxA) + 2*v*maxA*t/(2*maxA) + maxA*maxA*t*t/(2*maxA) + v*t + 1/2*maxA *t*t
	// dT = v*v/(2*maxA) + v*t                 + maxA*t*t/(2) + v*t + 1/2*maxA *t*t
	// dT = v*v/(2*maxA) + 2*v*t               + maxA*t*t
	// 0 = maxA*t*t + 2*v*t + (v*v/(2*maxA)-dT)

	//			double a = maxAcceleration;
	//			double b = 2.0*magSpeed;
	//			double c = magSpeed*magSpeed/((double)2.0*maxAcceleration) - getDistanceLeft();
	//    long double accToDecTime = ((-(long double)2.0 * speed) +
	//						   sqrt(square(2.0 * speed) - (long double)4.0 * maxAcceleration * (speed * speed / ((long double)2.0 * maxAcceleration) -
	//                   getDistanceLeft()))) / ((long double)2.0 * maxAcceleration);

	long double a = maxAcceleration;
	long double b = 2.0*speed;
	long double c = speed*speed/((long double)2.0*maxAcceleration) - getDistanceLeft();
	long double accToDecTime = (-b + sqrt(square(b) - (long double)4.0 * a*c)) / ((long double)2.0 * a);
//					std::cout << "maxSpeed = " << maxSpeed << std::endl;
//					std::cout << "maxAcceleration = " << maxAcceleration << std::setprecision(30) << std::fixed << std::endl;
//					std::cout << "speed = " << speed << std::endl;
//					std::cout << "getDistanceLeft() = " << getDistanceLeft() << std::endl;
//					std::cout << "(-b + sqrt(square(b) - (long double)4.0 * a*c) = " << (-b + sqrt(square(b) - (long double)4.0 * a*c)) << std::endl;
//					std::cout << "accToDecTime = " << accToDecTime << std::endl;
//					std::cout << "result = " << result << std::endl;
//					std::cout << "locationState = " << stateToString(locationState) << std::endl;
	if (accToDecTime < result) {
		result = accToDecTime > 0 ? accToDecTime : 0;

		//locationState = DECELERATING;	// HACK: There are precision issues when transitioning from accelerating to decelerating
	}

} else if (locationState == DECELERATING) {
	result = speed / maxAcceleration; // time from current to 0 speed based on acc
} else {                        // Then we must be in the state of MOVING:
	// If we are moving, then we are at maxSpeed (no need to compute magnitude).
	// Need to find the stopping time after undergoing deceleration from
	// maxSpeed to 0 (maxSpeed/maxAcceleration)
	// if dtime is larger than this, then return the difference. (external
	// logic?)

	long double currentStoppingDistance =
	square(maxSpeed) /
	((long double)2.0 *
			maxAcceleration);// how far the distance is for deceleration
	long double distanceForMovingState = getDistanceLeft() - currentStoppingDistance;
	result = distanceForMovingState / maxSpeed;
}

return result;
}

method(void, updateModel(long double dtime)){
// x(n+1) = x(n) + (v(n) + a(n)*t/2)*t				// 1 + 1 + 1 +
// 1
// x(n+1) = x(n) + v(n)*t + a(n)*t*t/2				// 1 + 1 + 1 +
// 1
// x(n+1) = x(n) + (v(n) + a(n)*t)*t - a(n)*t*t/2	// 1 + 1 + 1 + 1 + 1 + 1
// x(n+1) = x(n) + v(n+1)*t - a(n)*t*t/2			// 1 + 1 + 1 + 1
// x(n+1) = x(n) + (v(n+1) - a(n)*t/2)*t			// 1 + 1 + 1 + 1
position = position + (velocity + (acceleration / (long double)2.0)* dtime) * dtime;
// v(n+1) = v(n) + a(n)*t
velocity = velocity + acceleration * dtime;
speed = getMagnitude(velocity);// get the current speed as a double value
if (speed > maxSpeed) {
	// std::cout << "Time to switch to moving!" << std::endl;
	// Note on coverage: This should never run if getNextStateChangeTime() is correct
	// This DOES run when in velocity control mode when there is a maximum velocity set.
	velocity = velocity / speed * maxSpeed;
	speed = maxSpeed;// get the current speed as a double value
}
}

template const double& MotionControl<double>::processLocationControl(
		long double dtime);
template const Vector& MotionControl<Vector>::processLocationControl(
		long double dtime);
template<class T>
const T& MotionControl<T>::processLocationControl(long double dtime) {
	if (controlMode == VELOCITY) {
		updateModel(dtime);
		if (speed == 0) {
			locationState = STOPPED;
#ifdef DEBUGSTATE
			std::cout << " - state is now: STOPPED" << std::endl;
#endif
		} else {
			locationState = MOVING;
#ifdef DEBUGSTATE
			std::cout << " - state is now: MOVING" << std::endl;
#endif
		}
		return position;
	}

	//	if (locationState == ACCELERATION && dtime > 0) {
	//		calculateAcceleration();  // do action'
	//		long double dtimeForNow = getNextStateChangeTime(dtime);
	//		if (dtimeForNow > dtime) {
	//			updateModel(dtime);
	//			updateState();
	//		} else if( dtimeForNow == 0 ) {	// corner case
	//			//updateModel(dtimeForNow);
	//			locationState = DECELERATING;
	//			//dtime -= dtimeForNow;
	//			dtimeForNow = getNextStateChangeTime(dtime);
	//			if (dtimeForNow > dtime) {
	//				updateModel(dtime);
	//				updateState();
	//			} else {
	//				updateModel(dtimeForNow);
	//				locationState = DECELERATING;
	//			}
	//		}
	//	}
	//int maxIterations = 4;
	while (dtime > 0) {				// &&
		// maxIterations-- >= 0) {  // dtime becomes divided into different sections based on the state

		if (locationState == STOPPED) {  // just basic optimization
			return position;
		}

		calculateAcceleration();  // do action
		long double dtimeForNow = getNextStateChangeTime(dtime);
//					std::cout << "dtimeForNow = " << dtimeForNow << " dtime = " << dtime << std::endl;
		if (dtimeForNow > dtime) {
			dtimeForNow = dtime;
		} else if (locationState == ACCELERATION && dtimeForNow == 0) {	// corner case
//						std::cout << "WINNERWINEEREINFISDNVFISADF" << std::endl;
				//updateModel(dtimeForNow);	// will do nothing.
				//updateState();	// will also do nothing
			if (speed == maxSpeed) {
				locationState = MOVING;
			} else {
				locationState = DECELERATING;
			}
			calculateAcceleration();  // do action
			dtimeForNow = getNextStateChangeTime(dtime);
		}

		updateModel(dtimeForNow);

		updateState();

		dtime -= dtimeForNow;
	}

	return position;
}

void MotionControlAngular::setGoalPosition(const double& heading) {
	MotionControl<double>::setGoalPosition(
			fmod((double) 3.0 * (double) MOGI_PI + heading,
					(double) 2.0 * (double) MOGI_PI) - (double) MOGI_PI);
}

long double MotionControlAngular::getDistanceLeft() {
	long double result = fabs(goalPosition - position);
	if (result > MOGI_PI) {
		result = 2.0 * MOGI_PI - result;
	}
	return result;
}

void MotionControlAngular::updateModel(long double dtime) {
	MotionControl<double>::updateModel(dtime);
	position = fmod((long double) 3.0 * (long double) MOGI_PI + position,
			(long double) 2.0 * (long double) MOGI_PI) - (long double) MOGI_PI;
}

double MotionControlAngular::getDirectionToGoal() const {
	double difference = goalPosition - position;
	double magnitude = fabs(difference);
	if (magnitude > (double) MOGI_PI) {
		return -difference / magnitude;
	}
	return difference / magnitude;
}
}
}

#ifdef _cplusplus
}
#endif
