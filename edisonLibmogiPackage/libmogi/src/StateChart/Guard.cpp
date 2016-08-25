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

CallbackGuard Guard::getGuardCallback() {
	return _guard;
}
