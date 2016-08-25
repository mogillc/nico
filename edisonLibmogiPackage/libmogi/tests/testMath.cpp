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

#include "math.h"
#include <math.h>
#include <stdio.h>   // printf
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h>
#include <iostream>
#include "systems.h"
#include "mogi/math/algorithm.h"

using namespace Mogi::Math;

bool testConstruction(bool verbose);
bool testLUDecomposition(bool verbose);
bool testPrimitives(bool verbose);
bool testVectors(bool verbose);
bool testGLK(bool verbose);
bool testTransformations(bool verbose);
bool testQuaternions(bool verbose);
bool testOtherCpp(bool verbose);
bool testLinearSystem();
bool testGrahamScan(bool verbose);

bool matricesEqual(Matrix *A, Matrix *B) {
	bool testPassed = true;

	if (A->numRows() != B->numRows()) {
		printf(" - %s rows != %s rows, %d != %d\n", A->getName(), B->getName(),
				A->numRows(), B->numRows());
		testPassed = false;
	}

	if (A->numColumns() != B->numColumns()) {
		printf(" - %s cols != %s cols, %d != %d\n", A->getName(), B->getName(),
				A->numColumns(), B->numColumns());
		testPassed = false;
	}

	if (testPassed)
		for (int i = 0; i < A->numRows(); i++) {
			for (int j = 0; j < A->numColumns(); j++) {
				if (fabs((*A)(i, j) - (*B)(i, j)) > 0.000001) {
					testPassed = false;
					printf(" - %s(%d,%d) != %s(%d,%d), %f != %f\n",
							A->getName(), i, j, B->getName(), i, j, (*A)(i, j),
							(*B)(i, j));
				}
			}
		}

	return testPassed;
}

int main(int argc, char *argv[]) {
	bool allTestsPass = true;
	bool verbose = false;
	// char *test;
	// test = new char[100];
	// delete [] test;

//	Vector *dummy = Vector::create();
//	Matrix *dummy2 = Vector::create();
//	delete dummy;
//	delete dummy2;
//
//	std::cout << "int         size: " << sizeof(int) << std::endl;
//	std::cout << "int*        size: " << sizeof(int*) << std::endl;
//	std::cout << "Matrix      size: " << sizeof(Matrix) << std::endl;
//	std::cout << "Vector      size: " << sizeof(Vector) << std::endl;
//	std::cout << "Quaternion  size: " << sizeof(Quaternion) << std::endl;
//	std::cout << "Bezier      size: " << sizeof(Bezier) << std::endl;
//	std::cout << "Matrix*     size: " << sizeof(Matrix *) << std::endl;
//	std::cout << "Vector*     size: " << sizeof(Vector *) << std::endl;
//	std::cout << "Quaternion* size: " << sizeof(Quaternion *) << std::endl;
//	std::cout << "Bezier*     size: " << sizeof(Bezier *) << std::endl;
//
//	Time timeTracker;
//	std::cout << "Performing 100 multiplications of 100x100 square matrices..."
//			<< std::endl;
//	// Matrices:
//	timeTracker.initialize();
//	for (int i = 0; i < 100; i++) {
//		Matrix *DeleteImmediate2 = new Matrix(100, 100);
//		(*DeleteImmediate2) = (*DeleteImmediate2) * (*DeleteImmediate2);
//		delete DeleteImmediate2;
//		timeTracker.update();
//	}
//	std::cout << " - Computed at " << timeTracker.frequencyAverage() << "Hz"
//			<< std::endl;
//
//	int numberOfAllocations = 1000000;
//	std::cout << "Performing " << numberOfAllocations
//			<< " allocations of 0x0 matrices..." << std::endl;
//	// Matrices:
//	timeTracker.initialize();
//	for (int i = 0; i < numberOfAllocations; i++) {
//		Matrix *DeleteImmediate2 = Matrix::create(); //new Matrix(4, 4);
//		delete DeleteImmediate2;
//		timeTracker.update();
//	}
//	std::cout << " - Computed at " << timeTracker.frequencyAverage() << "Hz"
//			<< std::endl;

	if (!testConstruction(verbose)) {
		allTestsPass = false;
	}

	if (!testPrimitives(verbose)) {
		allTestsPass = false;
	}

	if (!testVectors(verbose)) {
		allTestsPass = false;
	}

	if (!testLUDecomposition(verbose)) {
		allTestsPass = false;
	}

	if (!testTransformations(verbose)) {
		allTestsPass = false;
	}

	if (!testGLK(verbose)) {
		allTestsPass = false;
	}

	allTestsPass = testQuaternions(verbose) ? allTestsPass : false;
	allTestsPass = testOtherCpp(verbose) ? allTestsPass : false;
	std::cout << " - Testing linear Systems:" << std::endl;
	allTestsPass = testLinearSystem() ? allTestsPass : false;
	allTestsPass = testGrahamScan(verbose) ? allTestsPass : false;

	printf("\nSummary:\n");
	if (allTestsPass) {
		printf("\t- All tests succesful!\n\n");
	} else {
		printf("\t- Something appears to be be broken :(\n\n");
	}

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testConstruction(bool verbose) {
	bool allTestsPass = true;

	Matrix matrix(100, 100);
	matrix.name("matrix");
	for (int i = 0; i < matrix.numColumns(); i++) {
		for (int j = 0; j < matrix.numRows(); j++) {
			matrix(i, j) = (MogiDouble) i / (MogiDouble) j * 100 - 50;
		}
	}

	printf("Checking resize ......................... ");
	matrix.setSize(200, 200);
	Matrix correctResult(200, 200);
	if (matricesEqual(&matrix, &correctResult)) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking name ........................... ");
	if (strcmp("matrix", matrix.getName()) == 0) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking float conversion ............... ");
	matrix.setSize(2, 2);
	matrix(0, 0) = 1;
	matrix(0, 1) = 2;
	matrix(1, 0) = 3;
	matrix(1, 1) = 4;
	float *result = matrix.dataAsFloat();
	if (result[0] == 1 && result[1] == 3 && result[2] == 2 && result[3] == 4) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	Matrix* matrix2 = Matrix::create();
	printf("Checking bad index 1 .................... ");
	matrix2->setSize(2, 2);
	MogiDouble* ref = &(*matrix2)(-1, 0);
	if (ref == NULL) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking bad index 2 .................... ");
	matrix.setSize(2, 2);
	ref = &matrix(0, -1);
	if (ref == NULL) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking bad index 1 .................... ");
	ref = &matrix(3, 1);
	if (ref == NULL) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking bad index 4 .................... ");
	ref = &matrix(1, 3);
	if (ref == NULL) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testGLK(bool verbose) {
	bool allTestsPass = true;

	printf("Checking GLKMatrix4MakeOrtho ............ ");
	Matrix matrix = GLKMatrix4MakeOrtho(-3, 2, -1, 80, .1, 60);
	Matrix expected(4, 4);
	expected(0, 0) = 0.4;
	expected(1, 1) = 2.0 / 81.0;
	expected(2, 2) = -2 / 59.9;
	expected(3, 3) = 1.0;
	expected(0, 3) = 1.0 / 5.0;
	expected(1, 3) = -0.975309;
	expected(2, 3) = -1.003339;
	if (matricesEqual(&expected, &matrix)) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	printf("Checking GLKMatrix4MakePerspective ...... ");
	matrix = GLKMatrix4MakePerspective(.2, .3, 1.2, 200.3);
	expected = Matrix(4, 4);
	expected(0, 0) = 33.222148;
	expected(1, 1) = 9.966644;
	expected(2, 2) = -1.012054;
	expected(2, 3) = -2.414465;
	expected(3, 2) = -1.0;
	if (matricesEqual(&expected, &matrix)) {
		printf("Passed\n");
	} else {
		printf("FAILED\n");
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testVectors(bool verbose) {
	bool allTestsPass = true;

	Vector A;
	A.name("A");
	Vector B(10);
	// Vector C(39, 1);	// This should NOT compile

	for (int i = 0; i < 10; i++) {
		B(i) = i;
	}

	A = B;
	B.name("B");

	printf("Checking Vector B = A ................... ");
	if (!matricesEqual(&A, &B)) {
		printf("FAILED: Error!  \"B\" assignment failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	A.setLength(2);
	A(0) = 25;
	A(1) = MOGI_PI;
	double d = A.magnitude();

	printf("Checking Vector A.magnitude() ........... ");
	if (fabs(d - sqrt(625 + MOGI_PI * MOGI_PI)) > 0.000001) {
		printf("FAILED: Error!  Vector \"A\" vector magnitude() failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	Matrix C(1, 1);
	C.name("C");

	C(0, 0) = 2.0;
	A = A * C;  // Vector * Matrix

	// A.print_stats();

	C.setSize(5, 1);
	for (int i = 0; i < 5; i++) {
		C(i, 0) = i * 7;
	}
	A = C;  // Vector = Matrix

	printf("Checking Vector A = Matrix C ............ ");
	if ((A.size() != 5)) {
		printf("FAILED: Error!  Vector \"A\" = Matrix \"C\" failed!\n");
		allTestsPass = false;
	} else if (!matricesEqual(&A, &C)) {
		printf("FAILED: Error!  Vector \"A\" = Matrix \"C\" failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	A.setLength(5);
	for (int i = 0; i < 5; i++) {
		A(i, 0) = i * 7;
		A(i, 0) *= 0.5;
	}

	printf("Checking 2 * A .......................... ");
	B = 2 * A;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error! \"A\" * 2 failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking B * 0.5  ....................... ");
	B = B * 0.5;
	if (!matricesEqual(&B, &A)) {
		printf("FAILED: Error! 0.5 * \"B\" failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking A dot B  ....................... ");
	A.setLength(3);
	B.setLength(3);
	A(0) = 1;
	B(0) = 1;
	A(1) = -2;
	B(1) = 1;
	A(2) = 100;
	B(2) = -10;

	double answer = A * B;
	if (!(answer == -1001)) {
		printf("FAILED: Error! A dot B = answer != -1001, failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	// C.set_size(5, 2);
	// A = C;	// This should fail

	return allTestsPass;
}

bool testPrimitives(bool verbose) {
	bool allTestsPass = true;

	Matrix A(3, 3);
	A.name("A");

	A(0, 0) = 2;
	A(0, 1) = 1;
	A(0, 2) = 1;
	A(1, 0) = 4;
	A(1, 1) = 3;
	A(1, 2) = 3;
	A(2, 0) = 8;
	A(2, 1) = 7;
	A(2, 2) = 9;

	printf("Checking B = A .......................... ");
	Matrix B = A;
	B.name("B");
	if (!matricesEqual(&A, &B)) {
		printf("FAILED: Error!  \"B\" assignment failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C = A*2 ........................ ");
	Matrix C = A * 2;
	C.name("C");
	B(0, 0) = 4;
	B(0, 1) = 2;
	B(0, 2) = 2;
	B(1, 0) = 8;
	B(1, 1) = 6;
	B(1, 2) = 6;
	B(2, 0) = 16;
	B(2, 1) = 14;
	B(2, 2) = 18;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Scalar multiplication failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking B += 2 ......................... ");
	Matrix compare(3, 3);
	B(0, 0) = 4;
	B(0, 1) = 2;
	B(0, 2) = 2;
	B(1, 0) = 8;
	B(1, 1) = 6;
	B(1, 2) = 6;
	B(2, 0) = 16;
	B(2, 1) = 14;
	B(2, 2) = 18;
	B += 2;
	compare(0, 0) = 6;
	compare(0, 1) = 4;
	compare(0, 2) = 4;
	compare(1, 0) = 10;
	compare(1, 1) = 8;
	compare(1, 2) = 8;
	compare(2, 0) = 18;
	compare(2, 1) = 16;
	compare(2, 2) = 20;
	if (!matricesEqual(&B, &compare)) {
		printf("FAILED: Error!  addition failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking B += A ......................... ");
	B(0, 0) = 4;
	B(0, 1) = 2;
	B(0, 2) = 2;
	B(1, 0) = 8;
	B(1, 1) = 6;
	B(1, 2) = 6;
	B(2, 0) = 16;
	B(2, 1) = 14;
	B(2, 2) = 18;
	B += A;

	compare(0, 0) = 6;
	compare(0, 1) = 3;
	compare(0, 2) = 3;
	compare(1, 0) = 12;
	compare(1, 1) = 9;
	compare(1, 2) = 9;
	compare(2, 0) = 24;
	compare(2, 1) = 21;
	compare(2, 2) = 27;
	if (!matricesEqual(&B, &compare)) {
		printf("FAILED: Error!  Addition failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C = B/2 ........................ ");
	B(0, 0) = 4;
	B(0, 1) = 2;
	B(0, 2) = 2;
	B(1, 0) = 8;
	B(1, 1) = 6;
	B(1, 2) = 6;
	B(2, 0) = 16;
	B(2, 1) = 14;
	B(2, 2) = 18;
	C = B / 2;
	if (!matricesEqual(&C, &A)) {
		printf("FAILED: Error!  Scalar division failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C = A.T() ...................... ");
	C = A.transpose();
	B(0, 0) = 2;
	B(0, 1) = 4;
	B(0, 2) = 8;
	B(1, 0) = 1;
	B(1, 1) = 3;
	B(1, 2) = 7;
	B(2, 0) = 1;
	B(2, 1) = 3;
	B(2, 2) = 9;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix transpose failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C = A.Inv() .................... ");
	C = A.inverse();
	B(0, 0) = 1.5;
	B(0, 1) = -.5;
	B(0, 2) = 0;
	B(1, 0) = -3;
	B(1, 1) = 2.5;
	B(1, 2) = -.5;
	B(2, 0) = 1;
	B(2, 1) = -1.5;
	B(2, 2) = .5;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix inverse failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking I == A.Inv()*A ................. ");
	C = C * A;
	B(0, 0) = 1;
	B(0, 1) = 0;
	B(0, 2) = 0;
	B(1, 0) = 0;
	B(1, 1) = 1;
	B(1, 2) = 0;
	B(2, 0) = 0;
	B(2, 1) = 0;
	B(2, 2) = 1;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix multiplication failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C ^= 3 ......................... ");
	C(0, 0) = 1;
	C(0, 1) = 2;
	C(0, 2) = 3;
	C(1, 0) = -1;
	C(1, 1) = 1;
	C(1, 2) = 2;
	C(2, 0) = 1;
	C(2, 1) = 2;
	C(2, 2) = -10;
	B(0, 0) = -31;
	B(0, 1) = -32;
	B(0, 2) = 256;
	B(1, 0) = -24;
	B(1, 1) = -39;
	B(1, 2) = 216;
	B(2, 0) = 112;
	B(2, 1) = 176;
	B(2, 2) = -1135;
	C ^= 3;
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix multiplication failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C ^= -3 ........................ ");
	C(0, 0) = 1;
	C(0, 1) = 2;
	C(0, 2) = 3;
	C(1, 0) = -1;
	C(1, 1) = 1;
	C(1, 2) = 2;
	C(2, 0) = 1;
	C(2, 1) = 2;
	C(2, 2) = -10;
	A = C ^ -3;
	C(0, 0) = 1;
	C(0, 1) = 2;
	C(0, 2) = 3;
	C(1, 0) = -1;
	C(1, 1) = 1;
	C(1, 2) = 2;
	C(2, 0) = 1;
	C(2, 1) = 2;
	C(2, 2) = -10;
	C ^= 3;
	C *= A;
	B.makeI(3);
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix multiplication failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking C ^= 0 ......................... ");
	C(0, 0) = 1;
	C(0, 1) = 2;
	C(0, 2) = 3;
	C(1, 0) = -1;
	C(1, 1) = 1;
	C(1, 2) = 2;
	C(2, 0) = 1;
	C(2, 1) = 2;
	C(2, 2) = -10;
	C ^= 0;
	B.makeI(3);
	if (!matricesEqual(&B, &C)) {
		printf("FAILED: Error!  Matrix multiplication failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking det 1x1 ........................ ");
	B.setSize(1, 1);
	B.value(0, 0) = 0.1;
	if (B.determinant() != 0.1) {
		printf("FAILED\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	return allTestsPass;
}

bool testLUDecomposition(bool verbose) {
	bool allTestsPass = true;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Now going to perform LU decomposition.
	// This example was derived from
	// http://www.math.kent.edu/~reichel/courses/intr.num.comp.1/fall09/lecture9/lecture4.pdf
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (verbose) {
		printf(
				"// Now going to perform LU decomposition.\n// This example was "
						"derived from "
						"http://www.math.kent.edu/~reichel/courses/intr.num.comp.1/fall09/"
						"lecture9/lecture4.pdf\n");
	}

	Matrix A(4, 4);
	A.name("A");

	A(0, 0) = 2;
	A(0, 1) = 1;
	A(0, 2) = 1;
	A(0, 3) = 0;
	A(1, 0) = 4;
	A(1, 1) = 3;
	A(1, 2) = 3;
	A(1, 3) = 1;
	A(2, 0) = 8;
	A(2, 1) = 7;
	A(2, 2) = 9;
	A(2, 3) = 5;
	A(3, 0) = 6;
	A(3, 1) = 7;
	A(3, 2) = 9;
	A(3, 3) = 8;

	LUdecomposer decomposer;

	// A.performLUDecomp();
	decomposer.perform(A);
	Matrix L = decomposer.L, Lc(4, 4);
	Matrix U = decomposer.U, Uc(4, 4);
	Matrix P = decomposer.P, Pc(4, 4);
	L.name("L");
	U.name("U");
	P.name("P");

	if (verbose) {
		A.print();
		U.print();
		L.print();
		P.print();
	}

	// This is the solutiont to LU decomposition:
	Lc(0, 0) = 1;
	Lc(0, 1) = 0;
	Lc(0, 2) = 0;
	Lc(0, 3) = 0;
	Lc(1, 0) = 3.0 / 4.0;
	Lc(1, 1) = 1;
	Lc(1, 2) = 0;
	Lc(1, 3) = 0;
	Lc(2, 0) = 1.0 / 2.0;
	Lc(2, 1) = -2.0 / 7.0;
	Lc(2, 2) = 1;
	Lc(2, 3) = 0;
	Lc(3, 0) = 1.0 / 4.0;
	Lc(3, 1) = -3.0 / 7.0;
	Lc(3, 2) = 1.0 / 3.0;
	Lc(3, 3) = 1;

	Uc(0, 0) = 8;
	Uc(0, 1) = 7.0;
	Uc(0, 2) = 9.0;
	Uc(0, 3) = 5.0;
	Uc(1, 0) = 0;
	Uc(1, 1) = 7.0 / 4.0;
	Uc(1, 2) = 9.0 / 4.0;
	Uc(1, 3) = 17.0 / 4.0;
	Uc(2, 0) = 0;
	Uc(2, 1) = 0;
	Uc(2, 2) = -6.0 / 7.0;
	Uc(2, 3) = -2.0 / 7.0;
	Uc(3, 0) = 0;
	Uc(3, 1) = 0;
	Uc(3, 2) = 0;
	Uc(3, 3) = 2.0 / 3.0;

	Pc(0, 0) = 0;
	Pc(0, 1) = 0;
	Pc(0, 2) = 1;
	Pc(0, 3) = 0;
	Pc(1, 0) = 0;
	Pc(1, 1) = 0;
	Pc(1, 2) = 0;
	Pc(1, 3) = 1;
	Pc(2, 0) = 0;
	Pc(2, 1) = 1;
	Pc(2, 2) = 0;
	Pc(2, 3) = 0;
	Pc(3, 0) = 1;
	Pc(3, 1) = 0;
	Pc(3, 2) = 0;
	Pc(3, 3) = 0;

	printf("Checking LU decomposition ............... ");
	bool testPassed = true;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (fabs(Pc(i, j) - P(i, j)) > 0.000001) {
				testPassed = false;
				printf(" - Pc(%d,%d) != P(%d,%d), %f != %f\n", i, j, i, j,
						Pc(i, j), P(i, j));
			}
			if (fabs(Lc(i, j) - L(i, j)) > 0.000001) {
				testPassed = false;
				printf(" - Lc(%d,%d) != L(%d,%d), %f != %f\n", i, j, i, j,
						Lc(i, j), L(i, j));
			}
			if (fabs(Uc(i, j) - U(i, j)) > 0.000001) {
				testPassed = false;
				printf(" - Uc(%d,%d) != U(%d,%d), %f != %f\n", i, j, i, j,
						Uc(i, j), U(i, j));
			}
		}
	}
	if (testPassed == false) {
		printf("Error!  LU decomposition failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking A = P^-1 * L * U composition ... ");
	Matrix Ac = P.inverse() * L * U;
	/*
	 testPassed = true;
	 for (int i = 0; i < 4; i++) {
	 for (int j = 0 ; j < 4; j++) {
	 if (fabs(Ac(i,j) - A(i,j)) > 0.000001) {
	 testPassed = false;
	 printf(" - Ac(%d,%d) != A(%d,%d), %f != %f\n", i, j, i, j, Ac(i,j), A(i,j));
	 }
	 }
	 }
	 if (testPassed == false) {
	 */
	if (!matricesEqual(&Ac, &A)) {
		printf("FAILED: Error!  \"A\" composition failed!\n");
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}
	return allTestsPass;
}

bool testTransformations(bool verbose) {
	bool allTestsPass = true;

	Matrix R, Rscratch, S, T, transform;
	Matrix x, y, z;
	x.makeXRotation(.2);
	y.makeYRotation(.1234);
	z.makeZRotation(-826);
	Rscratch = x * y * z;
	R.makeI(4);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			R(i, j) = Rscratch(i, j);
		}
	}
	S.makeI(4);
	S(0, 0) = 65.7613;
	S(1, 1) = .231457;
	S(2, 2) = -3.518491;

	T = GLKMatrix4MakeTranslation(57.143243, 83.8234, -200.718972);
//	T.makeI(4);
//	T(0, 3) = 57.143243;
//	T(1, 3) = 83.8234;
//	T(2, 3) = -200.718972;

	transform = T * R * S;

	Matrix Rout, Sout, Tout;
	printf("Checking transformation decomposition ... ");
	transformationToScaleRotationTranslation(&transform, &Sout, &Rout, &Tout);
	printf("Passed\n");

	Rout.name("Rout");
	R.name("R");
	Sout.name("Sout");
	S.name("S");
	Tout.name("Tout");
	T.name("T");

	printf("Checking transform decomposition for R... ");
	if (!matricesEqual(&Rout, &R)) {
		printf(
				"FAILED: Error! transformationToScaleRotationTranslation() Rotation "
						"failed!\n");
		R.print();
		Rout.print();
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking transform decomposition for S... ");
	if (!matricesEqual(&Sout, &S)) {
		printf(
				"FAILED: Error! transformationToScaleRotationTranslation() Scale "
						"failed!\n");
		S.print();
		Sout.print();
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking transform decomposition for T... ");
	if (!matricesEqual(&Tout, &T)) {
		printf(
				"FAILED: Error! transformationToScaleRotationTranslation() Translation "
						"failed!\n");
		T.print();
		Tout.print();
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	if (!allTestsPass) {
		printf("Here is the transformati0on matrix computed from T*R*S");
		transform.name("transform");
		transform.print();
	}

	printf("Checking bad transform .................. ");
	transform.setSize(3, 4);
	if (transformationToScaleRotationTranslation(&transform, &Sout, &Rout,
			&Tout) != -1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking NULL transform ................. ");
	if (transformationToScaleRotationTranslation(NULL, &Sout, &Rout, &Tout)
			!= -1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking bad transform 2 ................ ");
	transform.setSize(4, 4);
	if (transformationToScaleRotationTranslation(&transform, &Sout, &Rout,
			&Tout) != -1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	printf("Checking NULL Sout ...................... ");
	transform = T * R * S;
	if (transformationToScaleRotationTranslation(&transform, NULL, &Rout, &Tout)
			== -1 || !matricesEqual(&Tout, &T)) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		printf("Passed\n");
	}

	return allTestsPass;
}

bool testQuaternions(bool verbose) {
	bool allTestsPass = true;

	std::cout << " - Testing quaternions methods:" << std::endl;

	std::cout << "Checking Q1 = Q2 ........................ ";
	Quaternion Q1, Q2;
	Q1.name("Q1");
	Q2.name("Q2");
	Q1(0) = .707;
	Q1(1) = -.707;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2 = Q1;
	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	// Correct answers from
	// http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer
	std::cout << "Checking makeFromAngleAndAxis bad axis .. ";
	Vector axis(1);
	int result = Q1.makeFromAngleAndAxis(0, axis);
	if (result == -1) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis zero axis . ";
	axis.setLength(3);
	result = Q1.makeFromAngleAndAxis(0, axis);
	Quaternion Q3;
	if (result == 0 && matricesEqual(&Q1, &Q3)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis x axis .... ";
	axis(0) = 1;
	result = Q1.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2(0) = 0;
	Q2(1) = 1;
	Q2(2) = 0;
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis x axis 2 .. ";
	axis(0) = 1;
	result = Q1.makeFromAngleAndAxis(MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(0.5);
	Q2(1) = sqrt(0.5);
	Q2(2) = 0;
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis x axis 3 .. ";
	axis(0) = 1;
	result = Q1.makeFromAngleAndAxis(-MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(0.5);
	Q2(1) = -sqrt(0.5);
	Q2(2) = 0;
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis y axis .... ";
	axis(0) = 0;
	axis(1) = 1;
	axis(2) = 0;
	result = Q1.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2(0) = 0;
	Q2(1) = 0;
	Q2(2) = 1;
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis y axis 2 .. ";
	axis(0) = 0;
	axis(1) = 1;
	axis(2) = 0;
	result = Q1.makeFromAngleAndAxis(MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(.5);
	Q2(1) = 0;
	Q2(2) = sqrt(.5);
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis y axis 3 .. ";
	axis(0) = 0;
	axis(1) = -1;
	axis(2) = 0;
	result = Q1.makeFromAngleAndAxis(MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(.5);
	Q2(1) = 0;
	Q2(2) = -sqrt(.5);
	Q2(3) = 0;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis z axis .... ";
	axis(0) = 0;
	axis(1) = 0;
	axis(2) = 1;
	result = Q1.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2(0) = 0;
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = 1;
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis z axis 2 .. ";
	axis(0) = 0;
	axis(1) = 0;
	axis(2) = 1;
	result = Q1.makeFromAngleAndAxis(MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(.5);
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = sqrt(.5);
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeFromAngleAndAxis z axis 3 .. ";
	axis(0) = 0;
	axis(1) = 0;
	axis(2) = 1;
	result = Q1.makeFromAngleAndAxis(-MOGI_PI / 2.0, axis);
	Q2(0) = sqrt(.5);
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = -sqrt(.5);
	if (result == 0 && matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis x axis ........ ";
	Vector axisCorrect(3);
	axisCorrect(0) = 1;
	axisCorrect(1) = 0;
	axisCorrect(2) = 0;
	Q1(0) = sqrt(0.5);
	Q1(1) = sqrt(0.5);
	Q1(2) = 0;
	Q1(3) = 0;
	double angle = Q1.makeAngleAndAxis(&axis);
	//	std::cout << "angle: " << angle;
	//	axis.name("name");
	//	axis.print_stats();
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis x axis 2 ...... ";
	axisCorrect(0) = -1;
	axisCorrect(1) = 0;
	axisCorrect(2) = 0;
	Q1(0) = sqrt(0.5);
	Q1(1) = -sqrt(0.5);
	Q1(2) = 0;
	Q1(3) = 0;
	angle = Q1.makeAngleAndAxis(&axis);
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis y axis ........ ";
	axisCorrect(0) = 0;
	axisCorrect(1) = 1;
	axisCorrect(2) = 0;
	Q1(0) = sqrt(0.5);
	Q1(1) = 0;
	Q1(2) = sqrt(0.5);
	Q1(3) = 0;
	angle = Q1.makeAngleAndAxis(&axis);
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis y axis 2 ...... ";
	axisCorrect(0) = 0;
	axisCorrect(1) = -1;
	axisCorrect(2) = 0;
	Q1(0) = sqrt(0.5);
	Q1(1) = 0;
	Q1(2) = -sqrt(0.5);
	Q1(3) = 0;
	angle = Q1.makeAngleAndAxis(&axis);
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis z axis ........ ";
	axisCorrect(0) = 0;
	axisCorrect(1) = 0;
	axisCorrect(2) = 1;
	Q1(0) = sqrt(0.5);
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = sqrt(0.5);
	angle = Q1.makeAngleAndAxis(&axis);
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking makeAngleAndAxis z axis 2 ...... ";
	axisCorrect(0) = 0;
	axisCorrect(1) = 0;
	axisCorrect(2) = -1;
	Q1(0) = sqrt(0.5);
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = -sqrt(0.5);
	angle = Q1.makeAngleAndAxis(&axis);
	if (fabs(angle - MOGI_PI / 2.0) < 0.00000001
			&& matricesEqual(&axisCorrect, &axis)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking inverse 1 ...................... ";
	Q1(0) = sqrt(0.5);
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = -sqrt(0.5);
	Q2 = Q1.inverse();
	Q1 = Q2 * Q1;
	Q2 = Quaternion();
	if (matricesEqual(&Q2, &Q1)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking inverse 1 ...................... ";
	Q1(0) = sqrt(0.5);
	Q1(1) = -1;
	Q1(2) = -sqrt(0.5);
	Q1(3) = 1;
	Q1.normalize();
	Q2 = Q1.inverse();
	Q1 = Q1 * Q2;
	Q2 = Quaternion();
	if (matricesEqual(&Q2, &Q1)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp .......................... ";
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 0;
	Q2(1) = 1;
	Q2(2) = 0;
	Q2(3) = 0;
	Q3 = slerp(Q1, Q2, 0.5);
	Quaternion Q4;
	Q4(0) = sqrt(0.5);
	Q4(1) = sqrt(0.5);
	Q4(2) = 0;
	Q4(3) = 0;
	Q4.name("Q4");
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 2 ........................ ";
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 0;
	Q2(1) = 0;
	Q2(2) = 1;
	Q2(3) = 0;
	Q3 = slerp(Q1, Q2, 0.5);
	Q4(0) = sqrt(0.5);
	Q4(1) = 0;
	Q4(2) = sqrt(0.5);
	Q4(3) = 0;
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 3 ........................ ";
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 0;
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = 1;
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = sqrt(0.5);
	Q4(1) = 0;
	Q4(2) = 0;
	Q4(3) = sqrt(0.5);
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 4 ........................ ";
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 0;
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = -1;
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = sqrt(0.5);
	Q4(1) = 0;
	Q4(2) = 0;
	Q4(3) = -sqrt(0.5);
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 5 ........................ "; // almost a corner case
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 0.9999;
	Q2(1) = 0.0001;
	Q2(2) = 0;
	Q2(3) = 0;
	Q2.normalize();
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = .99999999;
	Q4(1) = .00005;
	Q4(2) = 0;
	Q4(3) = 0;
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 6 ........................ ";  // a corner case
	Q1(0) = 1;
	Q1(1) = 0;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = 1;
	Q2(1) = 0;
	Q2(2) = 0;
	Q2(3) = 0;
	Q2.normalize();
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = 1;
	Q4(1) = 0;
	Q4(2) = 0;
	Q4(3) = 0;
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 7 ........................ ";  // a corner case
	Q1(0) = 0;
	Q1(1) = -1;
	Q1(2) = 0;
	Q1(3) = 0;
	Q2(0) = sqrt(0.5);
	Q2(1) = sqrt(0.5);
	Q2(2) = 0;
	Q2(3) = 0;
	Q2.normalize();
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = 0.382683;
	Q4(1) = 0.923880;
	Q4(2) = 0;
	Q4(3) = 0;
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking slerp 8 ........................ ";  // a corner case
	Q1(0) = 0.9999995;
	Q1(1) = 0;
	Q1(2) = sqrt(1 - 0.9999995 * 0.9999995);
	Q1(3) = 0;
	Q2(0) = 0.9999996;
	Q2(1) = sqrt(1 - 0.9999996 * 0.9999996);
	Q2(2) = 0;
	Q2(3) = 0;
	Q2.normalize();
	Q3 = slerp(Q2, Q1, 0.5);
	Q4(0) = 1.0;
	Q4(1) = 0.000447;
	Q4(2) = 0.000500;
	Q4(3) = 0;
	if (matricesEqual(&Q3, &Q4)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking Matrix.quaternion() 1 .......... ";
	Matrix rotationMatrix;
	rotationMatrix.makeI(3);
	axis(0) = 0;
	axis(1) = -sqrt(0.5);
	axis(2) = sqrt(0.5);
	Q1 = rotationMatrix.rotationToQuaternion();
	Q2.makeFromAngleAndAxis(0, axis);
	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking Matrix.quaternion() 2 .......... ";
	rotationMatrix.makeI(4);
	angle = MOGI_PI / 2.0;
	rotationMatrix.makeXRotation(MOGI_PI / 2.0);
	axis(0) = 1;
	axis(1) = 0;
	axis(2) = 0;
	Q1 = rotationMatrix.rotationToQuaternion();
	Q2.makeFromAngleAndAxis(MOGI_PI / 2.0, axis);
	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking Matrix.quaternion() 3 .......... "; // corner case, trace=0
	Matrix rotationMatrix2;
	rotationMatrix.makeI(4);
	rotationMatrix.makeXRotation(MOGI_PI);
	rotationMatrix2.makeYRotation(MOGI_PI);
	rotationMatrix = rotationMatrix * rotationMatrix2;
	Q1 = rotationMatrix.rotationToQuaternion();
	axis(0) = 1;
	axis(1) = 0;
	axis(2) = 0;
	Q2.makeFromAngleAndAxis(MOGI_PI, axis);
	axis(0) = 0;
	axis(1) = 1;
	axis(2) = 0;
	Q3.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2 = Q2 * Q3;

	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking Matrix.quaternion() 4 .......... "; // corner case, trace=0
	rotationMatrix.makeYRotation(MOGI_PI);
	rotationMatrix2.makeZRotation(MOGI_PI);
	rotationMatrix = rotationMatrix * rotationMatrix2;
	Q1 = rotationMatrix.rotationToQuaternion();
	axis(0) = 0;
	axis(1) = 1;
	axis(2) = 0;
	Q2.makeFromAngleAndAxis(MOGI_PI, axis);
	axis(0) = 0;
	axis(1) = 0;
	axis(2) = 1;
	Q3.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2 = Q2 * Q3;

	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking Matrix.quaternion() 5 .......... "; // corner case, trace=0
	rotationMatrix.makeZRotation(MOGI_PI);
	rotationMatrix2.makeXRotation(MOGI_PI);
	rotationMatrix = rotationMatrix * rotationMatrix2;
	Q1 = rotationMatrix.rotationToQuaternion();
	axis(0) = 0;
	axis(1) = 0;
	axis(2) = 1;
	Q2.makeFromAngleAndAxis(MOGI_PI, axis);
	axis(0) = 1;
	axis(1) = 0;
	axis(2) = 0;
	Q3.makeFromAngleAndAxis(MOGI_PI, axis);
	Q2 = Q2 * Q3;

	if (matricesEqual(&Q1, &Q2)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking eulerAnglesFromQuaternion() 1 .. ";
	Vector eulerExpected(3);
	eulerExpected(0) = 0.1;
	eulerExpected(1) = 0.2;
	eulerExpected(2) = 0.15;
	Q1.makeFromAngleAndAxis(eulerExpected(0), Vector::xAxis);
	Q2.makeFromAngleAndAxis(eulerExpected(1), Vector::yAxis);
	Q3.makeFromAngleAndAxis(eulerExpected(2), Vector::zAxis);

	Q1 = Q3 * Q2 * Q1;
	Vector euler = Q1.eulerAngles();
	if (matricesEqual(&euler, &eulerExpected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking eulerAnglesFromQuaternion() 2 .. ";
	eulerExpected(0) = 0.11;
	eulerExpected(1) = -0.2;
	eulerExpected(2) = -0.05;
	Q1.makeFromAngleAndAxis(eulerExpected(0), Vector::xAxis);
	Q2.makeFromAngleAndAxis(eulerExpected(1), Vector::yAxis);
	Q3.makeFromAngleAndAxis(eulerExpected(2), Vector::zAxis);

	Q1 = Q3 * Q2 * Q1;
	euler = Q1.eulerAngles();
	if (matricesEqual(&euler, &eulerExpected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking eulerAnglesFromQuaternion() 2 .. ";
	eulerExpected(0) = 3;
	eulerExpected(1) = 1.5;
	eulerExpected(2) = 2.9;
	Q1.makeFromAngleAndAxis(eulerExpected(0), Vector::xAxis);
	Q2.makeFromAngleAndAxis(eulerExpected(1), Vector::yAxis);
	Q3.makeFromAngleAndAxis(eulerExpected(2), Vector::zAxis);

	Q1 = Q3 * Q2 * Q1;
	euler = Q1.eulerAngles();
	if (matricesEqual(&euler, &eulerExpected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testOtherCpp(bool verbose) {
	bool allTestsPass = true;

	std::cout << " - testing MBother.cpp" << std::endl;

	std::cout << "Checking tri ............................ ";
	if (fabs(tri(MOGI_PI) - 0.0) < 0.0000001
			&& fabs(tri(-MOGI_PI) - 0.0) < 0.0000001
			&& fabs(tri(MOGI_PI / 2.0) - 1.0) < 0.0000001
			&& fabs(tri(-MOGI_PI / 2.0) - -1.0) < 0.0000001
			&& fabs(tri(MOGI_PI / 4.0) - 0.5) < 0.0000001
			&& fabs(tri(MOGI_PI / 2.0 + MOGI_PI / 4.0) - 0.5) < 0.0000001) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking sqr ............................ ";
	if (sqr(0.0001) == 1 && sqr(MOGI_PI / 2 - 0.0001) == 1
			&& sqr(MOGI_PI / 2 + 0.0001) == -1
			&& sqr(3 * MOGI_PI / 2 - 0.0001) == -1
			&& sqr(3 * MOGI_PI / 2 + 0.0001) == 1
			&& sqr(5 * MOGI_PI / 2 - 0.0001) == 1
			&& sqr(5 * MOGI_PI / 2 + 0.0001) == -1
			&& sqr(-3 * MOGI_PI / 2 - 0.0001) == -1
			&& sqr(-3 * MOGI_PI / 2 + 0.0001) == 1) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking clamp .......................... ";
	if (clamp(7, -10, 91) == 7 && clamp(-1000, -30, .90) == -30
			&& clamp(1000, -.54, -.25) == -.25) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	// from http://www.mathsisfun.com/algebra/trig-cosine-law.html
	std::cout << "Checking lawCos ......................... ";
	if (fabs(lawCos(5, 9, 8) - acosf(42.0 / 90.0)) < 0.000001) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	// from http://www.mathsisfun.com/algebra/trig-sine-law.html
	std::cout << "Checking lawSin ......................... ";
	if (fabs(lawSin(5.5, 63 * MOGI_PI / 180.0, 4.7) - 49.6 * MOGI_PI / 180.0)
			< 0.001) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking relativeToGlobal ............... ";
	Vector result, resultExpected(3);
	Quaternion rootOrientation;
	Vector rootLocation(3);
	rootLocation(0) = 100;
	rootLocation(1) = 150;
	rootLocation(2) = 200;
	rootOrientation.makeFromAngleAndAxis(MOGI_PI / 2.0, Vector::yAxis);
	Vector relativeLocation(3);
	relativeLocation(0) = 1;

	result = relativeToGlobal(rootOrientation, rootLocation, relativeLocation);
	resultExpected(0) = 100;
	resultExpected(1) = 150;
	resultExpected(2) = 199;

	if (matricesEqual(&result, &resultExpected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testLinearSystem() {
	bool allTestsPass = true;

	Matrix A(3, 3), b(3, 1);

	// Example from here: http://www.purplemath.com/modules/systlin7.htm
	A(0, 0) = 2;
	A(0, 1) = 1;
	A(0, 2) = 3;
	b(0, 0) = 1;
	A(1, 0) = 2;
	A(1, 1) = 6;
	A(1, 2) = 8;
	b(1, 0) = 3;
	A(2, 0) = 6;
	A(2, 1) = 8;
	A(2, 2) = 18;
	b(2, 0) = 5;

	std::cout << "Checking fastLinearSystemSolve .......... ";
	LinearSystemSolver solver;

	Matrix x = solver.perform(A, b);
	Matrix xActual(3, 1);
	xActual(0, 0) = 3.0 / 10.0;
	xActual(1, 0) = 2.0 / 5.0;
	xActual(2, 0) = 0;
	if (matricesEqual(&xActual, &x)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	// from: http://www.mathsisfun.com/algebra/systems-linear-equations.html
	A(0, 0) = 1;
	A(0, 1) = 1;
	A(0, 2) = 1;
	b(0, 0) = 6;
	A(1, 0) = 0;
	A(1, 1) = 2;
	A(1, 2) = 5;
	b(1, 0) = -4;
	A(2, 0) = 2;
	A(2, 1) = 5;
	A(2, 2) = -1;
	b(2, 0) = 27;

	std::cout << "Checking fastLinearSystemSolve 2 ........ ";
	x = solver.perform(A, b);
	xActual(0, 0) = 5;
	xActual(1, 0) = 3;
	xActual(2, 0) = -2;
	if (matricesEqual(&xActual, &x)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testGrahamScan(bool verbose) {
	bool allTestsPass = true;

	std::cout << " - Testing Graham Scan:" << std::endl;

	std::cout << "Checking GrahamScan::ccw 1 .............. ";
	Vector P1(2), P2(2), P3(2);
	//	   |P1
	// P3  |
	//----------
	//     |  P2
	//	   |
	P1(0) = 1;
	P1(1) = 2;
	P2(0) = 2;
	P2(1) = -1;
	P3(0) = -2;
	P3(1) = 1;

	if (GrahamScan::ccw(&P1, &P2, &P3) < 0) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking GrahamScan::ccw 2 .............. ";
	if (GrahamScan::ccw(&P2, &P1, &P3) > 0) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking GrahamScan::ccw 3 .............. ";
	P3(0) = 4;
	P3(1) = -7;
	if (GrahamScan::ccw(&P1, &P2, &P3) == 0) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	GrahamScan sorter;
	std::vector<Vector*> vectorsToSort;
	Vector P4(2), P5(2), P6(2), P7(2);
	vectorsToSort.push_back(&P1);
	vectorsToSort.push_back(&P2);
	vectorsToSort.push_back(&P3);
	vectorsToSort.push_back(&P4);
	vectorsToSort.push_back(&P5);
	vectorsToSort.push_back(&P6);
	vectorsToSort.push_back(&P7);

	//	   |P1
	// P3  |P5
	//-----P7----
	//     P6 P2
	// P4  |

	P1.name("P1");
	P2.name("P2");
	P3.name("P3");
	P4.name("P4");
	P5.name("P5");
	P6.name("P6");

	P1(0) = 1;
	P1(1) = 2;
	P2(0) = 2;
	P2(1) = -1;
	P3(0) = -2;
	P3(1) = 1;
	P4(0) = -2;
	P4(1) = -2;
	P5(0) = 1;
	P5(1) = 1;
	P6(0) = 0;
	P6(1) = -1;
	P7(0) = 0;
	P7(1) = 0;

	sorter.setInput(vectorsToSort);
	std::vector<Vector*> result = sorter.getConvexHull();

	std::cout << "Checking GrahamScan::perform ............ ";
	// Basically any points inside the hull should not be a part of the result.
	bool noPointsInHull = true;
	for (std::vector<Vector*>::iterator it = result.begin(); it != result.end();
			it++) {
		if ((*it) == &P5 || (*it) == &P6) {
			noPointsInHull = false;
		}
	}
	if (noPointsInHull) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	//	 P1|P3
	// P6  |  P4
	//-----P8----
	//     |
	//  P7 |P5 P2

	Vector P8(2);
	vectorsToSort.push_back(&P8);

	P1(0) = -1;
	P1(1) = 2;
	P2(0) = 2;
	P2(1) = -2;
	P3(0) = 1;
	P3(1) = 2;
	P4(0) = 1.5;
	P4(1) = 1.5;
	P5(0) = 1;
	P5(1) = -2;
	P6(0) = -2;
	P6(1) = 1;
	P7(0) = -1.5;
	P7(1) = -2;
	P8(0) = 0;
	P8(1) = 0;

	std::cout << "Checking GrahamScan::perform 2 .......... ";
	result = sorter.getConvexHull();
	noPointsInHull = true;
	for (std::vector<Vector*>::iterator it = result.begin(); it != result.end();
			it++) {
		if ((*it) == &P8) {
			noPointsInHull = false;
		}
	}
	if (noPointsInHull) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking GrahamScan::inHull 1 ........... ";
	if (sorter.inHull(P8)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << "Checking GrahamScan::inHull 2 ........... ";
	P8(1) = -10;
	if (!sorter.inHull(P8)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}
