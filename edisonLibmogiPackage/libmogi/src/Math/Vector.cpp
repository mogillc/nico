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

#include "mmath.h"

#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

namespace Mogi {
namespace Math {

Vector* Vector::create() {
#ifdef UNIX
	return new Vector;
}
#else
Vector* result = (Vector*)malloc(sizeof(Vector));
new (result) Vector;
return result;
}
#endif

Vector::Vector(unsigned int length) :
		Matrix(length, 1) {
}
;

/*//////////////////////////////////*/
/*     -Overloaded Operators-       */
/*//////////////////////////////////*/
MogiDouble& Vector::operator()(const unsigned int& m_des) {
	return (*this)(m_des, 0);
	// return garbage;
}

Vector& Vector::operator=(const Matrix& param) {
	if (param.numColumns() == 1) {
		this->setLength(param.numRows());
		for (unsigned int i = 0; i < param.numRows(); i++) {
			(*this)(i) = param.valueAsConst(i, 0);
		}
	}
#ifdef DEBUG_MATH
	else {
		printf(
				"Error: matrix assignment to vector must have 1 column, dimensionality "
				"of %s is (%d, %d), performing no = operation\n",
				param.label, param.numRows(), param.numColumns());
	}
#endif

	return *this;
}

// Dot product
MogiDouble Vector::operator*(const Vector& param) const {
	MogiDouble result = 0;
	//    Vector temp;
	//    temp = *this; // this is messy, should be able to doVector result =
	//    *this

	if (param.size() == this->size()) {
		for (unsigned int j = 0; j < this->size(); j++) {
			// printf("(*this)(%d,0) = %f\n", j, (*this)(j,0));
			result += valueAsConst(j, 0) * param.valueAsConst(j, 0);
		}
	}
#ifdef DEBUG_MATH
	else {
		printf(
				"Error: vector sizes do not agree between vectors %s and %s ( %d != %d "
				"), performing no * operation\n",
				label, param.label, this->Size(), param.Size());
	}
#endif

	return result;
}

void Vector::setLength(unsigned int length) {
	this->setSize(length, 1);
}

MogiDouble Vector::magnitude(void) const {
	return sqrt((*this) * (*this));
}

struct XAxis {
	operator Vector() {
		Vector axis(3);
		axis(0) = 1;
		axis.name("xAxis");
		return axis;
	}
};
const Vector& Vector::xAxis = XAxis();

struct YAxis {
	operator Vector() {
		Vector axis(3);
		axis(1) = 1;
		axis.name("yAxis");
		return axis;
	}
};
const Vector& Vector::yAxis = YAxis();

struct ZAxis {
	operator Vector() {
		Vector axis(3);
		axis(2) = 1;
		axis.name("zAxis");
		return axis;
	}
};
const Vector& Vector::zAxis = ZAxis();
}
}
#ifdef _cplusplus
}
#endif
