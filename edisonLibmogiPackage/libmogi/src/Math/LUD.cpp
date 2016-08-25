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

#include "mmath.h"

#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

void LUdecomposer::perform(const Matrix& input) {
	unsigned int m = input.numColumns();
	if (m != input.numRows()) {
		return;
	}

	U = input;
	L.makeI(m);
	P.makeI(m);

	for (unsigned int i = 0; i < m; i++) {  // for all Columns
		unsigned int max_j = i;
		for (unsigned int j = i; j < m; j++) {  // for all ROWS/subcolumns
			if (fabs(U(j, i)) > fabs(U(max_j, i))) {
				max_j = j;
			}
		}

		// for (int j = i; j < m; j++) {	// for all ROWS/subcolumns
		if (max_j != i) {
			for (unsigned int k = 0; k < m; k++) {  // for all columns
				MogiDouble temp = P(max_j, k);
				P(max_j, k) = P(i, k);
				P(i, k) = temp;

				temp = U(max_j, k);
				U(max_j, k) = U(i, k);
				U(i, k) = temp;
			}
			// also need to swap all rows under the diagonal:
			for (unsigned int l = 0; l < i; l++) {  // for all columns
				MogiDouble temp = L(max_j, l);
				L(max_j, l) = L(i, l);
				L(i, l) = temp;
			}
		}
		//}
		// Now subtract the required amounts in each row in L and U, after THIS
		// ROW:
		for (unsigned int j = (i + 1); j < m; j++) { // repeat over all sub rows
			float factor = U(j, i) / U(i, i); // row value/cuurent diagonal value
			for (unsigned int k = i; k < m; k++) {  // for columns
				U(j, k) -= factor * U(i, k); // Current value minus factor times ith row above value
			}
			L(j, i) = factor;  // same for L
		}
	}

}

const Matrix& LinearSystemSolver::perform(const Matrix& A, const Matrix& b) {
	unsigned int length = b.numRows();
	unsigned int m = A.numRows();
	if (m != A.numColumns() || m != length || b.numColumns() != 1) {
		LUx *= 0;
		return LUx;
	}

	luDecomposer.perform(A);

	LUy.setSize(length, 1);
	// Matrix bp = Pmat*b;
	LUPb = luDecomposer.P * b;

	LUy.value(0, 0) = LUPb.value(0, 0) / luDecomposer.L.value(0, 0);
	for (unsigned int i = 1; i < length; i++) { // for all values of y (rows of L):
		MogiDouble temp = 0;
		for (unsigned int j = 0; j < i; j++) { // for all preceding columns of L (rows of y)
			temp += luDecomposer.L.value(i, j) * LUy.value(j, 0);
		}
		// solution of y value:
		LUy.value(i, 0) = (LUPb.value(i, 0) - temp)
				/ luDecomposer.L.value(i, i);
	}

	// End with back substitution to solve Ux=y
	LUx.setSize(length, 1);
	LUx.value(length - 1, 0) = LUy.value(length - 1, 0)
			/ luDecomposer.U.value(length - 1, length - 1);
	for (int i = (int) length - 2; i >= 0; i--) { // for all values of y (rows of L):
		MogiDouble temp = 0;
		for (unsigned int j = length - 1; j > i; j--) { // for all preceding columns of L (rows of y)
			temp += luDecomposer.U.value((unsigned int) i, j) * LUx(j, 0);
		}
		// solution of x value:
		LUx.value((unsigned int) i, 0) = (LUy.value((unsigned int) i, 0) - temp)
				/ luDecomposer.U.value((unsigned int) i, (unsigned int) i);
	}

	return LUx;
}

}
}

#ifdef _cplusplus
}
#endif
