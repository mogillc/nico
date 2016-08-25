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

std::vector<Guard*> Transition::getGuards() {
	return guards;
}
