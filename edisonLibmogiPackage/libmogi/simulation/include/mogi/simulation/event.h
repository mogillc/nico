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

#ifndef libmogi_MBevent_h
#define libmogi_MBevent_h

#include <iostream>
#include "function.h"

namespace Mogi {
namespace Simulation {

class MBevent {
private:
	Function *function;
	MBevent *nextEvent;

public:
	MBevent() {
		function = NULL;
		nextEvent = NULL;
	}

	void setFunction(Function *yourFunction) {
		function = yourFunction;
	}
	void setChildEvent(MBevent *child) {
		nextEvent = child;
	}

	void trigger();
};
}
}

#endif
