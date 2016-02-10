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
