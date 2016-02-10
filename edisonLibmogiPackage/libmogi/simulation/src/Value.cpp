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

#include "value.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBtype::operator int() {
	if (type == Int) {
		return *(int*) value;
	} else {
		switch (type) {
		case Double:
			return (int) *(double*) value;
			break;
		case Bool:
			return (int) *(bool*) value;
			break;
		}
	}
	return *(int*) value;
}

MBtype::operator double() {
	if (type == Double) {
		return *(double*) value;
	} else {
		switch (type) {
		case Int:
			return (double) *(int*) value;
			break;
		case Bool:
			return (double) *(bool*) value;
			break;
		}
	}
	return *(double*) value;
}
MBtype::operator bool() {
	if (type == Bool) {
		return *(bool*) value;
	} else {
		switch (type) {
		case Int:
			return (bool) *(int*) value;
			break;
		case Double:
			return (bool) *(double*) value;
			break;
		}
	}
	return *(bool*) value;
}

void MBtype::deleteValue() {
	if (value) {
		switch (type) {
		case Bool:
			delete (bool*) value;
			break;
		case Int:
			delete (int*) value;
			break;
		case Double:
			delete (double*) value;
			break;
		default:
			break;
		}
		value = NULL;
		type = -1;
	}
}
int MBtype::setType(int newType) {
	if (value) {
		switch (type) {
		case Bool:
			return sizeof(bool);
			break;

		case Int:
			return sizeof(int);
			break;

		case Double:
			return sizeof(double);
			break;

		default:
			return 0;
			break;
		}
	}
	switch (newType) {
	case Bool:
		type = Bool;
		value = new bool;
		return sizeof(bool);
		break;
	case Int:
		type = Int;
		value = new int;
		return sizeof(int);
		break;
	case Double:
		type = Double;
		value = new double;
		return sizeof(double);
		break;
	default:
		type = -1;
		break;
	}
	return 0;
}

MBtype& MBtype::operator=(const MBtype& newValue) {
	if (this->type != newValue.type) {
		deleteValue();
	}
	int newSize = setType(newValue.type);
	memcpy(this->value, newValue.value, newSize);

	return *this;
}
MBtype& MBtype::operator=(const double& newValue) {
	if (type != Double) {
		deleteValue();
		setType(Double);
	}
	*(double*) value = newValue;
	return *this;
}
MBtype& MBtype::operator=(const int& newValue) {
	if (type != Int) {
		deleteValue();
		setType(Int);
	}
	*(int*) value = newValue;
	return *this;
}
MBtype& MBtype::operator=(const bool& newValue) {
	if (type != Bool) {
		deleteValue();
		setType(Bool);
	}
	*(bool*) value = newValue;
	return *this;
}

#ifdef _cplusplus
}
#endif
