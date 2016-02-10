/*
 *  calibration.h
 *
 *
 *  Created by Matt Bunting on 10/18/12.
 *  Copyright 2012 12 Cent Dwarf. All rights reserved.
 *
 */

#ifndef CALIBRATION_H
#define CALIBRATION_H

using namespace Mogi;
using namespace Math;

const Vector calibrateArm2( Vector& angles )
{
	Vector ret(6);
	angles *= 180.0/MOGI_PI;

	ret(0) = (( angles(0) )*1.0  + (300.0/2)  - 22.5) *1023.0/300.0;      // RX-64
	ret(1) = (( angles(1) )*1.0  + (250.92/2) + 90   ) *4095.0/250.92;          // EX-106+
	ret(2) = ((-angles(2) )*1.0  + (280.6/2) + 90 + 22.5 + 3.0 ) *4095.0/280.6; // EX-106
	ret(3) = (( angles(3) )*1.0  + (300.0/2) + 22.5) *1023.0/300.0;                    // RX-28
	ret(4) = (( angles(4) )*1.08 + (300.0/2) - 2.0) *1023.0/300.0;                    // RX-28
	ret(5) = ((-angles(5) )*1.08 + (300.0/2) - 3) *1023.0/300.0;      // RX-28

	return ret;
}

const Matrix linkConfigForArm2( )
{
	Matrix linkConfig(3,7);
	//linkConfig.set_size(3,7);

	// Joint 1 config, give the location:
	linkConfig(0,0) = 0;
	linkConfig(1,0) = 0;
	//linkConfig[0](2,0) = RX_64_HINGE_WIDTH/2.0 + EX_106_HORN_TO_IDLER/2.0 + EX_106_HINGE_RADIUS_BUNTING;
	linkConfig(2,0) = RX_64_ANGLE_BRACKET_BUNTING + RX_64_HORN_TO_IDLER/2.0;// + EX_106_HINGE_RADIUS_BUNTING;

	// Joint 2 config,
	linkConfig(2,1) = EX_106_HINGE_RADIUS_BUNTING;

	// everything below is incremented by one:

	// Joint 3 config,
	linkConfig(0,2) = EX_106_DOUBLE_MATE;//93.03;


	// Joint 4 config,
	linkConfig(0,3) = RX_28_HORN_TO_IDLER/2.0;//65.43;//68.5;//36;
	linkConfig(2,3) = EX_106_HINGE_RADIUS_BUNTING + RX_28_SIDE_OFFSET + RX_28_SIDE_HOLE_OFFSET;//50.7;//55.1;//41.92;

	// Joint 5 config,
	linkConfig(0,4) = RX_28_COLUMN_BUNTING + RX_28_SIDE_OFFSET + RX_28_LONG_HOLE_OFFSET;//68.06;

	// Joint 6 config,
	linkConfig(0,5) = RX_28_HINGE_OFFSET + RX_28_SIDE_OFFSET + RX_28_LONG_HOLE_OFFSET;//66.80;

	// Joint 7 config,
	linkConfig(0,6) = RX_28_HINGE_OFFSET + END_EFFECTOR_CC;//74.26;

	return linkConfig;
}

const Matrix axisConfigForArm2( )
{
	Matrix axisConfig(3,6);

	axisConfig(0, 0) = 0;	axisConfig(1, 0) = 0;	axisConfig(2, 0) = 1;
	axisConfig(0, 1) = 0;	axisConfig(1, 1) = 1;	axisConfig(2, 1) = 0;
	axisConfig(0, 2) = 0;	axisConfig(1, 2) = 1;	axisConfig(2, 2) = 0;
	axisConfig(0, 3) = 1;	axisConfig(1, 3) = 0;	axisConfig(2, 3) = 0;
	axisConfig(0, 4) = 0;	axisConfig(1, 4) = 0;	axisConfig(2, 4) = 1;
	axisConfig(0, 5) = 0;	axisConfig(1, 5) = 1;	axisConfig(2, 5) = 0;

	return axisConfig;
}

const Vector limitMotors( const Vector& input)
{
	Vector limited = input;
	return limited;

	float max[6] = {150, 0, 180, 150, 97, 97};
	float min[6] = {-150, -180, 0, -150, -97, -97};
	float center[6] = {0, -90, 90, 0, 0, 0};

	for (int i = 0; i < 6; i++) {
		max[i] *= MOGI_PI/180.0;
		min[i] *= MOGI_PI/180.0;
		center[i] *= MOGI_PI/180.0;

	if (limited(i) > max[i]) {
		limited(i) = max[i];
	} else if(limited(i) < min[i]){
		limited(i) = min[i];
	} else if(limited(i) != limited(i)){
		limited(i) = 0;
	}
	}

	return limited;
}


const Matrix linkConfigForArm2OLD( )
{
	Matrix linkConfig(3,6);
	//linkConfig.set_size(3,7);

	// Joint 1 config, give the location:
	linkConfig(0,0) = 0;
	linkConfig(1,0) = 0;
	//linkConfig[0](2,0) = RX_64_HINGE_WIDTH/2.0 + EX_106_HORN_TO_IDLER/2.0 + EX_106_HINGE_RADIUS_BUNTING;
	linkConfig(2,0) = RX_64_ANGLE_BRACKET_BUNTING + RX_64_HORN_TO_IDLER/2.0 + EX_106_HINGE_RADIUS_BUNTING;

	// Joint 2 config,
	//linkConfig(2,1) = EX_106_HINGE_RADIUS_BUNTING;

	// everything below is incremented by one:

	// Joint 3 config,
	linkConfig(0,1) = EX_106_DOUBLE_MATE;//93.03;


	// Joint 4 config,
	linkConfig(0,2) = RX_28_HORN_TO_IDLER/2.0;//65.43;//68.5;//36;
	linkConfig(2,2) = EX_106_HINGE_RADIUS_BUNTING + RX_28_SIDE_OFFSET + RX_28_SIDE_HOLE_OFFSET;//50.7;//55.1;//41.92;

	// Joint 5 config,
	linkConfig(0,3) = RX_28_COLUMN_BUNTING + RX_28_SIDE_OFFSET + RX_28_LONG_HOLE_OFFSET;//68.06;

	// Joint 6 config,
	linkConfig(0,4) = RX_28_HINGE_OFFSET + RX_28_SIDE_OFFSET + RX_28_LONG_HOLE_OFFSET;//66.80;

	// Joint 7 config,
	linkConfig(0,5) = RX_28_HINGE_OFFSET + END_EFFECTOR_CC;//74.26;

	return linkConfig;
}



#endif
