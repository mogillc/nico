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

#include "statechart.h"

using namespace Mogi;
using namespace StateChart;

Guard::Guard() {
}

Guard::~Guard() {
}
;

bool Guard::isSatisfied() {
	return _guard.cb(_guard.parameter);
}

void Guard::setGuardCallback(guardCallback cb, void* parameter) {
	_guard.cb = cb;
	_guard.parameter = parameter;
}
