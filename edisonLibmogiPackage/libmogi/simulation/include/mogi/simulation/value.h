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

#ifndef libmogi_MBvalue_h
#define libmogi_MBvalue_h

#include <string.h>
#include <iostream>
#include "mogi/math/mmath.h"

namespace Mogi {
namespace Simulation {

class MBtype {
private:
	void deleteValue();
	int setType(int newType);

public:
	enum {
		Bool, Int, Double, MBMATRIX
	};

	int type;
	void* value;

	MBtype() {
		value = NULL;
		type = -1;
	}
	;
	~MBtype() {
		deleteValue();
	}
	;

	MBtype& operator=(const MBtype& newValue);
	MBtype& operator=(const double& newValue);
	MBtype& operator=(const int& newValue);
	MBtype& operator=(const bool& newValue);

	operator bool();
	operator int();
	operator double();
};
}
}

#endif
