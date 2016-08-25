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
static const char* const Algorithm_C_Id = "$Id$";
#endif

#include "algorithm.h"
#include <algorithm>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {
// Possible TODO: check colinear cases:
double GrahamScan::ccw(const Vector* p1, const Vector* p2, const Vector* p3) {
	return (p2->valueAsConst(0, 0) - p1->valueAsConst(0, 0))
			* (p3->valueAsConst(1, 0) - p1->valueAsConst(1, 0))
			- (p2->valueAsConst(1, 0) - p1->valueAsConst(1, 0))
					* (p3->valueAsConst(0, 0) - p1->valueAsConst(0, 0));
}

Vector* v0;
bool compare(const void *p1, const void *p2) {
	Vector* v1 = (Vector*) p1;
	Vector* v2 = (Vector*) p2;

	return GrahamScan::ccw(v0, v1, v2) >= 0 ? true : false;

}

void GrahamScan::setInput(std::vector<Vector*>& input) {
	this->input = &input;
}

// Algorithm implemented from https://en.wikipedia.org/wiki/Graham_scan
std::vector<Vector*>& GrahamScan::getConvexHull() {
	int N = input->size();
	result = *input;

	// Find bottom most point in y, place at the beginning.
	for (std::vector<Vector*>::iterator it = (result.begin() + 1);
			it != result.end(); it++) {
		if ((*it)->valueAsConst(1, 0) < (*result.begin())->valueAsConst(1, 0)) {
			std::swap(*it, *result.begin());
		}
	}
	// The lowest point:
	v0 = (*result.begin());

	// Order the rest of the points in ascending angular order with respect to the lowest:
	std::sort(result.begin() + 1, result.end(), compare);

	// Stopping criteria for below algorithm
	result.insert(result.begin(), *result.rbegin());

	size_t M = 1;
	for (size_t i = 2; i < N; i++) {
		while (ccw(result[M - 1], result[M], result[i]) <= 0) {
			if (M > 1) {
				M -= 1;
			} else if (i == N) { // Corner case: all points colinear
				break;
			} else {
				i++;
			}
		}
		M++;
		std::swap(result[M], result[i]);
	}
	result.erase(result.begin() + (int) M + 1, result.end());

	return result;
}

bool GrahamScan::inHull(Vector& p) {
	bool insideHull = true;
	if (result.size() < 2) {
		return false;
	}
	for (std::vector<Vector*>::iterator it = result.begin();
			it != result.end() - 1; it++) {
		if (ccw(*it, *(it + 1), &p) < 0) {
			insideHull = false;
		}
	}
	return insideHull;
}
}
}

#ifdef _cplusplus
}
#endif
