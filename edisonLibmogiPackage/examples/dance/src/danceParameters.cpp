/**
 *  danceParameters.cpp - dance parameterization
 *
 *  Created by Matt Bunting on August 14th 2015
 *  Copyright 2015 Mogi LLC.  All rights reserved
 */

#include "danceParameters.h"

#include <fstream>
#include <string>
#include <iostream>
#include <map>

DanceParameters::DanceParameters() {
	doubleFistBumpBodyOffset.setLength(3);
	doubleFistBumpAngleOffset.setLength(3);
	waveHeadAngleOffset.setLength(3);
	waveHeadAngleMagnitude.setLength(3);
	waveHeadAnglePhase.setLength(3);
	waveHeadAngleFrequencySkew.setLength(3);
	cameraOrigin.setLength(3);
	waveBodyOffset.setLength(3);
	flyingCircleCenter.setLength(3);
	waveBumpCircleMagnitude.setLength(3);
	waveBumpCircleOffset.setLength(3);
	waveBumpAngleMagnitude.setLength(3);
	waveBumpAngleOffset.setLength(3);
}

void DanceParameters::setFromFile( std::string filePath ) {
	std::cout << "Loading dance parameter configuration file: " << filePath << std::endl;
	std::ifstream jsonConfigurationFile( filePath.c_str() );
	std::string jsonConfiguration((std::istreambuf_iterator<char>(jsonConfigurationFile)),
																std::istreambuf_iterator<char>());

	App::JsonValueInterface root;   // will contains the root value after parsing.
	//Json::Reader reader;

//	std::cout << "file contents:" << jsonConfiguration;

	bool parsingSuccessful = !App::JsonValueInterface::parse(jsonConfiguration, root);// reader.parse(jsonConfiguration, root);

	if (!parsingSuccessful) {
		std::cerr << " - Could not parse JSON from: " << filePath << std::endl;
		return;
	}

	std::map<std::string,double*> keytoDoubleMap;
	keytoDoubleMap["headingAcceleration"] = &headingAcceleration;
	keytoDoubleMap["headingMaxSpeed"] = &headingMaxSpeed;
	keytoDoubleMap["locationAcceleration"] = &locationAcceleration;
	keytoDoubleMap["locationMaxSpeed"] = &locationMaxSpeed;
	keytoDoubleMap["fistBumpCenterRadiusFromBody"] = &fistBumpCenterRadiusFromBody;
	keytoDoubleMap["fistBumpRadius"] = &fistBumpRadius;
	keytoDoubleMap["fistBumpHeight"] = &fistBumpHeight;
	keytoDoubleMap["doubleFistBumpRadius"] = &doubleFistBumpRadius;
	keytoDoubleMap["doubleFistBumpHeight"] = &doubleFistBumpHeight;
	keytoDoubleMap["legWaveBodyHeightOffset"] = &legWaveBodyHeightOffset;
	keytoDoubleMap["legWavefootLiftHeight"] = &legWavefootLiftHeight;
	keytoDoubleMap["walkingSpeedHorizontal"] = &walkingSpeedHorizontal;
	keytoDoubleMap["walkingSpeedVertical"] = &walkingSpeedVertical;
	keytoDoubleMap["walkingSpeedRotation"] = &walkingSpeedRotation;
	keytoDoubleMap["flyingCircleRadius"] = &flyingCircleRadius;
	keytoDoubleMap["flyingBodyLiftMagnitude"] = &flyingBodyLiftMagnitude;


	for (std::map<std::string,double*>::iterator it = keytoDoubleMap.begin(); it != keytoDoubleMap.end(); it++) {
		if( root[it->first].isDouble() ) {
			*(it->second) = root[it->first].asDouble();
		} else {
			std::cerr << " - Error, could not parse " << it->first << std::endl;
		}
	}

	std::map<std::string,Vector*> keytoVectorMap;
	keytoVectorMap["doubleFistBumpBodyOffset"] = &doubleFistBumpBodyOffset;
	keytoVectorMap["doubleFistBumpAngleOffset"] = &doubleFistBumpAngleOffset;
	keytoVectorMap["waveHeadAngleOffset"] = &waveHeadAngleOffset;
	keytoVectorMap["waveHeadAngleMagnitude"] = &waveHeadAngleMagnitude;
	keytoVectorMap["waveHeadAnglePhase"] = &waveHeadAnglePhase;
	keytoVectorMap["waveHeadAngleFrequencySkew"] = &waveHeadAngleFrequencySkew;
	keytoVectorMap["cameraOrigin"] = &cameraOrigin;
	keytoVectorMap["waveBodyOffset"] = &waveBodyOffset;
	keytoVectorMap["flyingCircleCenter"] = &flyingCircleCenter;
	keytoVectorMap["waveBumpCircleMagnitude"] = &waveBumpCircleMagnitude;
	keytoVectorMap["waveBumpCircleOffset"] = &waveBumpCircleOffset;
	keytoVectorMap["waveBumpAngleMagnitude"] = &waveBumpAngleMagnitude;
	keytoVectorMap["waveBumpAngleOffset"] = &waveBumpAngleOffset;

	for (std::map<std::string,Vector*>::iterator it = keytoVectorMap.begin(); it != keytoVectorMap.end(); it++) {
		if( root[it->first].isArray() &&
			 	root[it->first].size() == it->second->size() ) {
			for (int i = 0; i < it->second->size(); i++) {
				if(root[it->first][i].isDouble())
				{
    			it->second->value(i, 0) = root[it->first][i].asDouble();
				} else {
					std::cerr << " - Error, could not get double value at index " << i << " for " << it->first << std::endl;
				}
			}
		} else {
			std::cerr << " - Error, could not parse " << it->first << std::endl;
		}
	}

	std::cout << " - Done!" << std::endl;
}

void DanceParameters::setFromType( HEXAPOD_TYPE type ) {
	switch (type) {
		case HEXAPOD_TYPE_NICO:
			headingAcceleration = 3*MOGI_PI/180.0;
			headingMaxSpeed = 15*MOGI_PI/180.0;
			locationAcceleration = 10;
			locationMaxSpeed = 100;
			fistBumpCenterRadiusFromBody = 50;
			fistBumpRadius = 20;
			fistBumpHeight = 175;
			doubleFistBumpBodyOffset(0) = 0.0;
			doubleFistBumpBodyOffset(1) = -30.0;
			doubleFistBumpBodyOffset(2) = -5.0;
			doubleFistBumpAngleOffset(0) = 0;//-15.0 * MOGI_PI / 180;
			doubleFistBumpAngleOffset(1) = 0.0;
			doubleFistBumpAngleOffset(2) = 0.0;
			doubleFistBumpRadius = 30;
			doubleFistBumpHeight = 250;
			legWaveBodyHeightOffset = 20;
			legWavefootLiftHeight = 100;
			waveHeadAngleOffset(0) = (26.506 - 3.7601)/2.0 * MOGI_PI/180.0;
			waveHeadAngleOffset(1) = 0;
			waveHeadAngleOffset(2) = 0;
			waveHeadAngleMagnitude(0) = .5 *(26.506 - 3.7601) * MOGI_PI/180.0;
			waveHeadAngleMagnitude(1) = 0.9 * 20.0 * MOGI_PI/180.0;
			waveHeadAngleMagnitude(2) = .6 * 13.0 * MOGI_PI/180.0;
			waveHeadAnglePhase(0) = 0;
			waveHeadAnglePhase(1) = MOGI_PI/2;
			waveHeadAnglePhase(2) = 0;
			waveHeadAngleFrequencySkew(0) = 1;
			waveHeadAngleFrequencySkew(1) = 1;
			waveHeadAngleFrequencySkew(2) = 1.1;
			cameraOrigin(0) = 0.0;
			cameraOrigin(1) = 40;
			//camOrigin(1,0) = 133.48;
			cameraOrigin(2) = 23.6;
			waveBodyOffset(0) = 0;
			waveBodyOffset(1) = 0;
			waveBodyOffset(2) = 35;
			walkingSpeedHorizontal = 65;
			walkingSpeedVertical = 75;
			walkingSpeedRotation = 30.0*MOGI_PI/180.0;

			flyingCircleRadius = 20;
			flyingBodyLiftMagnitude = 10;
			flyingCircleCenter(0) = 50;
			flyingCircleCenter(1) = 0;
			flyingCircleCenter(2) = 75;
			break;

			case HEXAPOD_TYPE_ANOUK_CABLE:
		case HEXAPOD_TYPE_ANOUK:
			headingAcceleration = 3*MOGI_PI/180.0;
			headingMaxSpeed = 15*MOGI_PI/180.0;
			locationAcceleration = 10;
			locationMaxSpeed = 100;
			fistBumpCenterRadiusFromBody = 57.5;
			fistBumpRadius = 20;
			fistBumpHeight = 200;
			doubleFistBumpBodyOffset(0) = 0.0;
			doubleFistBumpBodyOffset(1) = -40.0;
			doubleFistBumpBodyOffset(2) = -15.0;
			doubleFistBumpAngleOffset(0) = 0.0;//-15.0 * MOGI_PI / 180;
			doubleFistBumpAngleOffset(1) = 0.0;
			doubleFistBumpAngleOffset(2) = 0.0;
			doubleFistBumpRadius = 30;
			doubleFistBumpHeight = 300;
			legWaveBodyHeightOffset = 20;
			legWavefootLiftHeight = 100;
			waveHeadAngleOffset(0) = (26.506 - 3.7601)/2.0 * MOGI_PI/180.0;
			waveHeadAngleOffset(1) = 0;
			waveHeadAngleOffset(2) = 0;
			waveHeadAngleMagnitude(0) = .5 *(26.506 - 3.7601) * MOGI_PI/180.0;
			waveHeadAngleMagnitude(1) = 0.9 * 20.0 * MOGI_PI/180.0;
			waveHeadAngleMagnitude(2) = .6 * 13.0 * MOGI_PI/180.0;
			waveHeadAnglePhase(0) = 0;
			waveHeadAnglePhase(1) = MOGI_PI/2;
			waveHeadAnglePhase(2) = 0;
			waveHeadAngleFrequencySkew(0) = 1;
			waveHeadAngleFrequencySkew(1) = 1;
			waveHeadAngleFrequencySkew(2) = 1.1;
			cameraOrigin(0) = 0.0;
			cameraOrigin(1) = 40;
			//camOrigin(1,0) = 133.48;
			cameraOrigin(2) = 23.6;
			waveBodyOffset(0) = 0;
			waveBodyOffset(1) = 0;
			waveBodyOffset(2) = 35;
			walkingSpeedHorizontal = 65;
			walkingSpeedVertical = 75;
			walkingSpeedRotation = 30.0*MOGI_PI/180.0;

			flyingCircleRadius = 20;
			flyingBodyLiftMagnitude = 10;
			flyingCircleCenter(0) = 50;
			flyingCircleCenter(1) = 0;
			flyingCircleCenter(2) = 75;

			break;

		case HEXAPOD_TYPE_DMITRI:
			headingAcceleration = 3*MOGI_PI/180.0;
			headingMaxSpeed = 15*MOGI_PI/180.0;
			locationAcceleration = 10;
			locationMaxSpeed = 100;
			fistBumpCenterRadiusFromBody = 150;
			fistBumpRadius = 40;
			fistBumpHeight = 320;
			doubleFistBumpBodyOffset(0) = 0.0;
			doubleFistBumpBodyOffset(1) = -70.0;
			doubleFistBumpBodyOffset(2) = 0.0;
			doubleFistBumpAngleOffset(0) = 0;//-15.0 * MOGI_PI / 180;
			doubleFistBumpAngleOffset(1) = 0.0;
			doubleFistBumpAngleOffset(2) = 0.0;
			doubleFistBumpRadius = 30;
			doubleFistBumpHeight = 450;
			legWaveBodyHeightOffset = 20;
			legWavefootLiftHeight = 100;
			waveHeadAngleOffset(0) = (26.506 - 3.7601)/2.0 * MOGI_PI/180.0;
			waveHeadAngleOffset(1) = 0;
			waveHeadAngleOffset(2) = 0;
			waveHeadAngleMagnitude(0) = .5 *(26.506 - 3.7601) * MOGI_PI/180.0;
			waveHeadAngleMagnitude(1) = 0.9 * 20.0 * MOGI_PI/180.0;
			waveHeadAngleMagnitude(2) = .6 * 13.0 * MOGI_PI/180.0;
			waveHeadAnglePhase(0) = 0;
			waveHeadAnglePhase(1) = MOGI_PI/2;
			waveHeadAnglePhase(2) = 0;
			waveHeadAngleFrequencySkew(0) = 1;
			waveHeadAngleFrequencySkew(1) = 1;
			waveHeadAngleFrequencySkew(2) = 1.1;
			cameraOrigin(0) = 0.0;
			//cameraOrigin(1) = 40;
			cameraOrigin(1) = 133.48;
			cameraOrigin(2) = 23.6;
			waveBodyOffset(0) = 0;
			waveBodyOffset(1) = 0;
			waveBodyOffset(2) = 35;
			walkingSpeedHorizontal = 65;
			walkingSpeedVertical = 75;
			walkingSpeedRotation = 30.0*MOGI_PI/180.0;
			flyingCircleRadius = 40;
			flyingBodyLiftMagnitude = 20;
			flyingCircleCenter(0) = 100;
			flyingCircleCenter(1) = 0;
			flyingCircleCenter(2) = 200;
			break;
	}
}
