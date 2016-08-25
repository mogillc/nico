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

#include <cstdlib>
#include <iostream>

#include "statechart.h"

using namespace Mogi;
using namespace StateChart;

struct CBtest {
	bool entryCBcalled;
	bool doCBcalled;
	bool eventCBcalled;
	bool exitCBcalled;
	bool transitionCBcalled;
};

/*
 Test classes
 */
class testStateChangeObserver: public StateChangeObserver {
private:
	void notify(Diagram* diagram) {
		notificationCount++;
	}

public:
	int notificationCount;
};

/*
 Global variables
 */

/*
 Global functions
 */
void resetFlags(CBtest* cbtest) {
	cbtest->entryCBcalled = false;
	cbtest->doCBcalled = false;
	cbtest->eventCBcalled = false;
	cbtest->exitCBcalled = false;
	cbtest->transitionCBcalled = false;
}

void entryCallback(void* param) {
	CBtest* cbtest = (CBtest*) param;
	cbtest->entryCBcalled = true;
}

void doCallback(void* param) {
	CBtest* cbtest = (CBtest*) param;
	cbtest->doCBcalled = true;
}

void eventCallback(void* param) {
	CBtest* cbtest = (CBtest*) param;
	cbtest->eventCBcalled = true;
}

void exitCallback(void* param) {
	CBtest* cbtest = (CBtest*) param;
	cbtest->exitCBcalled = true;
}

void transitionCallback(void* param) {
	CBtest* cbtest = (CBtest*) param;
	cbtest->transitionCBcalled = true;
}

/*
 Test function prototypes
 */
bool testBasic(bool verbose);
bool testGuards(bool verbose);
bool testTransitionEvent(bool verbose);
bool testRemoval(bool verbose);
bool testVirtual(bool verbose);

int main(int argc, char* argv[]) {
	bool verbose = true;
	bool allTestsPass = true;

	std::cout << "Testing Basic" << std::endl;
	allTestsPass = testBasic(verbose) ? allTestsPass : false;
	std::cout << "Testing Guards" << std::endl;
	allTestsPass = testGuards(verbose) ? allTestsPass : false;
	std::cout << "Testing Transition event" << std::endl;
	allTestsPass = testTransitionEvent(verbose) ? allTestsPass : false;
	std::cout << "Testing Removal" << std::endl;
	allTestsPass = testRemoval(verbose) ? allTestsPass : false;
	std::cout << "Testing Virtual" << std::endl;
	allTestsPass = testVirtual(verbose) ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testBasic(bool verbose) {
	bool allTestsPass = true;
	/*
	 A simple single state diagram with two completion transitions (no guards).
	 Calling Diagram::update() should result in being in the end state (@).
	 Also test a default guard.
	 ________________________________________________
	 |           _______________________             |
	 |          |         state         |            |
	 |     [1]  | entry/entryCallback() |            |
	 |   O ---->| do/doCallback()       |--------> @ |
	 |          | event/eventCallback() |            |
	 |          | exit/exitCallback()   |            |
	 |           -----------------------             |
	 |_______________________________________________|

	 */

	Mogi::StateChart::Diagram stateDiagram;

	std::cout << " - Testing number of states .................... ";
	if (stateDiagram.getStates().size() == 2) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	State* state = stateDiagram.addState(new State);

	std::cout << " - Testing number of states 2 .................. ";
	if (stateDiagram.getStates().size() == 3) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing start state removal ................. ";
	stateDiagram.removeState(stateDiagram.getInitialState());
	if (stateDiagram.getStates().size() == 3) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing start end removal ................... ";
	stateDiagram.removeState(stateDiagram.getFinalState());
	if (stateDiagram.getStates().size() == 3) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	CBtest cbTest;

	state->setCallbackEntry(entryCallback, &cbTest);
	state->setCallbackDo(doCallback, &cbTest);
	//	state->setCallbackEvent(eventCallback, &cbTest);
	state->setCallbackExit(exitCallback, &cbTest);

	Transition* transition = stateDiagram.getInitialState()->addTransition(
			state);
	state->addTransition(stateDiagram.getFinalState());

	transition->addGuard(new Guard);  // Always met.

	resetFlags(&cbTest);

	std::cout << " - Testing state double add .................... ";
	if (state == stateDiagram.addState(state)) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing simple state transition ............. ";
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == stateDiagram.getFinalState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls ...................... ";

	if (cbTest.entryCBcalled == true && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == true) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls 2 .................... ";
	resetFlags(&cbTest);
	stateDiagram.update();
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testGuards(bool verbose) {
	bool allTestsPass = true;

	/*
	 A simple single state diagram with one guard.
	 Calling Diagram::update() should result in being in the use state.
	 Calling Diagram::update() after setting passGuard=true should result
	 in being in the end state.
	 While in the state, event action will be tested
	 We will also test the transition callback.
	 ________________________________________________________________________
	 |           _______________________                                      |
	 |          |         state         |                                     |
	 |          | entry/entryCallback() | [passGuard]/transitionCallback()    |
	 |   O ---->| do/doCallback()       |---------------------------------> @ |
	 |          | event/eventCallback() |                                     |
	 |          | exit/exitCallback()   |                                     |
	 |           -----------------------                                      |
	 |________________________________________________________________________|

	 */

	Mogi::StateChart::Diagram stateDiagram;

	State* state = stateDiagram.addState(new State);

	CBtest cbTest;
	Event event;

	state->setCallbackEntry(entryCallback, &cbTest);
	state->setCallbackDo(doCallback, &cbTest);
	state->setCallbackEvent(&event, eventCallback, &cbTest);
	event.addObserver(state); // this is already handled by the above method call

	state->setCallbackExit(exitCallback, &cbTest);

	stateDiagram.getInitialState()->addTransition(state);
	Transition* transition = state->addTransition(stateDiagram.getFinalState());
	transition->setEvent(&event);
	event.addObserver(transition);

	std::cout << " - Testing duplicate add ....................... ";
	if (event.addObserver(state) == state) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	// TODO: make a test for this magically:
	event.removeObserver(transition);

	class mGuard: public Guard {
	public:
		bool passGuard;
		mGuard() :
				passGuard(false) {
		}
		;
		bool isSatisfied() {
			return passGuard;
		}
	};
	mGuard* guard = new mGuard;
	transition->addGuard(guard);
	transition->addGuard(guard);
	transition->setCallbackAction(transitionCallback, &cbTest);
	resetFlags(&cbTest);

	std::cout << " - Testing simple guard block .................. ";
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == state) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls ...................... ";
	if (cbTest.entryCBcalled == true && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state event action .................. ";
	resetFlags(&cbTest);
	event.trigger();
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == true && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state event action .................. ";
	resetFlags(&cbTest);
	stateDiagram.update();
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing simple guard pass transition ........ ";
	guard->passGuard = true;
	resetFlags(&cbTest);
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == stateDiagram.getFinalState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing transition action callback .......... ";
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == true
			&& cbTest.transitionCBcalled == true) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state event action, wrong state ..... ";
	resetFlags(&cbTest);
	event.trigger();
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing starting state reset ................ ";
	stateDiagram.reset();

	if (stateDiagram.getCurrentState() == stateDiagram.getInitialState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing simple guard block .................. ";
	resetFlags(&cbTest);
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == stateDiagram.getFinalState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls ...................... ";
	if (cbTest.entryCBcalled == true && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == true
			&& cbTest.transitionCBcalled == true) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testTransitionEvent(bool verbose) {
	bool allTestsPass = true;

	/*
	 A simple single state diagram with one guard.
	 Calling Diagram::update() should result in being in the use state.
	 Calling Diagram::update() after setting passGuard=true should result
	 in being in the end state.
	 We will also test the transition callback.
	 _______________________________________________________________________________
	 |           _______________________ |
	 |          |         state         | |
	 |      e2  | entry/entryCallback() | event[passGuard]/transitionCallback() |
	 |   O ---->| do/doCallback()       |--------------------------------------> @
	 |
	 |          | event/eventCallback() | |
	 |          | exit/exitCallback()   | |
	 |           ----------------------- |
	 |______________________________________________________________________________|

	 */

	Mogi::StateChart::Diagram stateDiagram;

	State* state = stateDiagram.addState(new State);

	CBtest cbTest;

	state->setCallbackEntry(entryCallback, &cbTest);
	state->setCallbackDo(doCallback, &cbTest);
	// state->setCallbackEvent(eventCallback, &cbTest );
	state->setCallbackExit(exitCallback, &cbTest);

	Transition* transition = stateDiagram.getInitialState()->addTransition(
			state);
	Event e2;
	transition->setEvent(&e2);
	transition = state->addTransition(stateDiagram.getFinalState());

	class mGuard: public Guard {
	public:
		bool passGuard;
		mGuard() :
				passGuard(false) {
		}
		;
		bool isSatisfied() {
			return passGuard;
		}
	};
	mGuard* guard = new mGuard;
	transition->addGuard(guard);
	transition->addGuard(guard);  // oops, a double addded guard
	transition->setCallbackAction(transitionCallback, &cbTest);

	Event event;
	transition->setEvent(&event);

	resetFlags(&cbTest);

	std::cout << " - Testing transition with event, no guard ..... ";
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == stateDiagram.getInitialState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing above transition trigger ............ ";
	e2.trigger();

	if (stateDiagram.getCurrentState() == state) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls ...................... ";
	if (cbTest.entryCBcalled == true && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing simple event trigger blocked ........ ";
	event.trigger();

	if (stateDiagram.getCurrentState() == state) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing simple event trigger unblocked ...... ";
	guard->passGuard = true;
	resetFlags(&cbTest);
	event.trigger();

	if (stateDiagram.getCurrentState() == stateDiagram.getFinalState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing transition action callback .......... ";
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == true
			&& cbTest.transitionCBcalled == true) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing transition event, different state ... ";
	resetFlags(&cbTest);
	event.trigger();

	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false
			&& cbTest.transitionCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testRemoval(bool verbose) {
	bool allTestsPass = true;
	/*
	 Here a bad state machine will be constructed, then pieces will be removed to
	 correct the state machine into something that fits previous examples.
	 _______________________________________________
	 |           _______________________             |
	 |          |         state         |            |
	 |     [1]  | entry/entryCallback() |            |
	 |   O ---->| do/doCallback()       |--------> @ |
	 |   |      | event/eventCallback() |          | |
	 |   |      | exit/exitCallback()   |<-+       | |
	 |   |       -----------------------   |      _| |
	 |   |       _______________________   |     |   |
	 |   |      |         state2        |--+     |   |
	 |   |      | entry/                |   [1]  |   |
	 |    ----->| do/                   |--------    |
	 |          | event/                |            |
	 |          | exit/                 |            |
	 |           -----------------------             |
	 |_______________________________________________|

	 becomes:
	 _______________________________________________
	 |           _______________________             |
	 |          |         state         |            |
	 |     [1]  | entry/entryCallback() |            |
	 |   O ---->| do/doCallback()       |--------> @ |
	 |   |      | event/eventCallback() |          | |
	 |   |      | exit/exitCallback()   |          | |
	 |   |       -----------------------          _| |
	 |   |       _______________________         |   |
	 |   |      |         state2        |        |   |
	 |   |      | entry/                |   [1]  |   |
	 |    ----->| do/                   |--------    |
	 |          | event/                |            |
	 |          | exit/                 |            |
	 |           -----------------------             |
	 |_______________________________________________|

	 then becomes:
	 _______________________________________________
	 |           _______________________             |
	 |          |         state         |            |
	 |     [1]  | entry/entryCallback() |            |
	 |   O ---->| do/doCallback()       |--------> @ |
	 |          | event/eventCallback() |            |
	 |          | exit/exitCallback()   |            |
	 |           -----------------------             |
	 |_______________________________________________|

	 */

	Mogi::StateChart::Diagram stateDiagram;

	std::cout << " - Testing number of states .................... ";
	if (stateDiagram.getStates().size() == 2) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	State* state = stateDiagram.addState(new State);

	std::cout << " - Testing number of states 2 .................. ";
	if (stateDiagram.getStates().size() == 3) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	CBtest cbTest;

	state->setCallbackEntry(entryCallback, &cbTest);
	state->setCallbackDo(doCallback, &cbTest);
	// state->setCallbackEvent(eventCallback, &cbTest );
	state->setCallbackExit(exitCallback, &cbTest);

	Transition* transition = stateDiagram.getInitialState()->addTransition(
			state);
	state->addTransition(stateDiagram.getFinalState());

	Guard* dumbGuard = new Guard;
	transition->addGuard(dumbGuard);  // Always met.

	State* state2 = stateDiagram.addState(new State);
	stateDiagram.getInitialState()->addTransition(state2);
	transition = state2->addTransition(stateDiagram.getFinalState());
	transition->addGuard(dumbGuard);  // Duplicate guard on different transition

	std::cout << " - Testing number of states 3 .................. ";
	if (stateDiagram.getStates().size() == 4) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state double add .................... ";
	if (state == stateDiagram.addState(state)) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	Transition* transition2 = state2->addTransition(state);
	state2->removeTransition(transition2);  // TODO: make this a test:
	//	std::cout << " - Testing transition removal .................. ";
	//	if (state == stateDiagram.addState(state)) {
	//		std::cout << "PASS" << std::endl;
	//	} else {
	//		std::cout << "FAILED" << std::endl;
	//		allTestsPass = false;
	//	}

	stateDiagram.removeState(state2);

	std::cout << " - Testing number of states 3 .................. ";
	if (stateDiagram.getStates().size() == 3) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	resetFlags(&cbTest);

	std::cout << " - Testing simple state transition ............. ";
	stateDiagram.update();

	if (stateDiagram.getCurrentState() == stateDiagram.getFinalState()) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls ...................... ";

	if (cbTest.entryCBcalled == true && cbTest.doCBcalled == true
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == true) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing callback calls 2 .................... ";
	resetFlags(&cbTest);
	stateDiagram.update();
	if (cbTest.entryCBcalled == false && cbTest.doCBcalled == false
			&& cbTest.eventCBcalled == false && cbTest.exitCBcalled == false) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testVirtual(bool verbose) {
	bool allTestsPass = true;
	/*
	 A slightly more complex state model.  This example implements callbacks to a
	 function, and a derived State class.
	 ____________________________________________________________________________________________
	 |           _______________________            _______________________ |
	 |          |          Idle         |          |        state1         | |
	 |   O ---->| entry/                |  begin   | entry/                |
	 [isGood]/success()   |
	 |          | do/                   |--------->| do/ |------------------> @ |
	 |       +->| event/doThis()        |          | event/doThis()        | |
	 |       |  | exit/                 |          | exit/                 |--+ |
	 |       |   -----------------------            -----------------------   | |
	 |       |   _______________________            _______________________
	 |[!isGood]/fail()   |
	 |       |  |         state3        |          |         state2        |  | |
	 |       |  | entry/                |  reset   | entry/                |  | |
	 |       +--| do/                   |<---------| do/                   |<-+ |
	 |          | event/doThis()        |          | event/doThis()        | |
	 |          |                       |          | event2/doThat()       | |
	 |          | exit/                 |          | exit/                 | |
	 |           -----------------------            ----------------------- |
	 |____________________________________________________________________________________________|
	 */

	class myState: public State {
	private:
		static void callback(void* param) {
			myState* mState = (myState*) param;
			mState->DesiredCallback();
		}

		void DesiredCallback() {
			desiredCallbackCalled = true;
		}

	public:
		std::string label;
		//		int doThisCount;
		bool desiredCallbackCalled;

		myState() :
				desiredCallbackCalled(false) {
			setCallbackEntry(this->callback, this);
		}
		;
		//		void actionEvent( Event* event ) { doThis(); };

		void actionEntry() {
			//			std::cerr << "entry for " << label << std::endl;
		}
	};

	//	class myGuard : public Guard {
	//	public:
	//		bool isSatisfied() { return isGood; }
	//	};
	//	class myNotGuard : public Guard {
	//	public:
	//		bool isSatisfied() { return !isGood; }
	//	};

	class NormalClass {
	private:
		static void doThis(void* param) {
			((NormalClass*) param)->thisCount++;
		}
		;
		static void doThat(void* param) {
			((NormalClass*) param)->thatCount++;
		}
		;

		// Action callbacks:
		static void success(void* param) {
			((NormalClass*) param)->success();
		}
		;
		void success() {
			//			std::cout << "success()" << std::endl;
			successVal = true;
		}

		static void fail(void* param) {
			((NormalClass*) param)->fail();
		}
		;
		void fail() {
			//			std::cout << "fail()" << std::endl;
			failVal = true;
		}

		static bool guardIsGood(void* param) {
			//			std::cout << "guardIsgGood()" << std::endl;
			return ((NormalClass*) param)->isGood;
		}

		static bool guardIsNotGood(void* param) {
			//			std::cout << "guardIsNotGood()" << std::endl;
			return !((NormalClass*) param)->isGood;
		}

	public:
		int thisCount;
		int thatCount;
		bool successVal;
		bool failVal;
		bool isGood;

		Diagram stateDiagram;

		Event resetEvent;
		Event beginEvent;
		Event stateEvent;
		Event stateEvent2;

		Guard* g1;
		Guard* g2;

		myState* Idle;
		myState* state1;
		myState* state2;
		myState* state3;

		testStateChangeObserver mstatechange;

		NormalClass() :
				thisCount(0), thatCount(0), successVal(false), failVal(false), isGood(
						false) {
			stateDiagram.addObserver(&mstatechange);

			Idle = (myState*) stateDiagram.addState(new myState);
			state1 = (myState*) stateDiagram.addState(new myState);
			state2 = (myState*) stateDiagram.addState(new myState);
			state3 = (myState*) stateDiagram.addState(new myState);

			Idle->label = "Idle";
			state1->label = "state1";
			state2->label = "state2";
			state3->label = "state3";

			Idle->setCallbackEvent(&stateEvent, doThis, this);
			state1->setCallbackEvent(&stateEvent, doThis, this);
			state2->setCallbackEvent(&stateEvent, doThat, this); // correction example
			state2->setCallbackEvent(&stateEvent, doThis, this);
			state2->setCallbackEvent(&stateEvent2, doThis, this); // correction example
			state2->setCallbackEvent(&stateEvent2, doThat, this);
			state3->setCallbackEvent(&stateEvent, doThis, this);

			Transition* transition =
					stateDiagram.getInitialState()->addTransition(Idle);

			transition = Idle->addTransition(state1);
			transition->setEvent(&beginEvent);

			transition = state1->addTransition(stateDiagram.getFinalState());

			g1 = new Guard;
			g1->setGuardCallback(guardIsGood, this);
			transition->addGuard(g1);
			transition->setCallbackAction(success, this);
			transition = state1->addTransition(state2);

			g2 = new Guard;
			g2->setGuardCallback(guardIsNotGood, this);
			transition->addGuard(g2);
			transition->setCallbackAction(fail, this);

			transition = state2->addTransition(state3);
			transition->setEvent(&resetEvent);

			transition = state3->addTransition(Idle);
		}
		~NormalClass() {
			removeStateObserver();
		}

		void removeStateObserver() {
			stateDiagram.removeObserver(&mstatechange);
		}

		void update() {
			stateDiagram.update();
		}
		void begin() {
			failVal = false;
			successVal = false;
			beginEvent.trigger();
		}
		void reset() {
			resetEvent.trigger();
		}
		void stateNotifyEvent() {
			stateEvent.trigger();
		}
	};

	// Now let's have fun!
	NormalClass testClass;

	std::cout << " - Testing Idle state .......................... ";
	testClass.update();

	if (testClass.stateDiagram.getCurrentState() == testClass.Idle) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing Idle->state1->state2 ................ ";
	testClass.mstatechange.notificationCount = 0;
	testClass.begin();

	if (testClass.stateDiagram.getCurrentState() == testClass.state2
			&& testClass.failVal == true && testClass.successVal == false
			&& testClass.mstatechange.notificationCount == 2) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing event state2 action ................. ";
	testClass.stateNotifyEvent();
	testClass.stateNotifyEvent();
	testClass.stateEvent2.trigger();

	if (testClass.thisCount == 2 && testClass.thatCount == 1
			&& testClass.mstatechange.notificationCount == 2) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state2->state3->Idle ................ ";
	testClass.reset();

	if (testClass.stateDiagram.getCurrentState() == testClass.Idle
			&& testClass.mstatechange.notificationCount == 4) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		std::cout << "testClass.mstatechange.notificationCount:"
				<< testClass.mstatechange.notificationCount << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing event Idle action ................... ";
	testClass.stateNotifyEvent();
	testClass.stateNotifyEvent();
	testClass.stateEvent2.trigger();

	if (testClass.thisCount == 4 && testClass.thatCount == 1
			&& testClass.mstatechange.notificationCount == 4) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing Idle->state1->state2 2 .............. ";
	testClass.begin();

	if (testClass.stateDiagram.getCurrentState() == testClass.state2
			&& testClass.failVal == true && testClass.successVal == false
			&& testClass.mstatechange.notificationCount == 6) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing event state2 action ................. ";
	testClass.stateNotifyEvent();
	testClass.stateNotifyEvent();
	testClass.stateEvent2.trigger();

	if (testClass.thisCount == 6 && testClass.thatCount == 2
			&& testClass.mstatechange.notificationCount == 6) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing state2->state3->Idle 2 .............. ";
	testClass.reset();

	if (testClass.stateDiagram.getCurrentState() == testClass.Idle
			&& testClass.mstatechange.notificationCount == 8) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	std::cout << " - Testing Idle->state1->end ................... ";
	testClass.removeStateObserver();
	testClass.isGood = true;
	testClass.begin();

	if (testClass.stateDiagram.getCurrentState()
			== testClass.stateDiagram.getFinalState()
			&& testClass.failVal == false && testClass.successVal == true
			&& testClass.mstatechange.notificationCount == 8) {
		std::cout << "PASS" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		std::cout << "testClass.mstatechange.notificationCount:"
				<< testClass.mstatechange.notificationCount << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}
