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

EventObserver::EventObserver() :
		numberOfEvents(0) {
}

EventObserver::~EventObserver() {
}

bool EventObserver::hasEvent() {
	return numberOfEvents > 0;
}

void Event::trigger() {
	for (std::vector<EventObserver*>::iterator it = observers.begin();
			it != observers.end(); it++) {
		(*it)->notify(this);
	}
}

EventObserver* Event::addObserver(EventObserver* observer) {
	for (std::vector<EventObserver*>::iterator it = observers.begin();
			it != observers.end(); it++) {
		if (*it == observer) {
			return observer;
		}
	}

	observers.push_back(observer);
	observer->numberOfEvents++;
	return observer;
}

void Event::removeObserver(EventObserver* observer) {
	for (std::vector<EventObserver*>::iterator it = observers.begin();
			it != observers.end();) {
		if (*it == observer) {
			observer->numberOfEvents--;
			it = observers.erase(it);
		} else {
			it++;
		}
	}
}
