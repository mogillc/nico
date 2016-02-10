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

#include "event.h"

#ifdef _cplusplus
extern "C" {
#endif
using namespace Mogi::Simulation;

void MBevent::trigger() {
	if (function) {
		function->perform();
	}
	if (nextEvent) {
		nextEvent->trigger();
	}
}

#ifdef _cplusplus
}
#endif
