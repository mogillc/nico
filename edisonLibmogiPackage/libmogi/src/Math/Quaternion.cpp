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

#ifdef UNIX
#include <iostream>
#else
#include <stdlib.h>
#endif

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

Quaternion::Quaternion() :
		Matrix(4, 1) {
	*data = 1;  // initialize with 0 rotation
}
;

Quaternion* Quaternion::create() {
#ifdef UNIX
	return new Quaternion;

#else
	Quaternion* result = (Quaternion*)malloc(sizeof(Quaternion));
	new (result) Quaternion;
	return result;
#endif
}

MogiDouble& Quaternion::operator()(const unsigned int& m_des) {
	return (*this)(m_des, 0);
	// return garbage;
}

int Quaternion::makeFromAngleAndAxis(MogiDouble angle, const Vector& axis) {
	MogiDouble omega, s;
	if (axis.size() != 3) {
#ifdef DEBUG_MATH
		printf(
				"Error! makeFromAngleAndAxis must only take an axis vector of size "
				"3.\n");
#endif
		return -1;
	}
	MogiDouble l = sqrt(axis * axis);

	if (l > 0) {
		omega = 0.5 * angle;
		s = sin(omega) / l;

		(*this)(0, 0) = (float) cos(omega);
		(*this)(1, 0) = (float) s * axis.valueAsConst(0, 0);
		(*this)(2, 0) = (float) s * axis.valueAsConst(1, 0);
		(*this)(3, 0) = (float) s * axis.valueAsConst(2, 0);
	} else {
		// perform no rotation:
		(*this)(0, 0) = 1;
		(*this)(1, 0) = 0;
		(*this)(2, 0) = 0;
		(*this)(3, 0) = 0;
	}
	return 0;
}

const Quaternion Quaternion::createFromAngleAndAxis(MogiDouble angle,
		const Vector& axis) {
	Quaternion result;
	result.makeFromAngleAndAxis(angle, axis);
	return result;
	//		if (axis.size() != 3) {
	//#ifdef DEBUG_MATH
	//			printf("Error! makeFromAngleAndAxis must only take an axis
	//vector of size 3.\n");
	//#endif
	//			return result;
	//		}
	//		MogiDouble omega, s;
	//		MogiDouble l = sqrt(axis*axis);
	//
	//		if (l > 0) {
	//			omega = 0.5 * angle;
	//			s = sin(omega) / l;
	//
	//			result.fastValue(0,0) = (float)cos(omega);
	//			result.fastValue(1,0) = (float)s * axis(0);
	//			result.fastValue(2,0) = (float)s * axis(1);
	//			result.fastValue(3,0) = (float)s * axis(2);
	//		} else {
	//			// perform no rotation:
	//			result.fastValue(0,0) = 1;
	//			result.fastValue(1,0) = 0;
	//			result.fastValue(2,0) = 0;
	//			result.fastValue(3,0) = 0;
	//		}
	//		return result;
}

const Matrix& Quaternion::makeRotationMatrix() const {
	// Matrix result(3, 3);
	(*scratch).setSize(3, 3);
	Quaternion me = *this;

	MogiDouble n = 1.0f
			/ sqrt(
					me(0, 0) * me(0, 0) + me(1, 0) * me(1, 0)
							+ me(2, 0) * me(2, 0) + me(3, 0) * me(3, 0));

	Matrix q = n * me;

	(*scratch)(0, 0) = 1 - 2 * q(2, 0) * q(2, 0) - 2 * q(3, 0) * q(3, 0);
	(*scratch)(1, 0) = 2 * q(1, 0) * q(2, 0) - 2 * q(0, 0) * q(3, 0);
	(*scratch)(2, 0) = 2 * q(1, 0) * q(3, 0) + 2 * q(0, 0) * q(2, 0);

	(*scratch)(0, 1) = 2 * q(1, 0) * q(2, 0) + 2 * q(0, 0) * q(3, 0);
	(*scratch)(1, 1) = 1 - 2 * q(1, 0) * q(1, 0) - 2 * q(3, 0) * q(3, 0);
	(*scratch)(2, 1) = 2 * q(2, 0) * q(3, 0) - 2 * q(0, 0) * q(1, 0);

	(*scratch)(0, 2) = 2 * q(1, 0) * q(3, 0) - 2 * q(0, 0) * q(2, 0);
	(*scratch)(1, 2) = 2 * q(2, 0) * q(3, 0) + 2 * q(0, 0) * q(1, 0);
	(*scratch)(2, 2) = 1 - 2 * q(1, 0) * q(1, 0) - 2 * q(2, 0) * q(2, 0);

	return (*scratch).transpose();
}

const Matrix& Quaternion::makeRotationMatrix4() const {
	// Matrix result(3, 3);
	//(*scratch).set_size(3, 3);
	Quaternion me = *this;
	(*scratch).makeI(4);

	MogiDouble n = 1.0f
			/ sqrt(
					me(0, 0) * me(0, 0) + me(1, 0) * me(1, 0)
							+ me(2, 0) * me(2, 0) + me(3, 0) * me(3, 0));

	Matrix q = n * me;

	(*scratch)(0, 0) = 1 - 2 * q(2, 0) * q(2, 0) - 2 * q(3, 0) * q(3, 0);
	(*scratch)(1, 0) = 2 * q(1, 0) * q(2, 0) - 2 * q(0, 0) * q(3, 0);
	(*scratch)(2, 0) = 2 * q(1, 0) * q(3, 0) + 2 * q(0, 0) * q(2, 0);

	(*scratch)(0, 1) = 2 * q(1, 0) * q(2, 0) + 2 * q(0, 0) * q(3, 0);
	(*scratch)(1, 1) = 1 - 2 * q(1, 0) * q(1, 0) - 2 * q(3, 0) * q(3, 0);
	(*scratch)(2, 1) = 2 * q(2, 0) * q(3, 0) - 2 * q(0, 0) * q(1, 0);

	(*scratch)(0, 2) = 2 * q(1, 0) * q(3, 0) - 2 * q(0, 0) * q(2, 0);
	(*scratch)(1, 2) = 2 * q(2, 0) * q(3, 0) + 2 * q(0, 0) * q(1, 0);
	(*scratch)(2, 2) = 1 - 2 * q(1, 0) * q(1, 0) - 2 * q(2, 0) * q(2, 0);

	return (*scratch).transpose();
}

Quaternion& Quaternion::operator*=(const Quaternion& param)  // A *= B (A = A*B)
		{
	Quaternion q1, q2, result;
	q1 = *this;
	q2 = param;
	result(0, 0) = q1(0, 0) * q2(0, 0) - q1(1, 0) * q2(1, 0) - q1(2, 0) * q2(2, 0) - q1(3, 0) * q2(3, 0);
	result(1, 0) = q1(0, 0) * q2(1, 0) + q1(1, 0) * q2(0, 0) + q1(2, 0) * q2(3, 0) - q1(3, 0) * q2(2, 0);
	result(2, 0) = q1(0, 0) * q2(2, 0) - q1(1, 0) * q2(3, 0) + q1(2, 0) * q2(0, 0) + q1(3, 0) * q2(1, 0);
	result(3, 0) = q1(0, 0) * q2(3, 0) + q1(1, 0) * q2(2, 0) - q1(2, 0) * q2(1, 0) + q1(3, 0) * q2(0, 0);

	*this = result;
	return *this;
}

//	void Quaternion::setupScratch()
//	{
//		if (scratch == NULL) {
//			Matrix::setupScratch(1);
//			return;
//#ifdef UNIX
//			scratch = new Matrix;
//#else
//			scratch = (Matrix *) malloc(sizeof(Matrix));
//			new(scratch)Matrix;
//#endif
//		}
//	}

const Quaternion& Quaternion::operator*(const Quaternion& param) const {
	Quaternion result;
	(*scratch) = *this; // this is messy, should be able to doMatrix result = *this
	(*(Quaternion*) scratch) *= param;
	return (*(Quaternion*) scratch);
}

//	Quaternion & Quaternion::operator= (const Matrix& param)
//	{
//		if ((param.numColumns() != 1) && (param.numRows() != 4)) {
//#ifdef DEBUG_MATH
//			printf("Error: matrix assignment to vector must have 1
//column, 4 rows, dimensionality of %s is (%d, %d), performing no =
//operation\n", param.label, param.numRows(), param.numColumns());
//#endif
//		} else {
//			//this->set_size( param.numRows() );
//			for (int i = 0; i < param.numRows(); i++) {
//				(*this)(i) = param.value(i, 0);
//			}
//		}
//
//		return *this;
//	}

double Quaternion::makeAngleAndAxis(Vector* result) {
	MogiDouble angle = acos((*this)(0, 0));
	MogiDouble sa = sin(angle);

	result->setLength(3);
	(*result)(0) = (*this)(1, 0) / sa;
	(*result)(1) = (*this)(2, 0) / sa;
	(*result)(2) = (*this)(3, 0) / sa;

	return angle * 2.0;
}

const Quaternion slerp(const Quaternion& v0, const Quaternion& v1,
		MogiDouble t) {
	// v0 and v1 should be unit length or else
	// something broken will happen.
	Quaternion result, qb;

	// Compute the cosine of the angle between the two vectors.
	MogiDouble cosHalfTheta = v0.dot(v1);

	if (cosHalfTheta < 0) {
		qb = -1 * v1;
		cosHalfTheta = -cosHalfTheta;
	} else {
		qb = v1;
	}

#define DOT_THRESHOLD 0.9999995
	if (cosHalfTheta > DOT_THRESHOLD) {
		// If the inputs are too close for comfort, linearly interpolate
		// and normalize the result.

		result = v0 + t * (qb - v0);
		result.normalize();
		return result;
	}

	cosHalfTheta = clamp(cosHalfTheta, -1, 1); // Robustness: Stay within domain of acos()
	MogiDouble halfTheta = acos(cosHalfTheta); // theta_0 = angle between input vectors
	MogiDouble sinHalfTheta = sqrt(1.0 - cosHalfTheta * cosHalfTheta);

	if (fabs(sinHalfTheta) < 0.001) {  // fabs is floating point absolute
		result = 0.5 * v0 + 0.5 * qb;
		result.normalize();
		return result;
	}

	MogiDouble ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
	MogiDouble ratioB = sin(t * halfTheta) / sinHalfTheta;

	result = ratioA * v0 + ratioB * qb;
	return result;
}

const Quaternion Quaternion::inverse() const {
	double magnitude = sqrt(
			data[0] * data[0] + data[1] * data[1] + data[2] * data[2]
					+ data[3] * data[3]);
	Quaternion result;
	result.value(0, 0) = data[0] / magnitude;
	result.value(1, 0) = -data[1] / magnitude;
	result.value(2, 0) = -data[2] / magnitude;
	result.value(3, 0) = -data[3] / magnitude;
	return result;
}

const Vector Quaternion::eulerAngles(void) {
	Vector result(3);

	result(2) = atan2(2.0 * ((*this)(1) * (*this)(2) + (*this)(0) * (*this)(3)),
			(*this)(0) * (*this)(0) + (*this)(1) * (*this)(1)
					- (*this)(2) * (*this)(2) - (*this)(3) * (*this)(3)); // + MOGI_PI;

	result(1) = asin(
			-2.0 * ((*this)(1) * (*this)(3) - (*this)(0) * (*this)(2)));

	result(0) = atan2(2.0 * ((*this)(2) * (*this)(3) + (*this)(0) * (*this)(1)),
			(*this)(0) * (*this)(0) - (*this)(1) * (*this)(1)
					- (*this)(2) * (*this)(2) + (*this)(3) * (*this)(3));

	return result;
}
}
}

#ifdef _cplusplus
}
#endif
