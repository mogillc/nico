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

Transition::Transition(State* src, State* dst) :
		parent(src), dst(dst) {
}

Transition::~Transition() {
}

bool Transition::guardsSatisfied() {
	bool shouldPerform = true;

	for (std::vector<Guard*>::iterator it = guards.begin(); it != guards.end();
			it++) {
		shouldPerform &= (*it)->isSatisfied();
	}

	return shouldPerform;
}

void Transition::notify(Event* event) {
	parent->parent->perform(this);
}

Guard* Transition::addGuard(Guard* guard) {
	for (std::vector<Guard*>::iterator it = guards.begin(); it != guards.end();
			it++) {
		if (guard == *it) {
			return guard;
		}
	}
	guards.push_back(guard);
	parent->parent->addGuard(guard);  // for deletion
	return guard;
}

void Transition::action() {
	_action.cb(_action.parameter);
}

void Transition::setCallbackAction(actionCallback cb, void* parameter) {
	_action.cb = cb;
	_action.parameter = parameter;
}

void Transition::setEvent(Event* event) {
	event->addObserver(this);
}
