/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
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


#ifdef _cplusplus
extern "C" {
#endif

	namespace Mogi {
		namespace Math {

			MotionControl::MotionControl() :
			controlMode(LOCATION), maxSpeed(1), locationState(STOPPED), velocity(0), maxAcceleration(1), goalPosition(0), position(0) {
				acc[1] = 0;
				acc[3] = 0;
			}

			void MotionControl::enableLocationControl() {
				if (controlMode != LOCATION) {
					controlMode = LOCATION;
					locationState = STOPPED;
#ifdef DEBUGSTATE
					std::cout << " - state is now: STOPPED" << std::endl;
#endif
					velocity *= 0;
//					speed = 0;
				}
			}

			void MotionControl::enableVelocityControl() {
				if (controlMode != VELOCITY) {
					controlMode = VELOCITY;
					acceleration *= 0;
				}
			}

			void MotionControl::setMaxSpeed(double newSpeed) {
				if (newSpeed < 0) {
					maxSpeed = 0;
				} else {
					maxSpeed = newSpeed;
				}
			}

			void MotionControl::setAcceleration(double newAcceleration) {
				if (newAcceleration < 0) {
					maxAcceleration = 0;
				} else {
					maxAcceleration = newAcceleration;
				}
			}

			double MotionControl::getMagnitude(const double& value) const {
				return fabs(value);
			}

			double MotionControl::getDirection(const double& value) const {
				return value >= 0 ? 1 : -1;
			}

			void MotionControl::setVelocity(const double& velocity) {
				this->velocity = velocity;
			}

			double MotionControl::getDistanceLeft() {
				return fabs(goalPosition - position);
			}

			void MotionControl::setGoalPosition(const double& location) {
				goalPosition = location;
			}

			double MotionControl::getGoalPosition() const {
				return goalPosition;
			}

			double MotionControl::getCurrentPosition() const {
				return position;
			}

			double MotionControl::getCurrentVelocity() const {
				return velocity;
			}

			double MotionControl::getMaxAcceleration() const {
				return maxAcceleration;
			}

			inline double p2(double t, double x, double v, double a) {
				return x + t*v + 0.5*t*t*a;
			};

			void MotionControl::updateModel(double dtime) {
				// x(n+1) = x(n) + (v(n) + a(n)*t/2)*t				// 1 + 1 + 1 +
				// 1
				// x(n+1) = x(n) + v(n)*t + a(n)*t*t/2				// 1 + 1 + 1 +
				// 1
				// x(n+1) = x(n) + (v(n) + a(n)*t)*t - a(n)*t*t/2	// 1 + 1 + 1 + 1 + 1 + 1
				// x(n+1) = x(n) + v(n+1)*t - a(n)*t*t/2			// 1 + 1 + 1 + 1
				// x(n+1) = x(n) + (v(n+1) - a(n)*t/2)*t			// 1 + 1 + 1 + 1
				position = position + (velocity + (acceleration / (double)2.0)* dtime) * dtime;
				// v(n+1) = v(n) + a(n)*t
				velocity = velocity + acceleration * dtime;
//				speed = getMagnitude(velocity);// get the current speed as a double value
//				if (speed > maxSpeed) {
//					// std::cout << "Time to switch to moving!" << std::endl;
//					// Note on coverage: This should never run if getNextStateChangeTime() is correct
//					// This DOES run when in velocity control mode when there is a maximum velocity set.
//					velocity = velocity / speed * maxSpeed;
//					speed = maxSpeed;// get the current speed as a double value
//				}
			}

			double MotionControl::getTimeToStop() {
				determineStateTimes();
				return dt[0] + dt[1] + dt[2];
			}

			double MotionControl::getSoonestStoppingLocation() {
				return square(velocity) / ((double)2.0 * maxAcceleration)*getDirection(velocity) + position;
			}

			void MotionControl::determineStateTimes() {
				// https://www.researchgate.net/profile/Erik_Weitnauer/publication/224339798_On-line_planning_of_time-optimal_jerk-limited_trajectories/links/00b7d52bf1a158f775000000.pdf

				double currentStoppingLocation = square(velocity) / ((double)2.0 * maxAcceleration)*getDirection(velocity) + position;
				double d = getDirection(getDifference(goalPosition, currentStoppingLocation));
				double v = d * maxSpeed;

				acc[0] = d*maxAcceleration;
				acc[2] = -d*maxAcceleration;

				// trapezoid times::
				dt[0] = (v - velocity) / acc[0];
				dt[2] = (v) / -acc[2];

				// If maxSpped is called while velocity is higher than the new setting, this handles the acceleraiton direction.
				if(dt[0] < 0) {
					dt[0] = -dt[0];
					acc[0] = -acc[0];
				}

				dx[0] = p2(dt[0], 0, velocity, acc[0]);
				dx[2] = p2(dt[2], 0, v, acc[2]);

				if (maxSpeed == 0) {
					dt[1] = 0;
					dt[2] = 0;
					return;
				} else {
					dt[1] = (goalPosition - (position + dx[0] + dx[2])) / v;
				}

				if (dt[1] <= 0) {
					// Use wedge, not trapezoidal.
					double magPeakV = sqrt(maxAcceleration*fabs(getDifference(goalPosition, position)) + 0.5*velocity*velocity);
					dt[0] = (d*magPeakV - velocity) / acc[0];
					dt[1] = 0;
					dt[2] = d*magPeakV / -acc[2];
				}

			}

			const double& MotionControl::processLocationControl(double dtime) {
				if (controlMode == VELOCITY) {
					updateModel(dtime);
					if (velocity == 0) {
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

				determineStateTimes();

				for (int i = 0; i < 3; i++) {
					acceleration = acc[i];
					if (dtime < dt[i]) {
						updateModel(dtime);
						locationState = (LocationState)i;
						// corner case?
						if (dt[i] == INFINITY) {
							locationState = STOPPED;
						}
						return position;
					}
					updateModel(dt[i]);
					dtime -= dt[i];
				}

				// update the model one last time, but really we know where we should be and what the velocity should be.
				position = goalPosition;
				velocity = 0;
				acceleration = 0;

				locationState = STOPPED;

				return position;
			}

			MotionControlMultiple::MotionControlMultiple( unsigned int count )
			:count(count){
				motionControllers = new MotionControl[count];
			}

			MotionControlMultiple::~MotionControlMultiple() {
				delete [] motionControllers;
			}

			Vector MotionControlMultiple::process(double dtime) {
				Vector result(count);

				for (unsigned int i = 0; i < count; i++) {
					result(i) = motionControllers[i].processLocationControl(dtime);
				}

				return result;
			}

			void MotionControlMultiple::setGoal(const Vector& goal) {
				double T = 0;
				double t3[count];
				unsigned int maxIndex = 0;
				for (unsigned int i = 0; i < count; i++) {
					motionControllers[i].setGoalPosition(goal.valueLinearIndex(i));
					motionControllers[i].setMaxSpeed(maxS);
					motionControllers[i].setAcceleration(maxA);
					motionControllers[i].enableLocationControl();

					t3[i] = motionControllers[i].getTimeToStop();
					if (t3[i] > T) {
						T = t3[i];
						maxIndex = i;
					}
				}

				if(T == 0) {
					return;
				}

				Vector v(count);	// new max speed for each motion controller
				for (unsigned int i = 0; i < count; i++) {
					v(i) = fabs((motionControllers[i].getGoalPosition() - motionControllers[i].getSoonestStoppingLocation()) / (T - fabs(motionControllers[i].getCurrentVelocity())/motionControllers[i].getMaxAcceleration()));

					// Need to be careful here, since once we are in the deceleration phase, the duration may closely match the duration, causing division by 0:
					if(v(i) != v(i) || v(i) > maxS) {
						v(i) = maxS;
					}
				}

				bool allZero = true;
				for (unsigned int i = 0; i < count; i++) {	// We may be at the goal in every axis, so don't mess things up:
					if (v(i) != 0) {
						allZero = false;
						break;
					}
				}
				if (allZero) {	// nothing to be done.
					return;
				}

				// Here we have a non-zero, non-nan, and non-inf values in the vector:
				v.normalize();
				v *= fabs(maxS);
				for (unsigned int i = 0; i < count; i++) {
					motionControllers[i].setMaxSpeed(v(i));
				}

			}
			void MotionControlMultiple::setMaxSpeed(const double& max) {
				maxS = max;
			}
			void MotionControlMultiple::setMaxAcceleration(const double& max) {
				maxA = max;
			}
			Vector MotionControlMultiple::getCurrentPosition() {
				Vector result(count);
				for (unsigned int i = 0; i < count; i++) {
					result(i) = motionControllers[i].getCurrentPosition();
				}
				return result;
			}
			Vector MotionControlMultiple::getCurrentVelocity() {
				Vector result(count);
				for (unsigned int i = 0; i < count; i++) {
					result(i) = motionControllers[i].getCurrentVelocity();
				}
				return result;
			}
			Vector MotionControlMultiple::getGoalPosition() {
				Vector result(count);
				for (unsigned int i = 0; i < count; i++) {
					result(i) = motionControllers[i].getGoalPosition();
				}
				return result;
			}
			Vector MotionControlMultiple::getSoonestStoppingLocation() {
				Vector result(count);
				for (unsigned int i = 0; i < count; i++) {
					result(i) = motionControllers[i].getSoonestStoppingLocation();
				}
				return result;
			}

			bool MotionControlMultiple::isStopped() {
				for (unsigned int i = 0; i < count; i++) {
					if(motionControllers[i].getState() != MotionControl::STOPPED) {
						return false;
					}
				}
				return true;
			}


			double loopRange(double x) {
				if (x < 0) {
					return -(fmod(MOGI_PI - x, 2.0*MOGI_PI) - MOGI_PI);
				}
				return fmod(MOGI_PI + x, 2.0*MOGI_PI) - MOGI_PI;
			}
			
			void MotionControlAngular::setGoalPosition(const double& heading) {
				MotionControl::setGoalPosition( loopRange(heading) );
			}
			
			double MotionControlAngular::getDistanceLeft() {
				double result = fabs( loopRange(goalPosition - position) );
				return result;
			}
			
			void MotionControlAngular::updateModel(double dtime) {
				MotionControl::updateModel(dtime);
				position = loopRange(position);
			}

			double MotionControl::getDifference(const double& to, const double& from) const {
				return to - from;
			}

			double MotionControlAngular::getDifference(const double& to, const double& from) const {
				return loopRange(to - from);
			}
			

		}
	}
	
#ifdef _cplusplus
}
#endif
