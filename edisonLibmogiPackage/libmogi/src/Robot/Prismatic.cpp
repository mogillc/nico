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

	void Prismatic::setValue( double value ) {
		if(node) {
			this->value = value; // TODO: this is always required, maybe implement two functions for this.
			node->setLocation(axis * value);
		}
	}

#ifdef _cplusplus
}
#endif
