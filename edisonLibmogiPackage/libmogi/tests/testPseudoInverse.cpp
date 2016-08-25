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
#include <stdio.h>

using namespace Mogi::Math;

Matrix psuedoInverse(Matrix);

int main(int argc, char *argv[]) {
	//	// Matrices:
	//    Matrix A(5,2);
	//	Matrix y(5,1);
	//	Matrix x;
	//
	//	// Matrix Labels:
	//    A.name("A");
	//	x.name("x");
	//	y.name("y");
	//
	//	printf("Least squares solution for model y = mx + b:\n");
	//	for (int i = 0; i<5; i++) {
	//		A(i,0) = 1;
	//		A(i,1) = i-2;
	//	}
	//    y(0,0) = 0.1;
	//	y(1,0) = 1.05;
	//	y(2,0) = 1.8;
	//	y(3,0) = 3.2;
	//	y(4,0) = 4.02;
	//
	//  	A.print_stats();
	//	y.print_stats();
	//
	//	x = psuedoInverse(A)*y;
	//
	//	x.print_stats();
	//
	//	printf("Solution: m = %.2f, b = %.2f\n", x(1,0), x(0,0));
	//
	//
	//	for (int i = 0; i<5; i++) {
	//		A(i,0) = 1;
	//		A(i,1) = 1.6-1.34*i;
	//	}
	//	y(0,0) = 28.48;
	//	y(1,0) = 16.91;
	//	y(2,0) = 7.65;
	//	y(3,0) = 0.21;
	//	y(4,0) = -8.57;
	//
	//
	//	x = psuedoInverse(A)*y;
	//	A.print_stats();
	//	y.print_stats();
	//
	//	x = psuedoInverse(A)*y;
	//
	//	x.print_stats();
	//
	//	printf("Solution: m = %.2f, b = %.2f\n", x(1,0), x(0,0));

	return 0;
}

Matrix psuedoInverse(Matrix A) {
	Matrix temp;
	temp = A.transpose() * A;
	return temp.inverse() * A.transpose();
}
