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

#include "mogi/math/mmath.h"
//#define DEBUG_MATH
#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Mogi::Math;

//	Bezier::Bezier() {
//
//		initialized = false;
//	}

Bezier::Bezier(unsigned int order) {
	setDegree(order);
}

//	void Bezier::initialize() {
//		degree = 4;
//
//		createM();
//		createT();
//		initialized = true;
//	}

Bezier::~Bezier() {
	//		if (initialized) {
	//
	//		}
}

void Bezier::setDegree(unsigned int order) {
	// initialized = true;
	degree = order;

	createM();
	createT();
}

void Bezier::createM() {
	if (degree == 4) {
		M.setSize(4, 4);
		M(0, 0) = 1;
		M(0, 1) = -3;
		M(0, 2) = 3;
		M(0, 3) = -1;
		M(1, 0) = 0;
		M(1, 1) = 3;
		M(1, 2) = -6;
		M(1, 3) = 3;
		M(2, 0) = 0;
		M(2, 1) = 0;
		M(2, 2) = 3;
		M(2, 3) = -3;
		M(3, 0) = 0;
		M(3, 1) = 0;
		M(3, 2) = 0;
		M(3, 3) = 1;
	} else if (degree == 3) {
		M.setSize(3, 3);
		M(0, 0) = 1;
		M(0, 1) = -2;
		M(0, 2) = 1;
		M(1, 0) = 0;
		M(1, 1) = 2;
		M(1, 2) = -2;
		M(2, 0) = 0;
		M(2, 1) = 0;
		M(2, 2) = 1;
	} else if (degree == 2) {
		M.setSize(2, 2);
		M(0, 0) = 1;
		M(0, 1) = -1;
		M(1, 0) = 0;
		M(1, 1) = 1;
	}
	//		else {
	//			initialized = false;
	//		}
}

void Bezier::createT() {
	if ((degree > 1) && (degree <= 4)) {
		T.setSize(degree, 1);
		T(0, 0) = 1;
	}
}

void Bezier::setT(MogiDouble t) {
	unsigned int i;
	// T(0,0) = 1;
	if (t < 0)
		t = 0;
	if (t > 1)
		t = 1;

	for (i = 1; i < degree; i++) {
		T(i, 0) = T(i - 1, 0) * t;
	}
}

const Matrix& Bezier::compute(MogiDouble t) {
	// const Matrix ret;
	//		if (initialized) {
	setT(t);
	return P * M * T;
	//		}
	//		return P;
}

//	const Matrix Bezier::getP( int choose ) {
//		Vector ret(P.numRows());
//		for (int i = 0; i < P.numRows(); i++) {
//			ret(i) = P(i, choose);
//		}
//		return ret;
//	}

void Bezier::setControlPoint(const Matrix& p, unsigned int choose) {
	//		if (!initialized) {
	//			initialize();
	//		}
	if (choose < degree) {
		// if (((p.numRows() == 3) || (p.numRows() == 4)) && (p.numCols() == 1)) {
		if (p.numColumns() == 1) {
			if (P.numRows() != p.numRows()) {
				P.setSize(p.numRows(), degree);
			}

			for (unsigned int i = 0; i < p.numRows(); i++) {
				P(i, choose) = p.valueAsConst(i, 0);
			}
		}
	}
}

void Bezier::setStartPoint(const Matrix& p) {
	setControlPoint(p, 0);
}

void Bezier::setFinalPoint(const Matrix& p) {
	setControlPoint(p, degree - 1);
}

#ifdef _cplusplus
}
#endif
