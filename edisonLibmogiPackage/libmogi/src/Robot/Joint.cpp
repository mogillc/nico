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
