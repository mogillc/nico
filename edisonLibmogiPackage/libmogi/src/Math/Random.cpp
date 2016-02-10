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

#include "systems.h"

#include <math.h>

#ifdef UNIX
#include <iostream>
#endif

#include <stdlib.h>

namespace Mogi {
namespace Math {

Random::Random() {
	srand(time(NULL));
}

double Random::uniform(double min, double max) {
	return (max - min) * ((double) rand() / (double) RAND_MAX) + min;
}

double Random::normal(double mean, double variance) {
	// from http://en.wikipedia.org/wiki/Box–Muller_transform
	static bool haveSpare = false;
	static double rand1, rand2;

	if (haveSpare) {
		haveSpare = false;
		return sqrt(variance * rand1) * sin(rand2) + mean;
	}

	haveSpare = true;

	rand1 = rand() / ((double) RAND_MAX);
	if (rand1 < 1e-100)
		rand1 = 1e-100;
	rand1 = -2 * log(rand1);
	rand2 = (rand() / ((double) RAND_MAX)) * 2 * MOGI_PI;

	return sqrt(variance * rand1) * cos(rand2) + mean;
}
}
}
