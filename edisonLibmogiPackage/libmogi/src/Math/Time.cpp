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
static const char* const Time_C_Id = "$Id$";
#endif
#include "systems.h"

#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

Time::Time() {
	favg = 0;
	fnum = 0;
}

void Time::initialize() {
	// tint = 0;
	gettimeofday(&tv, NULL); // Grab the current time (simply an initialization)
	timecycle = tv.tv_sec + tv.tv_usec * 1e-6; // Compute the current time, in seconds
	runningtime = 0;
	// usleep(10000);
}

void Time::update() {
	gettimeofday(&tv, NULL);   // Grab the current time
	oldtimecycle = timecycle;  // Store the old time
	timecycle = tv.tv_sec + tv.tv_usec * 1e-6; // Compute the current time, in seconds
//	double dtimeTest = timecycle - oldtimecycle;
//	if (dtimeTest < maxDTime) { // The difference in time, most times, should be
//								// less than this.
//		dtime = dtimeTest; // Find the time difference.  This is used throughout
//						   // the main loop, for velocities and temporal filters
//	}
	dtime = timecycle - oldtimecycle;

	if (favg == 0) {
		favg = 1 / dtime;
	} else {
		favg = 1. / (((1. / favg) * fnum + dtime) / (fnum + 1)); // Compute the running average of the looop frequency
	}
	fnum++;
	runningtime += dtime;

	for (int i = 9; i > 0; i--) {
		dTimeBuffer[i] = dTimeBuffer[i - 1];
	}
	dTimeBuffer[0] = dtime;

	float currentFrequency = 0;
	float w = 5;
	for (int i = 0; i < 10; i++) {
		// currentFrequency += ((9.5 - (double)i)/10.0)/(5) * dtimeBuffer[i];
		currentFrequency += ((w / 2.0 - (w / 9.0) * (double) i + 5) / 50)
				* dTimeBuffer[i];
	}
	currentFrequency = 1.0 / (currentFrequency);

	float fpsAlpha;
	fpsAlpha = expf(-dtime / 0.4);
	fpsLPF = fpsLPF * fpsAlpha + (1 - fpsAlpha) * currentFrequency;
	fps = fpsLPF;  // 1.0/dtimeLPF;
}

void Time::reset() {
//	gettimeofday(&tv, NULL);  // Grab the current time
//	timecycle = tv.tv_sec + tv.tv_usec * 1e-6; // Compute the current time, in seconds
	initialize();
}

double Time::runningTime() {
	return runningtime;
}
;

double Time::dTime() {
	return dtime;
}
;

double Time::frequencyAverage() {
	return favg;
}
;

double Time::frequency() {
	return fps;
}
}
}

#ifdef _cplusplus
}
#endif
