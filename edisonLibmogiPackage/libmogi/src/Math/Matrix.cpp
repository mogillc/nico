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
//#define DEBUG_MATRICES
#include <iostream>
#endif

#include <stdlib.h>
#include <string.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

// Matrix* Matrix::scratch = NULL;
// int Matrix::numScratch = 0;
// MogiDouble Matrix::garbage = -1;

//	int Matrix::numScratch = 0;
//	Matrix* Matrix::scratch = NULL;
//	MogiDouble Matrix::garbage = -1;
//	int Matrix::currScratch = 0;

Matrix* Matrix::create() {
#ifdef UNIX
	return new Matrix;
#else
	Matrix* result = (Matrix*)malloc(sizeof(Matrix));
	new (result) Matrix;
	return result;
#endif
}

const unsigned int& Matrix::numColumns() const {
	return _n;
}

const unsigned int& Matrix::numRows() const {
	return _m;
}

MogiDouble Matrix::valueAsConst(const unsigned int& row,
		const unsigned int& column) const {
	return *(data + row + _m * column);
}

MogiDouble Matrix::valueLinearIndex(unsigned int i) const {
	return *(data + i);
}

/*//////////////////////////////////*/
/*        -Funny Overloaded-        */
/*//////////////////////////////////*/
const Matrix& operator*(MogiDouble c, const Matrix& param) {
	return param * c;
}

/*//////////////////////////////////*/
/*       -Basic Initializers-       */
/*//////////////////////////////////*/
void Matrix::setSize(unsigned int new_m, unsigned int new_n) {
	// if (_s != (new_m*new_n))
	//{
	_m = new_m;  // set the newly assigned size
	_n = new_n;  // set the newly assigned size
	_s = _m * _n;

#ifdef UNIX
	//			if(_v != NULL || 1) {
	//				delete [] _v;
	//				delete [] _u;
	// free(_v);
	// free(_u);
	data = (MogiDouble*) realloc(data, _s * sizeof(MogiDouble));
	_u = (float*) realloc(_u, _s * sizeof(float));
//			} else {
////			_v = new MogiDouble[_s];  // now reallocate with new
///size
////			_u = new float[_s];
//				_v = (MogiDouble*) malloc(_s *
//sizeof(MogiDouble));
//				_u = (float*) malloc(_s * sizeof(float));
//			}

#else
	//			if(_v != NULL) {
	// free(_v);//delete [] v;
	data = (MogiDouble*)realloc(data, _s * sizeof(MogiDouble));
// free(u);//delete [] u;
//			} else {
//				_v = (MogiDouble *)malloc(sizeof(MogiDouble)*_s);//new
//(nothrow) MogiDouble[s];  // now reallocate with new size
//			}
// u = (float *)malloc(sizeof(float)*s);//new (nothrow) float[s];
#endif

	//////// THIS WAS REMOVED TO TEST BRANCH OVERAGE //////////
	//			if(_v == NULL) {
	//#ifdef DEBUG_MATRICES
	//				std::cout << "Error: could not allocate
	//memory\n";
	//#endif
	//				exit(EXIT_FAILURE);
	//			}

	// if (_s > 0) {
	setupScratch(1);
	//}
	//}
	//		for(int i = 0; i < _s; i++)
	//		{
	//			_v[i] = 0;  // just set to zero vaues for no reason
	//		}
	memset(data, 0, _s * sizeof(MogiDouble));
}

void Matrix::name(const char* array) {
	unsigned int i = 0;

	while (*(array + i) != 0)
		i++;

#ifdef UNIX
	if (label != NULL)
		free(label);
	// delete [] label;

	// label = new char [i+1];
	label = (char*) malloc((i + 1) * sizeof(char));

	for (unsigned int j = 0; j <= i; j++)
		label[j] = array[j];
#else
// if(label != NULL)
//	free(label);//delete [] label;

// label = (char *)malloc(sizeof(char)*(i+1));//new (nothrow) char [i+1];
#endif
}

float* Matrix::dataAsFloat() {
#ifdef UNIX
	copyToFloat();
	return _u;
#else
	return data;
#endif
}

void Matrix::setupScratch(unsigned int number) {
	if (numScratch != number) {
		/*if (scratch == NULL) {
		 scratch = new Matrix[number];
		 numScratch = number;
		 }
		 return;*/
#ifdef UNIX
		if (scratch != NULL) {
			delete[] scratch;
		}
		if (number > 0) {
			scratch = new Matrix[number];
		} else {
			scratch = NULL;
		}

#else
		if (scratch != NULL) {
			for (unsigned int i = 0; i < numScratch; i++) {
				scratch[i].~Matrix();
			}
			free(scratch);
		}
		if (number > 0) {
			scratch = (Matrix*)malloc(number * sizeof(Matrix));
			for (unsigned int i = 0; i < number; i++) {
				new (&scratch[i]) Matrix;
			}
		} else {
			scratch = NULL;
		}
#endif
		numScratch = number;
	}
}

/*//////////////////////////////////*/
/*   -Constructors/Destructors-     */
/*//////////////////////////////////*/

//	void Matrix::deleteMe() {
//		if (_v != NULL) {
//#ifdef DEBUG_MATH
//			char str1[32];
//			if(label != NULL) sprintf(str1,"%s",label);
//			std::cout << "Cleared memory! Deleted " << str1 << ". matrix
//size:" << s << std::endl;
//			std::cout << "\tlabel: " << label << std::endl;
//#endif
//#ifdef UNIX
//			free(_v);
//			free(_u);
////			delete [] _v;
////			delete [] _u;
//		}
//		if( label != NULL)
//			free(label);
////			delete [] label;
//
//		if (scratch != NULL) {
//			setupScratch(0);
//			//delete [] scratch;
//			numScratch = 0;
//			scratch = NULL;
//		}
//#else
//		free(_v);// delete [] v;
//		//free(u);//delete [] u;
//	}
//
//	if (scratch != NULL) {
//		setupScratch(0);
//	}
//
//#endif
//}
Matrix::Matrix() :
		_m(0), _n(0), _s(0), data(NULL), numScratch(0), scratch(NULL), _u(NULL), label(
		NULL) {
	name("(null)");
	//	set_size(_m, _n);
}

// Copy constructor
Matrix::Matrix(const Matrix& param) :
		_m(0), _n(0), _s(0), data(NULL), numScratch(0), scratch(NULL), _u(NULL), label(
		NULL) {
	*this = param;
	name("(null)");
}

// size specification constructor:
Matrix::Matrix(unsigned int size_rows, unsigned int size_cols) :
		_s(0), data(NULL), numScratch(0), scratch(NULL), _u(NULL), label(NULL) {
	setSize(size_rows, size_cols);
	name("(null)");
}

Matrix& Matrix::operator=(const Matrix& param) {
	if (this != &param)  // only run if it is not the same object
			{
		this->setSize(param.numRows(), param.numColumns()); // should deallocate then reallocate...
		memcpy(this->data, param.data, this->_s * sizeof(MogiDouble));
	}
#ifdef DEBUG_MATRICES
//	 else printf("Uhhh, why are you trying to set a matrix to itself?
//:-/\n");
#endif
	return *this;
}

Matrix::~Matrix() {
	// deleteMe();
	if (data != NULL) {
#ifdef DEBUG_MATH
		char str1[32];
		if (label != NULL) sprintf(str1, "%s", label);
		std::cout << "Cleared memory! Deleted " << str1 << ". matrix size:" << s
		<< std::endl;
		std::cout << "\tlabel: " << label << std::endl;
#endif
#ifdef UNIX
		free(data);
		free(_u);
		//			delete [] _v;
		//			delete [] _u;
	}
	if (label != NULL)
		free(label);
	//			delete [] label;

	if (scratch != NULL) {
		setupScratch(0);
		// delete [] scratch;
		numScratch = 0;
		scratch = NULL;
	}
#else
	free(data);  // delete [] v;
	// free(u);//delete [] u;
}

if (scratch != NULL) {
	setupScratch(0);
}

#endif
}

/*//////////////////////////////////*/
/*     -Overloaded Operators-       */
/*//////////////////////////////////*/
MogiDouble& Matrix::operator()(const unsigned int& m_des,
		const unsigned int& n_des) {
	if (m_des >= _m || n_des > _n) {
#ifdef DEBUG_MATRICES
		std::cout << "index out of bounds for Matrix " << label << std::endl;
#endif
		return *(MogiDouble*) NULL; // cause a null exception
	}

	return *(data + m_des + _m * n_des);
}

MogiDouble& Matrix::value(const unsigned int& m_des,
		const unsigned int& n_des) {
	return *(data + m_des + _m * n_des);
}

Matrix& Matrix::operator+=(const Matrix& param) {
	if ((param.numRows() == _m) && (param.numColumns() == _n)) {
		for (unsigned int j = 0; j < _s; j++)
			data[j] += param.valueLinearIndex(j);
	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error: matrix sizes do not agree between matrices "
		<< label << " and " << param.label << " ( " << _m << "," << _n
		<< " != " << param.numRows() << "," << param.numRows()
		<< " ), performing no += operation" << std::endl;
	}
#endif
	return *this;
}

Matrix& Matrix::operator-=(const Matrix& param) {
	if ((param.numRows() == _m) && (param.numColumns() == _n)) {
		for (unsigned int j = 0; j < _s; j++)
			data[j] -= param.valueLinearIndex(j);
	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error: matrix sizes do not agree between matrices "
		<< label << " and " << param.label << " ( " << _m << "," << _n
		<< " != " << param.numRows() << "," << param.numRows()
		<< " ), performing no -= operation" << std::endl;
	}
#endif

	return *this;
}

Matrix& Matrix::operator*=(const Matrix& param)  // A *= B (A = A*B)
		{
	(*this) = (*this) * param;
	return (*this);
}

Matrix& Matrix::operator*=(const MogiDouble& param) {
	for (unsigned int j = 0; j < _s; j++)
		data[j] *= param;

	return *this;
}

Matrix& Matrix::operator+=(const MogiDouble& param) {
	for (unsigned int j = 0; j < _s; j++)
		data[j] += param;

	return *this;
}

Matrix& Matrix::operator/=(const MogiDouble& param) {
	for (unsigned int j = 0; j < _s; j++)
		data[j] /= param;

	return *this;
}

Matrix& Matrix::operator^=(const int& param) {
	// Matrix result;
	// result = *this;
	if (_m == _n) {
		if (param == 0) {
			makeI(_m);
		} else {
			Matrix temp = *this;

			if (param < 0) {  // find inverse first
				*this = inverse();
				temp = *this;
				for (int j = -1; j > param; j--)
					*this *= temp;
			} else {
				for (int j = 1; j < param; j++)
					(*this) *= temp;
			}
		}
	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error: matrix must be square " << label << " ( " << _m
		<< "!=" << _n << " ), performing no ^= operation" << std::endl;
	}
#endif

	return *this;
}

const Matrix& Matrix::operator+(const Matrix& param) const {
	(*scratch) = *this;
	(*scratch) += param;

	return (*scratch);
}

const Matrix& Matrix::operator-(const Matrix& param) const {
	(*scratch) = *this;
	(*scratch) -= param;

	return (*scratch);
}

const Matrix& Matrix::operator*(const MogiDouble& param) const {
	(*scratch) = *this;
	(*scratch) *= param;

	return (*scratch);
}

const Matrix& Matrix::operator*(const Matrix& param) const {
	// This seems like a very peculiar method, but is hand-tuned for speed and
	// therefore looks ugly
	scratch->setSize(_m, param.numColumns());
	MogiDouble myValue;
	unsigned int rowLocation;

	if (_n == param.numRows()) {
		for (unsigned int i = 0; i < _m; i++)  // for all rows of A
				{
			for (unsigned int k = 0; k < param.numColumns(); k++) // for all columns of B
					{
				myValue = 0;
				rowLocation = k * param.numRows();
				for (unsigned int j = 0; j < _n; j++)  // for all columns of A
						{
					myValue += valueAsConst(i, j)
							* param.valueLinearIndex(rowLocation + j); //(j, k);
				}
				scratch->value(i, k) = myValue;
			}
		}

	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error: matrix sizes do not agree between matrices "
		<< label << " and " << param.label << " ( " << _m << "," << _n
		<< " != " << param.numRows() << "," << param.numRows()
		<< " ), performing no *= operation" << std::endl;
	}
#endif

	return (*scratch);
}

const Matrix& Matrix::operator/(const MogiDouble& param) const {
	(*scratch) = *this;
	(*scratch) /= param;

	return (*scratch);
}

const Matrix& Matrix::operator^(const int& param) const {
	(*scratch) = *this;
	(*scratch) ^= param;

	return (*scratch);
}

/*//////////////////////////////////*/
/*        -Basic functions-         */
/*//////////////////////////////////*/

const Matrix& Matrix::transpose(void) const  // Transpose
		{
	scratch->setSize(_n, _m);
	for (unsigned int i = 0; i < _m; i++)    // columns
		for (unsigned int j = 0; j < _n; j++)  // rows
			scratch->value(j, i) = (*this).valueAsConst(i, j);

	return (*scratch);
}

void Matrix::makeI(unsigned int dimension) {
	setSize(dimension, dimension);
	for (unsigned int i = 0; i < dimension; i++) {
		this->value(i, i) = 1;
	}
}

MogiDouble Matrix::determinant(void) const  // Determinant
		{
	MogiDouble result = 0;
	// MogiDouble temp;
	// int s = n;
	Matrix tempM, tempM2;
	tempM = *this;

	if (_n > 2) {
		for (unsigned int i = 0; i < _n; i++)  // for all cols
				{
			// temp = 1;
			// for(int k=0;k<i;k++) temp *= -1;
			tempM2 = tempM.subMatrix(0, i);
			if (i & 1) {
				result += -valueAsConst(0, i) * tempM2.determinant(); // woo recursion!
			} else {
				result += valueAsConst(0, i) * tempM2.determinant(); // woo recursion!
			}
		}
	} else if (_n == 2) {
		result = valueAsConst(0, 0) * valueAsConst(1, 1)
				- valueAsConst(1, 0) * valueAsConst(0, 1);
	} else if (_n == 1)
		result = valueAsConst(0, 0);

	return result;
}

const Matrix& Matrix::inverse(void) const  // Inverse
		{
	(*scratch) = cofactor();

	return (scratch->transpose() / determinant());
}

const Matrix& Matrix::cofactor(void) const  // Inverse
		{
	scratch->setSize(_m, _n);
	Matrix tempM, tempM2;
	tempM = *this;

	for (unsigned int i = 0; i < _m; i++)
		for (unsigned int j = 0; j < _n; j++) {
			// int temp = 1;
			// for(int k=0;k<(i+j);k++) temp *= -1;
			tempM2 = tempM.subMatrix(i, j);
			if ((i + j) & 1) {
				scratch->value(i, j) = -tempM2.determinant();
			} else {
				scratch->value(i, j) = tempM2.determinant();
			}
			// result(i,j) = temp * tempM2.det();
		}

	return (*scratch);
}

const Matrix& Matrix::subMatrix(unsigned int des_m, unsigned int des_n) const // Inverse
		{
	// Matrix result(m-1,n-1);
	scratch->setSize(_m - 1, _n - 1);
	unsigned int i2 = 0, j2;

	for (unsigned int i = 0; i < _m; i++) {
		if (i != des_m) {
			j2 = 0;
			for (unsigned int j = 0; j < _n; j++) {
				if (j != des_n)
					scratch->value(i2, j2++) = valueAsConst(i, j);
			}
			i2++;
		}
	}

	return (*scratch);
}

void Matrix::print(void) const {
//#ifdef DEBUG_MATRICES
	std::cout << "matrix: " << label << "=" << std::endl;

	if (_s == 0)
	std::cout << "\tNothing allocated..." << std::endl;
	else if (_s == 1) {
		std::cout << "\t[" << data[0] << "]" << std::endl;
	} else {
		for (unsigned int j = 0; j < _m; j++) {
			std::cout << "\t[";
			for (unsigned int i = 0; i < _n; i++)
			std::cout << valueAsConst(j, i) << "\t";
			std::cout << "]" << std::endl;
		}
	}
//#endif
}

inline float SIGN(float x) {
	return (x >= 0.0f) ? +1.0f : -1.0f;
}
inline float NORM(float a, float b, float c, float d) {
	return sqrt(a * a + b * b + c * c + d * d);
}

const Quaternion Matrix::rotationToQuaternion(void) {
	Quaternion q;
	if (((_n == 3) && (_m == 3)) || ((_n == 4) && (_m == 4))) {
		/*
		 MogiDouble r;

		 q(0,0) = ( (*this)(2,2) + (*this)(1,1) + (*this)(0,0) + 1.0f) / 4.0f;
		 q(1,0) = ( (*this)(2,2) - (*this)(1,1) - (*this)(0,0) + 1.0f) / 4.0f;
		 q(2,0) = (-(*this)(2,2) + (*this)(1,1) - (*this)(0,0) + 1.0f) / 4.0f;
		 q(3,0) = (-(*this)(2,2) - (*this)(1,1) + (*this)(0,0) + 1.0f) / 4.0f;
		 if(q(0,0) < 0.0f) q(0,0) = 0.0f;
		 if(q(1,0) < 0.0f) q(1,0) = 0.0f;
		 if(q(2,0) < 0.0f) q(2,0) = 0.0f;
		 if(q(3,0) < 0.0f) q(3,0) = 0.0f;
		 q(0,0) = sqrt(q(0,0));
		 q(1,0) = sqrt(q(1,0));
		 q(2,0) = sqrt(q(2,0));
		 q(3,0) = sqrt(q(3,0));
		 if((q(0,0) >= q(1,0)) && (q(0,0) >= q(2,0)) && (q(0,0) >= q(3,0))) {
		 q(0,0) *= +1.0f;
		 q(1,0) *= SIGN((*this)(1,0) - (*this)(0,1));
		 q(2,0) *= SIGN((*this)(0,2) - (*this)(2,0));
		 q(3,0) *= SIGN((*this)(2,1) - (*this)(1,2));
		 } else if((q(1,0) >= q(0,0)) && (q(1,0) >= q(2,0)) && (q(1,0) >= q(3,0))) {
		 q(0,0) *= SIGN((*this)(1,0) - (*this)(0,1));
		 q(1,0) *= +1.0f;
		 q(2,0) *= SIGN((*this)(2,1) + (*this)(1,2));
		 q(3,0) *= SIGN((*this)(0,2) + (*this)(2,0));
		 } else if((q(2,0) >= q(0,0)) && (q(2,0) >= q(1,0)) && (q(2,0) >= q(3,0))) {
		 q(0,0) *= SIGN((*this)(0,2) - (*this)(2,0));
		 q(1,0) *= SIGN((*this)(2,1) + (*this)(1,2));
		 q(2,0) *= +1.0f;
		 q(3,0) *= SIGN((*this)(1,0) + (*this)(0,1));
		 } else if((q(3,0) >= q(0,0)) && (q(3,0) >= q(1,0)) && (q(3,0) >= q(2,0))) {
		 q(0,0) *= SIGN((*this)(2,1) - (*this)(1,2));
		 q(1,0) *= SIGN((*this)(2,0) + (*this)(0,2));
		 q(2,0) *= SIGN((*this)(1,0) + (*this)(0,1));
		 q(3,0) *= +1.0f;
		 } else {
		 #ifdef DEBUG_MATRICES
		 std::cout << "Coding error on quaternion: " << label << std::endl;
		 #endif
		 }
		 r = NORM(q(0,0), q(1,0), q(2,0), q(3,0));
		 q(0,0) /= r;
		 q(1,0) /= r;
		 q(2,0) /= r;
		 q(3,0) /= r;

		 r = q(0,0);
		 q(0,0) = q(3,0);
		 q(3,0) = r;
		 r = q(1,0);
		 q(1,0) = q(2,0);
		 q(2,0) = r;

		 */

		// Converted from :
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		float trace = (*this)(0, 0) + (*this)(1, 1) + (*this)(2, 2); // I removed + 1.0f; see discussion with Ethan
		if (trace > 0) {              // I changed M_EPSILON to 0
			float s = 0.5f / sqrtf(trace + 1.0f);
			q(0, 0) = 0.25f / s;
			q(1, 0) = ((*this)(2, 1) - (*this)(1, 2)) * s;
			q(2, 0) = ((*this)(0, 2) - (*this)(2, 0)) * s;
			q(3, 0) = ((*this)(1, 0) - (*this)(0, 1)) * s;
		} else {
			if ((*this)(0, 0) > (*this)(1, 1)
					&& (*this)(0, 0) > (*this)(2, 2)) {
				float s = 2.0f
						* sqrtf(
								1.0f + (*this)(0, 0) - (*this)(1, 1)
										- (*this)(2, 2));
				q(0, 0) = ((*this)(2, 1) - (*this)(1, 2)) / s;
				q(1, 0) = 0.25f * s;
				q(2, 0) = ((*this)(0, 1) + (*this)(1, 0)) / s;
				q(3, 0) = ((*this)(0, 2) + (*this)(2, 0)) / s;
			} else if ((*this)(1, 1) > (*this)(2, 2)) {
				float s = 2.0f
						* sqrtf(
								1.0f + (*this)(1, 1) - (*this)(0, 0)
										- (*this)(2, 2));
				q(0, 0) = ((*this)(0, 2) - (*this)(2, 0)) / s;
				q(1, 0) = ((*this)(0, 1) + (*this)(1, 0)) / s;
				q(2, 0) = 0.25f * s;
				q(3, 0) = ((*this)(1, 2) + (*this)(2, 1)) / s;
			} else {
				float s = 2.0f
						* sqrtf(
								1.0f + (*this)(2, 2) - (*this)(0, 0)
										- (*this)(1, 1));
				q(0, 0) = ((*this)(1, 0) - (*this)(0, 1)) / s;
				q(1, 0) = ((*this)(0, 2) + (*this)(2, 0)) / s;
				q(2, 0) = ((*this)(1, 2) + (*this)(2, 1)) / s;
				q(3, 0) = 0.25f * s;
			}
		}

	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error!  Quaternion may not be computed for matrix size m="
		<< _m << " n=" << _n << "\n - Needs to be a 3x3 matrix"
		<< std::endl;
	}
#endif
	return q;
}

void Matrix::makeXRotation(MogiDouble angle) {
	if ((_m != 3) || (_n != 3)) {
		this->setSize(3, 3);
	}
	for (unsigned int i = 0; i < _s; i++) {
		data[i] = 0;
	}

	(*this)(0, 0) = 1;
	(*this)(1, 1) = (*this)(2, 2) = cos(angle);
	(*this)(2, 1) = sin(angle);
	(*this)(1, 2) = -(*this)(2, 1);
}

void Matrix::makeYRotation(MogiDouble angle) {
	if ((_m != 3) || (_n != 3)) {
		this->setSize(3, 3);
	}
	for (unsigned int i = 0; i < _s; i++) {
		data[i] = 0;
	}

	(*this)(1, 1) = 1;
	(*this)(0, 0) = (*this)(2, 2) = cos(angle);
	(*this)(0, 2) = sin(angle);
	(*this)(2, 0) = -(*this)(0, 2);
}

void Matrix::makeZRotation(MogiDouble angle) {
	if ((_m != 3) || (_n != 3)) {
		this->setSize(3, 3);
	}
	for (unsigned int i = 0; i < _s; i++) {
		data[i] = 0;
	}
	(*this)(2, 2) = 1;
	(*this)(0, 0) = (*this)(1, 1) = cos(angle);
	(*this)(1, 0) = sin(angle);
	(*this)(0, 1) = -(*this)(1, 0);
}

void Matrix::normalize() {
	if ((*this).numColumns() == 1) {
		MogiDouble sum = 0;
		for (unsigned int i = 0; i < _s; i++) {
			sum += (*(data + i)) * (*(data + i));
		}
		MogiDouble magnitude = sqrt(sum);
		for (unsigned int i = 0; i < _s; i++) {
			*(data + i) /= magnitude;
		}
	}
#ifdef DEBUG_MATRICES
	else {
		std::cout
		<< "normalize() has not been fully implemented yet for matrices "
		"with these dimensions." << std::endl;

	}
#endif
}

const Matrix& Matrix::cross(const Matrix& matrixOnRight) {
	scratch->setSize(3, 1);
	if (((*this).numColumns() == 1) && (matrixOnRight.numColumns() == 1)
			&& ((*this).numRows() == 3) && (matrixOnRight.numRows() == 3)) {
		scratch->value(0, 0) = (*this)(1, 0) * matrixOnRight.valueAsConst(2, 0)
				- (*this)(2, 0) * matrixOnRight.valueAsConst(1, 0);
		scratch->value(1, 0) = (*this)(2, 0) * matrixOnRight.valueAsConst(0, 0)
				- (*this)(0, 0) * matrixOnRight.valueAsConst(2, 0);
		scratch->value(2, 0) = (*this)(0, 0) * matrixOnRight.valueAsConst(1, 0)
				- (*this)(1, 0) * matrixOnRight.valueAsConst(0, 0);
	}
#ifdef DEBUG_MATRICES
	else {
		std::cout << "Error: Matrix dimensions do not agree between matrices "
		<< label << " and " << matrixOnRight.label << "  ( " << _m
		<< "," << _n << " X " << matrixOnRight.numRows() << ","
		<< matrixOnRight.numColumns()
		<< " ), performing no cross operation" << std::endl;

	}
#endif
	return (*scratch);
}

MogiDouble Matrix::dot(const Matrix& matrixOnRight) const {
	MogiDouble result = 0;
	if (((*this).numColumns() == 1) && (matrixOnRight.numColumns() == 1)
			&& ((*this).numRows() == matrixOnRight.numRows())) {
		for (unsigned int i = 0; i < _s; i++) {
			result += (*this).valueAsConst(i, 0)
					* matrixOnRight.valueAsConst(i, 0);
		}
	} else {
#ifdef DEBUG_MATRICES
		std::cout << "Error: Matrix dimensions do not agree between matrices "
		<< label << " and " << matrixOnRight.label << "  ( " << _m
		<< "," << _n << " X " << matrixOnRight.numRows() << ","
		<< matrixOnRight.numColumns()
		<< " ), performing no dot operation" << std::endl;
#endif
	}

	return result;
}
const Matrix GLKMatrix4MakeTranslation(MogiDouble x, MogiDouble y,
		MogiDouble z) {
	Matrix ret(4, 4);
	ret(0, 0) = 1;
	ret(1, 1) = 1;
	ret(2, 2) = 1;
	ret(3, 3) = 1;
	ret(0, 3) = x;
	ret(1, 3) = y;
	ret(2, 3) = z;
	return ret;
}

const Matrix GLKMatrix4MakePerspective(MogiDouble fovRadians, MogiDouble aspect,
		MogiDouble near, MogiDouble far) {
	Matrix ret(4, 4);

	MogiDouble yScale = 1.0 / tan(fovRadians / 2);
	MogiDouble xScale = yScale / aspect;
	MogiDouble nearmfar = near - far;

	ret(0, 0) = xScale;
	ret(1, 1) = yScale;
	ret(2, 2) = (far + near) / nearmfar;
	ret(2, 3) = 2 * far * near / nearmfar;
	ret(3, 2) = -1;

	// z' = (z*((far + near) / nearmfar) + 1*(2*far*near / nearmfar))/z
	// => z = (z' - 1*(2*far*near / nearmfar))/((far + near) / nearmfar)
	// => z = z'/((far + near) / nearmfar) - (2*far*near / nearmfar)/((far + near)
	// / nearmfar)
	// => z = nearmfar*z'/(far + near) - 2*far*near/(far + near)

	return ret;
}

const Matrix GLKMatrix4MakeOrtho(MogiDouble left, MogiDouble right,
		MogiDouble bottom, MogiDouble top, MogiDouble nearZ, MogiDouble farZ) {
	Matrix ret(4, 4);

	ret(0, 0) = 2.0 / (right - left);
	ret(1, 1) = 2.0 / (top - bottom);
	ret(2, 2) = -2.0 / (farZ - nearZ);
	ret(3, 3) = 1.0;
	ret(0, 3) = -(right + left) / (right - left);
	ret(1, 3) = -(top + bottom) / (top - bottom);
	ret(2, 3) = -(farZ + nearZ) / (farZ - nearZ);

	return ret;
}

int transformationToScaleRotationTranslation(const Matrix* transformation,
		Matrix* scale, Matrix* rotation, Matrix* translation) {
	if (transformation == NULL) {
#ifdef DEBUG_MATRICES
		std::cout << "Error: NULL transformation matrix in "
		"transformationToScaleRotationTranslation()" << std::endl;
#endif
		return -1;
	}

	if ((transformation->numRows() != 4)
			|| (transformation->numColumns() != 4)) {
#ifdef DEBUG_MATRICES
		std::cout << "Error: transformation matrix has invalid dimensions in "
		"transformationToScaleRotationTranslation()" << std::endl;
#endif
		return -1;
	}

	if ((transformation->valueAsConst(3, 1) != 0)
			|| (transformation->valueAsConst(3, 1) != 0)
			|| (transformation->valueAsConst(3, 2) != 0)
			|| (transformation->valueAsConst(3, 3) != 1)) {
#ifdef DEBUG_MATRICES
		std::cout << "Error: transformation matrix is homogeneous in "
		"transformationToScaleRotationTranslation()" << std::endl;
#endif
		return -1;
	}

	if (translation !=
	NULL) {  // Create the translation matrix, only if it exists:
		translation->makeI(4);
		translation->value(0, 3) = transformation->valueAsConst(0, 3);
		translation->value(1, 3) = transformation->valueAsConst(1, 3);
		translation->value(2, 3) = transformation->valueAsConst(2, 3);
	}

	bool deleteScale = false;

	if (scale == NULL) {
		if (rotation == NULL) {
			return 0;
		}
		scale = Matrix::create();
		deleteScale = true;
	}

	scale->makeI(4);
	scale->value(0, 0) = sqrt(
			transformation->valueAsConst(0, 0)
					* transformation->valueAsConst(0, 0)
					+ transformation->valueAsConst(1, 0)
							* transformation->valueAsConst(1, 0)
					+ transformation->valueAsConst(2, 0)
							* transformation->valueAsConst(2, 0));
	scale->value(1, 1) = sqrt(
			transformation->valueAsConst(0, 1)
					* transformation->valueAsConst(0, 1)
					+ transformation->valueAsConst(1, 1)
							* transformation->valueAsConst(1, 1)
					+ transformation->valueAsConst(2, 1)
							* transformation->valueAsConst(2, 1));
	scale->value(2, 2) = sqrt(
			transformation->valueAsConst(0, 2)
					* transformation->valueAsConst(0, 2)
					+ transformation->valueAsConst(1, 2)
							* transformation->valueAsConst(1, 2)
					+ transformation->valueAsConst(2, 2)
							* transformation->valueAsConst(2, 2));

	if (transformation->determinant() < 0) { // If the determinant is negative, we have
		// to negate an axis.  Unfortunately, we
		// just have to choose one...
		scale->value(2, 2) *= -1.0;
	}
	/*if (transformation->fastValue(0,0) + transformation->fastValue(1,0) +
	 transformation->fastValue(2,0) < 0) {
	 scale->fastValue(0, 0) *= -1.0;
	 }
	 if (transformation->fastValue(0,1) + transformation->fastValue(1,1) +
	 transformation->fastValue(2,1) < 0) {
	 scale->fastValue(1, 1) *= -1.0;
	 }
	 if (transformation->fastValue(0,2) + transformation->fastValue(1,2) +
	 transformation->fastValue(2,2) < 0) {
	 scale->fastValue(2, 2) *= -1.0;
	 }*/

	if (rotation != NULL) {
		*rotation = *transformation;

		rotation->value(0, 0) /= scale->value(0, 0);
		rotation->value(1, 0) /= scale->value(0, 0);
		rotation->value(2, 0) /= scale->value(0, 0);

		rotation->value(0, 1) /= scale->value(1, 1);
		rotation->value(1, 1) /= scale->value(1, 1);
		rotation->value(2, 1) /= scale->value(1, 1);

		rotation->value(0, 2) /= scale->value(2, 2);
		rotation->value(1, 2) /= scale->value(2, 2);
		rotation->value(2, 2) /= scale->value(2, 2);

		rotation->value(0, 3) = 0;
		rotation->value(1, 3) = 0;
		rotation->value(2, 3) = 0;
	}

	if (deleteScale) {
		delete scale;
	}
	return 0;
}
}
}

#ifdef _cplusplus
}
#endif
