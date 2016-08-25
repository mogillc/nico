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
static const char* const GAIN_C_Id = "$Id$";
#endif

#include "mogi/math/systems.h"

#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Math;

	Gain::Gain()
	:gain(1), saturationMax(1), saturationMin(-1) {
	}

	Gain::~Gain() {
	}

	void Gain::setGain(const double& K) {
		gain = K;
	}

	const double& Gain::getGain() const {
		return gain;
	}

	void Gain::setSaturation(const double& min, const double& max) {
		if (min < max) {
			saturationMin = min;
			saturationMax = max;
		} else {
			saturationMax = min;
			saturationMin = max;
		}
	}

	double Gain::perform(const double& input, const double& dTime) {
		return clamp(gain*computeBase(input, dTime), saturationMin, saturationMax);
	}

	double GainP::computeBase(const double& input, const double& dTime) {
		return input;
	}

	GainI::GainI()
	:runningValue(0) {
	}

	double GainI::computeBase(const double& input, const double& dTime) {
		runningValue += (input * dTime);
		runningValue = clamp(runningValue, saturationMin/getGain(), saturationMax/getGain());
		return runningValue;
	}

	void GainI::reset(const double& initialValue) {
		runningValue = initialValue;
	}

	GainD::GainD(int averagerKernelSize) {
		averagerLength = averagerKernelSize > 2 ? averagerKernelSize : 2;
		priorValues = new double[averagerLength];
		priorTimes = new double[averagerLength];

		for(int i = 0; i < averagerLength; i++) {
			priorValues[i] = 0;
			priorTimes[i] = 0;
		}
	}

	GainD::~GainD() {
		delete [] priorValues;
		delete [] priorTimes;
	}

	double GainD::computeBase(const double& input, const double& dTime) {
		double actualTime = dTime;
		int i;
		for(i = 1; i < averagerLength; i++) {	// YES, we start at 1 here since the first value if between the first and prior point!
			actualTime += priorTimes[i];
		}
		double result = (input - priorValues[0])/actualTime;

		for(i = 1; i < averagerLength; i++) {
			priorValues[i-1] = priorValues[i];
			priorTimes[i-1] = priorTimes[i];
		}
		priorValues[averagerLength-1] = input;
		priorTimes[averagerLength-1] = dTime;

		return result;
	}

	GainPID::GainPID(int averagerKernelSize)
 	:blockD(averagerKernelSize) {
		blockI.setGain(0);
		blockD.setGain(0);
	}

	double GainPID::computeBase(const double& input, const double& dTime) {
		return blockP.perform(input,dTime) + blockI.perform(input,dTime) + blockD.perform(input,dTime);
	}

	GainP* GainPID::gainP() {
		return &blockP;
	}

	GainI* GainPID::gainI() {
		return &blockI;
	}

	GainD* GainPID::gainD() {
		return &blockD;
	}

#ifdef _cplusplus
}
#endif
