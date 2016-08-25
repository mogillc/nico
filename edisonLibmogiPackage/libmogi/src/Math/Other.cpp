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

#include "mogi/math/mmath.h"

#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

float tri(float x) {
	float y;

	x += MOGI_PI / 2.0;
	if (x < 0)
		x = -x;

	y = 2.0 * fmodf(x, 2.0 * MOGI_PI) / MOGI_PI;
	if (y > 2.0) {
		y = 4 - y;
	}

	return (y - 1.0);
}

float sqr(float x) {
	float y;

	x += MOGI_PI / 2.0;
	if (x < 0)
		x = -x;

	y = fmodf(x, 2.0 * MOGI_PI);
	if (y > MOGI_PI) {
		y = -1;
	} else {
		y = 1;
	}

	return y;
}

float clamp(float x, float min, float max) {
	if (x < min) {
		return min;
	} else if (x > max) {
		return max;
	}
	return x;
}

float lawCos(float side1, float side2, float sideOpp) {
	return acosf(
			(square(side1) + square(side2) - square(sideOpp))
					/ (2 * side1 * side2));
}

float lawSin(float sideThetaOpp, float theta, float sideOpp) {
	return asinf(sideOpp * (sinf(theta) / sideThetaOpp));
}

const Vector relativeToGlobal(const Quaternion& rootOrientation,
		const Vector& rootLocation, const Vector& relativeLocation) {
	Vector result;

	result = rootLocation
			+ rootOrientation.makeRotationMatrix() * relativeLocation;

	return result;
}
}
}

#ifdef _cplusplus
}
#endif
