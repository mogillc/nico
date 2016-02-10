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

#include <math.h>

#include "hexapod.h"

#ifdef IDENT_C
static const char* const Hexapod_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
using namespace Math;
namespace Robot {

DmitriHeadMechanism::DmitriHeadMechanism() {
	yaw = 0;
	pitch = 0;
	roll = 0;

	yawzero = (8.203125);  //* MOGI_PI/180.0;
	pitchzero = (0);       //* MOGI_PI/180.0;
	rollzero = (-20);      //* MOGI_PI/180.0;
}

void DmitriHeadMechanism::setCalibration(double gearRatio, double servoCenter) {
	gear_ratio = gearRatio;
	motor_center = servoCenter;
}

void DmitriHeadMechanism::setAngles(const Vector& angles) {
	if (angles.size() == 3) {
		pitch = angles.valueAsConst(0, 0);
		roll = angles.valueAsConst(1, 0);
		yaw = angles.valueAsConst(2, 0);
	} else {
		std::cout << "setBodyAngles must only take a size 3 vector, not "
				<< angles.size() << std::endl;
	}
}

void DmitriHeadMechanism::setAngles(double x, double y, double z) {
	pitch = x;
	roll = y;
	yaw = z;
}

Vector DmitriHeadMechanism::getAngles() {
	Vector headAngles(3);

	headAngles(0) = pitch;
	headAngles(1) = roll;
	headAngles(2) = yaw;

	return headAngles;
}

Vector DmitriHeadMechanism::computeAngles() {
	Vector result(3);
	double pitch_t, yaw_t, roll_t;
	yaw_t = yaw * 180.0 / MOGI_PI;
	pitch_t = pitch * 180.0 / MOGI_PI;
	roll_t = roll * 180.0 / MOGI_PI;

	// first make sure we are not beyond physical limits:
	if (pitch_t > 26.506)
		pitch_t = 26.506;
	if (pitch_t < -3.7601)
		pitch_t = -3.7601;
	if (roll_t > 20)
		roll_t = 20.;
	if (roll_t < -20)
		roll_t = -20.;
	if (yaw_t > 13.75)
		yaw_t = 13.75;
	if (yaw_t < -13.75)
		yaw_t = -13.75;

	yaw_t = yaw_t - pitch_t; // apply kinematics (ugly, includes calibration parameters)
	// hex(1)aw_t = ( hex.bodytheta(1)*180./pi  + 2*hex.pitch_t)*15./24. +
	// hex.roll_t;
	roll_t = (roll_t - 10.0) + 2.0 * pitch_t + yaw_t;
	pitch_t = goofytrig(pitch_t);  // function for goofy geometry

	yaw_t *= gear_ratio;
	roll_t *= gear_ratio;

	yaw_t += yawzero;  // calibration
	roll_t += rollzero;
	pitch_t += pitchzero;

	// if(yaw_t > (yawzero + 20)) yaw_t = (yawzero + 20);	// limits in angle space
	// if(yaw_t < (yawzero - 20)) yaw_t = (yawzero - 20);
	// if(roll_t > (rollzero + 20)) roll_t = (rollzero + 20);
	// if(roll_t < (rollzero - 20))  roll_t = (rollzero - 20);
	// if(pitch_t> (pitchzero + 90)) pitch_t= (pitchzero + 90);
	// if(pitch_t< (pitchzero - 90)) pitch_t= (pitchzero - 90);

	result(0) = roll_t * MOGI_PI / 180.0;
	result(1) = pitch_t * MOGI_PI / 180.0;
	result(2) = yaw_t * MOGI_PI / 180.0;

	return result;
}

double DmitriHeadMechanism::goofytrig(double thetah) {
	double theta;  //, thetas;
	double a, b, c, d;

	//#ifdef INTELHEX // WARNING!  Modified for DMITRI!
	const double l = 45.27,  // length of linkage
			r = 8.,              // radius of motor
			xh = 13, // left/right distance to housing balljoint (distance between
					 // motor/housing pivot point)
			zc = 22.5, // 18.92,  // difference in height between motor/housing pivot
					   // point
			R = 30.88,     // radius of housing balljoint
			dist = 48.84; // front/back distance between motor/housing pivot point
	thetah += 0;
	//#endif

	thetah *= MOGI_PI / 180.;

	d = l * l - r * r - xh * xh - zc * zc
			- (dist - R * sin(thetah)) * (dist - R * sin(thetah))
			+ 2 * zc * R * cos(thetah) - R * R * cos(thetah) * cos(thetah);

	a = 4 * r * r
			* ((R * sin(thetah) - dist) * (R * sin(thetah) - dist) + xh * xh); // for sin
	b = 4 * r * d * (R * sin(thetah) - dist);
	c = d * d - 4 * r * r * xh * xh;

	if ((b * b - 4 * a * c) < 0) {
		if ((thetah * 180 / MOGI_PI) < 0) {
			theta = 75.6627;
		} else {
			theta = -90;
		}
	} else {
		theta =
				-asin((-b + sqrt(b * b - 4 * a * c)) / (2 * a)) * 180. / MOGI_PI;
	}

	return theta;
}
}
}

#ifdef _cplusplus
}
#endif
