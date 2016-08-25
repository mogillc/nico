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

#include "walkingdynamics.h"

#include <math.h>
#include <iostream>

#ifdef IDENT_C
static const char* const RigidGait_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
//using namespace Dynamixel;
using namespace Math;

namespace Robot {

RigidGait::RigidGait() {
	for (int i = 0; i < 6; i++) {
		//feetLocations.push_back(Vector(3));
		//footPivotForWalking.push_back(Vector(3));
		footDestinationForWalking.push_back(Vector(3));
		footLastLocationOnGroundForWalking.push_back(Vector(3));
	}

	setGait(1);
}

void RigidGait::perform(const double& dtime) {
	double theta;
	double ytemp, ztemp;  // Used for the Trajectory function
	//		static int gaitindexold = gaitindex;
	static int needToComputefootDestinationForWalkingination[6];
	static Vector previous_p[6];
	static bool first_time = true;
	static bool upflag[6] = { 0 };
	Vector velocity_vector(3), current_p(3);
	// double p_magnitude, p_angle;//, p_velocity;
	// double step_alpha = 0.45;

	// velocity_vector.name("velocity_vector");
	// current_p.name("current_p");

	static double footLiftHeight[6];

	if (first_time)  // Initialize velocity measurement stuff
	{
		for (unsigned int i = 0; i < 6; i++) {
			previous_p[i] = footPivotForWalking[i];
			previous_p[i].name("previous_p");
			// previous_p[i].print_stats();
			needToComputefootDestinationForWalkingination[i] = true;
			// New initialization:
			footLocations[i] = footPivotForWalking[i];
			footLastLocationOnGroundForWalking[i] = footPivotForWalking[i];
		}

		footTimeForTrajectoryFunctionsForWalking = 0;
	}
	// std::cerr << "RigidGait:" << std::endl;

	footTimeForTrajectoryFunctionsForWalking += dtime / steppingTime; // Increment the index, based on the loop time and
	// the desired leg lift time

	// Trajectory function can be used for simplistic leg motion, but is useful
	// for determining when to lift a leg and having half a circle drawn for the
	// lifting motion
	for (unsigned int i = 0; i < 6; i++) {
		theta = footTimeForTrajectoryFunctionsForWalking
				* (2 * MOGI_PI - switchpoint) + phase[i]; // Determine values for each leg for Traj function
		footTrajectory(&ytemp, &ztemp, &upflag[i], theta, switchpoint); // Semicircle Trajectory function (see below)...

		current_p = footPivotForWalking[i];

		velocity_vector = (current_p - previous_p[i]) / dtime; // This needs to be computed on every iteration
		// for instantaneous velocity.

		previous_p[i] = current_p;

		if (upflag[i] == 1) { // Foot is in the air according to the trajectory function

			// If leg is in the lifted state from function:
			//				if(gaitindexold == gaitindex)
			//				{
			if (needToComputefootDestinationForWalkingination[i] == true) {
				// footp[i] = footPivotForWalking[i];			// The pivot points
				// move out further when walking faster

				// From the old position in the down state, we could simply place the
				// foot back into the relative resting or initial position,
				// OR we could move beyond the resting position by a specific amount.
				// This could make the motion seem more natural.  Here, I determine the
				// distance needed to move the leg from the old position to the resting
				// position, then I over shoot by a factor of .45 more.  Basically, if
				// the
				// leg were to move a distance of 100 to get from the old position to
				// resting position, the leg would actually move a total distance of
				// 145.
				// This is basically a dampened oscillator.  This can only converge if
				// the value is less than 1.  I found .45 to work good, but this is more
				// an
				// art than it is science.

				static const double maxSteppingRadiusFromPivot = 0.001;

				double percentAdditionalStep = velocity_vector.magnitude()
						/ 100;  /// maxSteppingRadiusFromPivot;
				if (percentAdditionalStep > 1.0) {
					percentAdditionalStep = 1.0;
				}
				// std::cerr << " - leg " << i << " percentAdditionalStep = " <<
				// percentAdditionalStep << std::endl;

				// Foot destination position:
				static Vector additionalMovement = percentAdditionalStep
						* (current_p - footLastLocationOnGroundForWalking[i]);
				if (additionalMovement.magnitude()
						> maxSteppingRadiusFromPivot) {
					// std::cout << "foot " << i << " exceeds range! magnitude of
					// additiona step = " << additionalMovement.magnitude() << std::endl;
					additionalMovement.normalize();
					additionalMovement = maxSteppingRadiusFromPivot
							* additionalMovement;
				} else {
					// std::cout << "foot " << i << " Doesn't exceed range! magnitude of
					// additiona step = " << additionalMovement.magnitude() << std::endl;
				}
				footDestinationForWalking[i] = current_p + additionalMovement;

				double footTravelDistance =
						((const Vector) (footDestinationForWalking[i]
								- footLastLocationOnGroundForWalking[i])).magnitude();

				// If foot is within 5mm of pivot, no need to try to step closer
				if (((const Vector) (footDestinationForWalking[i]
						- footLastLocationOnGroundForWalking[i])).magnitude()
						< 1.0) {
					footDestinationForWalking[i](0) =
							footLastLocationOnGroundForWalking[i](0);
					footDestinationForWalking[i](1) =
							footLastLocationOnGroundForWalking[i](1);
					footDestinationForWalking[i](2) = 0;

					footLiftHeight[i] = 0.0;
					// needToComputefootDestinationForWalkingination[i] = true;	//
				} else {
					//							footLiftHeight[i] =
					//sqrt(footTravelDistance) * 7.5;	// Lift height from an arbitrary
					//function

					footLiftHeight[i] = clamp(sqrt(footTravelDistance) * 7.5,
							minimumFootLiftHeight, maximumFootLiftHeight);
				}
				needToComputefootDestinationForWalkingination[i] = false;
			}

			if (needToComputefootDestinationForWalkingination[i] == false) // use the previously computed information
					{
				// We lift the leg a certain amount based on the leg travel distance.
				// The longer the travel, the higher the leg lifts

				// footDestinationForWalking[i](2) = travel[i] * ztemp +
				// footPivotForWalking[i](2);

				// ytemp goes from -1 to 1 from Traj() in lift mode.  For smooth motion,
				// we "blend" the initial position and final position.
				// If ytemp = -1, foot = footLastLocationOnGroundForWalking.  If ytemp =
				// 1, foot = footDestinationForWalking
				footLocations[i] = (((ytemp + 1.) / 2.)
						* footDestinationForWalking[i]
						- ((ytemp - 1.) / 2.)
								* footLastLocationOnGroundForWalking[i]);

				// we don't care about blending the height, just use the result from the
				// trajectory function:
				footLocations[i](2) = footLiftHeight[i] * ztemp; // + footPivotForWalking[i](2);
			}

		} else {  // If leg is in down mode...
			needToComputefootDestinationForWalkingination[i] = true;
			footLastLocationOnGroundForWalking[i] = footLocations[i]; // always store last position (handles x,y and z)
			footLocations[i](2) = 0;
			// Note: we do not adjust foot x, y and z here, as they are touching the
			// gound and the ground is stationary.
		}
	}

	first_time = false;
}

void RigidGait::setGait(int gait) {
	static int gaitindexold = 0;
	//double sum_foot_height = 0;

	// Modify the Gait, only if all feet are touching the ground
//				for (int i = 0; i < 6; i++) sum_foot_height += foot[i](2);
	if (      //(sum_foot_height == 0) &&
	(gaitindexold != gait)) {
		gaitindexold = gait;

		switch (gait) {
		case 1:
			// Ripple Gait:
			switchpoint = 4.01 * MOGI_PI / 3;
			steppingTime = .75;
			phase[2] = 0;
			phase[3] = phase[2] + (2 * MOGI_PI / 3);
			phase[4] = phase[3] + (2 * MOGI_PI / 3);
			phase[1] = phase[4] + (5 * MOGI_PI / 3);
			phase[0] = phase[1] + (2 * MOGI_PI / 3);
			phase[5] = phase[0] + (2 * MOGI_PI / 3);
			break;
		case 3:
			// Tripod Gait:
			steppingTime = .7;
			switchpoint = 25. * MOGI_PI / 24.; // 13*MOGI_PI/12;//7 * MOGI_PI / 6;
			phase[0] = 0;
			phase[2] = 0;
			phase[4] = 0;
			phase[1] = MOGI_PI;
			phase[3] = MOGI_PI;
			phase[5] = MOGI_PI;
			break;
		case 4:
			// Quad Gait:
			steppingTime = .6;
			switchpoint = 13 * MOGI_PI / 8;
			phase[1] = 0;
			phase[0] = MOGI_PI / 2;
			phase[2] = MOGI_PI / 2;
			phase[5] = MOGI_PI;
			phase[3] = MOGI_PI;
			phase[4] = 3 * MOGI_PI / 2;
			break;
		case 6:
			// Wave Gait:
			switchpoint = 21 * MOGI_PI / 12;
			steppingTime = .6;
			phase[2] = 0;
			phase[3] = phase[2] + (MOGI_PI / 3);
			phase[4] = phase[3] + (MOGI_PI / 3);
			phase[1] = phase[4] + (MOGI_PI / 3);
			phase[0] = phase[1] + (MOGI_PI / 3);
			phase[5] = phase[0] + (MOGI_PI / 3);
			break;
		default:
			std::cout << "WARNING: Unrecognized gait: " << gait << std::endl;
			break;
		}
		steppingTime *= 0.75;
		//			if (legType == HexapodLeg::NICO) {
		//				steppingTime *= 0.75;
		//			}
	}

}

void footTrajectory(double* outx, double* outy, bool* upflag, double input,
		double switchpoint) {
	double temp;

	while (input < 0.0)  // terrible way to handle negative number.
		input += 2.0 * MOGI_PI;
	input = fmod(input, 2.0 * MOGI_PI);

	if (input < switchpoint) {  // linear region
		*outx = 1.0 - 2.0 * input / switchpoint;
		*outy = 0.0;
		*upflag = 0;
	} else {  // circular region
		temp = (input - 2.0 * MOGI_PI) * MOGI_PI
				/ (switchpoint - 2.0 * MOGI_PI);
		*outx = cos(temp);
		*outy = sin(temp);
		*upflag = 1;
	}
}

//void footTrajectory2(double* outx, double* outy, bool* upflag, double input,
//		double switchpoint) {
//	double temp;
//
//	while (input < 0.0)  // terrible way to handle negative number.
//		input += 2.0 * MOGI_PI;
//	input = fmod(input, 2.0 * MOGI_PI);
//
//	if (input < switchpoint) {  // linear region
//		*outx = 1.0 - 2.0 * input / switchpoint;
//		*outy = 0.0;
//		*upflag = 0;
//	} else {  // lift region
//		temp = (input - 2.0 * MOGI_PI) * MOGI_PI
//				/ (switchpoint - 2.0 * MOGI_PI);  // + 2.0 * MOGI_PI;
//		double temp2 = cos(temp);
//		*outx = -(1.1 * (temp2 - 1) * (temp2 + 1) - temp2);
//		*outy = 3.2 * (1.0 - square(sin(temp) - 1.0)) * square(temp / MOGI_PI)
//				* (temp / MOGI_PI) * (MOGI_PI - temp);
//		*upflag = 1;
//	}
//}
//
//void footTrajectory3(double* outx, double* outy, bool* upflag, double input,
//		double switchpoint) {
//	double temp;
//	double p = 2.0;  // 0.75;
//
//	while (input < 0.0)  // terrible way to handle negative number.
//		input += 2.0 * MOGI_PI;
//	input = fmod(input, 2.0 * MOGI_PI);
//
//	if (input < switchpoint) {  // linear region
//		*outx = 1.0 - 2.0 * input / switchpoint;
//		*outy = 0.0;
//		*upflag = 0;
//	} else {  // lift region
//		temp = (input - 2.0 * MOGI_PI) * MOGI_PI
//				/ (switchpoint - 2.0 * MOGI_PI);  // + 2.0 * MOGI_PI;
//		*outx = -pow(1.0 - cos(temp), p) / pow(2.0, p - 1) + 1.0;
//		//*outx *= -1;
//		//*outx = -temp/MOGI_PI * 2 + 1;
//		//*outy = 3.2*(1.0 - square(sin(temp) - 1.0)) *
//		//square(temp/MOGI_PI)*(temp/MOGI_PI) * (MOGI_PI - temp);
//		//*outy = (sin(temp) + sin(3*temp)/2.45) * (MOGI_PI - temp)/(MOGI_PI *
//		//0.78);
//		*outy = pow(sin(temp), 1.5);
//
//		//*outx = 1.0 - 2.0 * pow(1.0 - pow((cos(temp) + 1.0)/2.0, p), 1.0/p);
//		//*outy = pow(sin(temp), p < 1.0 ? p : 1.0/p);
//		*upflag = 1;
//	}
//}
//
//// Rectangular trajectory:
//void footTrajectory4(double* outx, double* outy, bool* upflag, double input,
//		double switchpoint) {
//	double temp;
//	// double p = 2.0;//0.75;
//
//	while (input < 0.0)  // terrible way to handle negative number.
//		input += 2.0 * MOGI_PI;
//	input = fmod(input, 2.0 * MOGI_PI);
//
//	if (input < switchpoint) {  // linear region (bottom of rectangle
//		*outx = 1.0 - 2.0 * input / switchpoint;
//		*outy = 0.0;
//		*upflag = 0;
//	} else {  // lift region
//		// temp is in the range of (0 to 1) when input goes from (switchpoint to
//		// 2pi)
//		temp = 1.0 - (input - 2.0 * MOGI_PI) / (switchpoint - 2.0 * MOGI_PI);
//
//		const double switchPoint1 = 0.4;
//		const double switchPoint2 = 0.8;
//
//		if (temp < switchPoint1) {
//			*outx = -1.0;
//			*outy = temp / switchPoint1;
//		} else if (temp < switchPoint2) {
//			*outx = 2 * (temp - (switchPoint2 + switchPoint1) / 2.0)
//					/ (switchPoint2 - switchPoint1);
//			*outy = 1;
//		} else {
//			*outx = 1.0;
//			*outy = 1.0 - ((temp - switchPoint2) / (1.0 - switchPoint2));
//		}
//		*outy = sqrt(*outy);
//
//		*upflag = 1;
//	}
//}

}
}

#ifdef _cplusplus
}
#endif
