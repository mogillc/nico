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

#include "joint.h"

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Robot;
	using namespace Math;

	Joint::Joint() :
	node(NULL), axis(Vector::zAxis) {
	}

	Joint* Joint::create(Math::Node* node, const Math::Vector& axis, Type type) {
		Joint* result = NULL;
		switch (type) {
			case PRISMATIC:
				result = new Prismatic;
				break;

			case REVOLUTE:
				result = new Revolute;
				break;
		}
		result->setAxis( axis );
		result->setNode( node );
		return result;
	}

	Joint::~Joint() {
	}

	void Joint::setNode( Math::Node* node ) {
		this->node = node;
	}

	void Joint::setAxis( const Math::Vector& axis ) {
		this->axis = axis;
	}

	double Joint::getValue() {
		return value;
	}


#ifdef _cplusplus
}
#endif
