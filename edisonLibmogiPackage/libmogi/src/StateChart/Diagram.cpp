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
//#include <iostream>
#include <algorithm>
#include <cctype>

#ifdef THREADS_FOUND
#include <pthread.h>
#else
#warning Threads not found.  This may still compile but will not be thread safe.
#endif

using namespace Mogi;
using namespace StateChart;

void __doNothing(void*) {
}
bool __returnTrue(void*) {
	return true;
}

Diagram::Diagram( const std::string& label ) :
		label(label), parent(NULL), currentTransition(NULL), processState(STATE_ENTRY) {
	initial = new State("initial");
	final = new State("final");
	initial->parent = this;
	final->parent = this;
	currentState = initial;
	states.push_back(initial);
	states.push_back(final);
#ifdef THREADS_FOUND
	_mutex = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*) _mutex, NULL);
#endif
}

Diagram::~Diagram() {
	for (std::vector<State*>::iterator it = states.begin(); it != states.end();
			it++) {
		delete *it;
	}
	for (std::vector<Guard*>::iterator it = guards.begin(); it != guards.end();
			it++) {
		delete *it;
	}
#ifdef THREADS_FOUND
	pthread_mutex_destroy((pthread_mutex_t*) _mutex);
	delete (pthread_mutex_t*) _mutex;
#endif
}

const std::string& Diagram::name() {
	return label;
}

State* Diagram::addState(State* state) {
	for (std::vector<State*>::iterator it = states.begin(); it != states.end();
			it++) {
		if (*it == state) {
			return state;
		}
	}
	state->parent = this;
	states.push_back(state);
	return state;
}

void Diagram::removeState(State* state) {
	if (state == initial || state == final) {
		return;
	}
	for (std::vector<State*>::iterator it = states.begin(); it != states.end();
			) {
		if (state == *it) {
			delete *it;
			it = states.erase(it);
		} else {
			for (std::vector<Transition*>::iterator it2 =
					(*it)->outgoingTransitions.begin();
					it2 != (*it)->outgoingTransitions.end();) {
				if ((*it2)->dst == state) {
					delete *it2;
					it2 = (*it)->outgoingTransitions.erase(it2);
				} else {
					it2++;
				}
			}
			it++;
		}
	}
}

const std::vector<State*>& Diagram::getStates() {
	return states;
}

State* Diagram::getCurrentState() {
	return currentState;
}

State* Diagram::getInitialState() {
	return initial;
}

State* Diagram::getFinalState() {
	return final;
}

bool Diagram::perform(Transition* transition) {
	bool transitionPerformed = false;

#ifdef THREADS_FOUND
	pthread_mutex_lock((pthread_mutex_t*) _mutex);
	transitionPerformed = performGettingMessy(transition);
	pthread_mutex_unlock((pthread_mutex_t*) _mutex);
#else
	transitionPerformed = performGettingMessy(transition);
#endif
	return transitionPerformed;
}

bool Diagram::performGettingMessy(Transition* transition) {
	bool transitionPerformed = false;
	if (transition == NULL) {
		process();
		transitionPerformed = true;
	} else {
		if (transition->parent == currentState
				&& transition->guardsSatisfied()) {
			currentTransition = transition;
			processState = TRANSITION;
			process();
			transitionPerformed = true;
		}
	}
	return transitionPerformed;
}

void Diagram::update() {
	perform(NULL);
}

void Diagram::notifyObservers(State* state) {
	if (dynamic_cast<CompositeState*>(state) != NULL) {
		// Only simple states may cause notifications
		return;
	}

	if (parent != NULL) {
		// Let the parent perform the notification
		parent->parent->notifyObservers(state);
		return;
	}

	for (std::vector<StateChangeObserver*>::iterator it =
			stateChangeObservers.begin(); it != stateChangeObservers.end();
			it++) {
		(*it)->notify(this);
	}
}

void Diagram::process() {
	switch (processState) {
	case STATE_ENTRY:
		currentState->actionEntry();
		notifyObservers(currentState);
		processState = STATE_DO;
		process();
		break;

	case STATE_DO:
		currentState->actionDo();
		for (std::vector<Transition*>::iterator it =
				currentState->outgoingTransitions.begin();
				it != currentState->outgoingTransitions.end(); it++) {
			if (!(*it)->hasEvent() && performGettingMessy(*it)) {
				return;
			}
		}
		break;

	case TRANSITION:
		currentState->actionExit();
		currentTransition->action();

		currentState = currentTransition->dst;
		processState = STATE_ENTRY;
		process();
		break;
	}
}

void Diagram::addGuard(Guard* guard) {
	if (parent != NULL) {
		parent->parent->addGuard(guard);
		return;
	}

	for (std::vector<Guard*>::iterator it = guards.begin(); it != guards.end();
			it++) {
		if (guard == *it) {
			return;
		}
	}

	guards.push_back(guard);
}

void Diagram::reset() {
	currentState = initial;
	processState = STATE_ENTRY;
}

void Diagram::addObserver(StateChangeObserver* observer) {
	if (parent != NULL) {
		parent->parent->addObserver(observer);
		return;
	}
	for (std::vector<StateChangeObserver*>::iterator it =
			stateChangeObservers.begin(); it != stateChangeObservers.end();
			it++) {
		if (*it == observer) {
			return;
		}
	}
#ifdef THREADS_FOUND
	pthread_mutex_lock((pthread_mutex_t*) _mutex);
#endif
	stateChangeObservers.push_back(observer);
#ifdef THREADS_FOUND
	pthread_mutex_unlock((pthread_mutex_t*) _mutex);
#endif
}

void Diagram::removeObserver(StateChangeObserver* observer) {
	if (parent != NULL) {
		parent->parent->removeObserver(observer);
		return;
	}
#ifdef THREADS_FOUND
	pthread_mutex_lock((pthread_mutex_t*) _mutex);
#endif
	stateChangeObservers.erase(
			std::remove(stateChangeObservers.begin(),
					stateChangeObservers.end(), observer),
			stateChangeObservers.end());
#ifdef THREADS_FOUND
	pthread_mutex_unlock((pthread_mutex_t*) _mutex);
#endif
}

StateChangeObserver::~StateChangeObserver() {
}
