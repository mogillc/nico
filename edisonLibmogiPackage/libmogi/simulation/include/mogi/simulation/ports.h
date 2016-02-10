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

#ifndef libmogi_MBports_h
#define libmogi_MBports_h

#include <iostream>
#include "value.h"

namespace Mogi {
namespace Simulation {

class MBport {
protected:
	MBtype* value;
	int type;

public:
	enum {
		input, output
	};

	void setValue(MBtype* newValue) {
		*value = *newValue;
	}
	;
	MBtype* getValue() {
		return value;
	}
	;
	static MBport* create(int typeOfPort);
	MBport() {
		value = new MBtype;
	}
};

class MBinputPort: public MBport {
public:
	MBinputPort() {
		type = input;
	}
	;
};

class MBoutputPort: public MBport {
public:
	MBoutputPort() {
		type = output;
	}
	;
};
}
}

#endif
