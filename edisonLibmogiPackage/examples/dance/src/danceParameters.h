/**
 *  dancePArameters.h - Header file for including dance parameters
 *
 *  Created by Matt Bunting on August 14th 2015
 *  Copyright 2015 Mogi LLC.  All rights reerved
 */

#include <mogi/robot/hexapod.h>

using namespace Mogi;
using namespace Math;

typedef enum {
	HEXAPOD_TYPE_DMITRI,
	HEXAPOD_TYPE_NICO,
	HEXAPOD_TYPE_ANOUK,
	HEXAPOD_TYPE_ANOUK_CABLE
} HEXAPOD_TYPE;


class DanceParameters {	// Bunting: would be nice to load this from a file instead or use the expression parsing
public:

	void setFromType( HEXAPOD_TYPE type );

	void setFromFile( std::string filePath );

	DanceParameters();

	double headingMaxSpeed;
	double headingAcceleration;
	double locationMaxSpeed;
	double locationAcceleration;
	double fistBumpCenterRadiusFromBody;
	double fistBumpRadius;
	double fistBumpHeight;
	Vector doubleFistBumpBodyOffset;
	Vector doubleFistBumpAngleOffset;
	double doubleFistBumpRadius;
	double doubleFistBumpHeight;
	double legWaveBodyHeightOffset;
	double legWavefootLiftHeight;
	Vector waveHeadAngleOffset;
	Vector waveHeadAngleMagnitude;
	Vector waveHeadAnglePhase;
	Vector waveHeadAngleFrequencySkew;
	Vector cameraOrigin;
	Vector waveBodyOffset;
	double walkingSpeedHorizontal;
	double walkingSpeedVertical;
	double walkingSpeedRotation;
	double flyingCircleRadius;
	double flyingBodyLiftMagnitude;
	Vector flyingCircleCenter;
	Vector waveBumpCircleMagnitude;
	Vector waveBumpCircleOffset;
	Vector waveBumpAngleMagnitude;
	Vector waveBumpAngleOffset;
	
};
