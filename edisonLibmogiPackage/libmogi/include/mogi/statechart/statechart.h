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

#ifndef MOGI_STATECHART_H
#define MOGI_STATECHART_H

#include <map>
#include <string>
#include <vector>
//#include <cstddef>	// for NULL

void __doNothing(void*);
bool __returnTrue(void*);

namespace Mogi {

/**
 * @namespace Mogi::StateChart
 * \brief An implementation of UML style state models.
 */
namespace StateChart {

/*! \typedef actionCallback
 \brief Callback function type for performing actions.
 */
typedef void (*actionCallback)(void*);

/*! \typedef guardCallback
 \brief Callback function type for performing guard checking.
 */
typedef bool (*guardCallback)(void*);

/*!
 \struct CallbackAction
 \brief Management of a callback function and the parameter to be provided.
 */
struct CallbackAction {

	/*! The callback function.
	 */
	actionCallback cb;

	/*! The parameter for the callback function.
	 */
	void* parameter;

	CallbackAction() :
			cb(__doNothing), parameter(NULL) {
	}
	;
};

/*!
 \struct CallbackGuard
 \brief Management of a callback function for guards and the parameter to be provided.
 */
struct CallbackGuard {

	/*! The callback function.
	 */
	guardCallback cb;

	/*! The parameter for the callback function.
	 */
	void* parameter;

	CallbackGuard() :
			cb(__returnTrue), parameter(NULL) {
	}
	;
};

class State;
class CompositeState;
class StateChangeObserver;
class Transition;
class Event;
class Guard;
class Event;
class EventObserver;

/*!
 @class Diagram
 \brief The entire state diagram structure.

 This is used to build and contain the set of states and transitions.  Upon
 construction, an empty initial and final state are created.  The user should
 create a completion transition (guardless) to connect from the initial state to
 their desired initial state.  The final state does not need to be implemented,
 but may be used as a terminator.
 During building, externally added States will be stored in an internal vector
 and will be destroyed by this class.
 @since 11-01-2015
 */
class Diagram {
	friend class State;
	friend class CompositeState;
	friend class Transition;

private:
	typedef enum {
		TRANSITION, STATE_ENTRY, STATE_DO,
	} DiagramState;

	std::string label;

	std::vector<State*> states;
	//			std::vector<Transition*> transitions;
	std::vector<Guard*> guards;

	CompositeState* parent;

	void* _mutex;

	State* currentState;
	State* initial;
	State* final;
	Transition* currentTransition;

	DiagramState processState;

	std::vector<StateChangeObserver*> stateChangeObservers;

	void addGuard(Guard*);

	bool perform(Transition*);
	bool performGettingMessy(Transition*);
	void
	process();  // should be called to update and states and perform any actions
	void notifyObservers(State* state);

public:
	~Diagram();

	/*!
	 \brief Default constructor with optional label definition.
	 \param name The optional name of the diagram.
	 */
	Diagram(const std::string& name = "");

	/*!
	 \brief Returns the provided name.
	 @return The name provided during construction.
	 */
	const std::string& name();

	/*!
	 \brief Adds a state to the diagram.

	 This will result in a containment by composition, so you should never delete
	 (or free) the state after calling this.
	 @param state The state to be added to the diagram.
	 @return The same state that was just added.
	 */
	State* addState(State* state);

	/*!
	 \brief Removes a state from the diagram.

	 Due to the containment by composition, this state will be deleted.
	 @param state The state to be removed from the diagram.
	 */
	void removeState(State* state);

	/*!
	 \brief Gets the current state.
	 @return The currently active state.
	 */
	State* getCurrentState();

	/*!
	 \brief Gets the initial state.
	 @return The initial state.
	 */
	State* getInitialState();

	/*!
	 \brief Gets the final state.
	 @return The final state.
	 */
	State* getFinalState();

	/*!
	 \brief Gets the vector of all states in the diagram.
	 @return The vector of states.
	 */
	const std::vector<State*>& getStates();

	/*!
	 \brief Updates the current state and performs actions based on the state.

	 This should be called regularly to perform checks on the guards
	 perform state transitions.  Ideally this would be run in a thread to
	 automatically perform State Do actions and check guards of the transitions on
	 the current state.  The only other way for transitions to occur is to signal
	 transitions from Events if they have been set up.
	 */
	void update();

	/*!
	 \brief Sets the current state back to the initial state.
	 */
	void reset();

	/*!
	 \brief Adds an observer to be notified when in a new state.
	 @param observer The observer to be notified.
	 @return The same observer you gave it (why??)
	 */
	void addObserver(StateChangeObserver* observer);  // by aggregation

	/*!
	 \brief Removes an observer to be notified.
	 @param observer The observer to no longer be notified.
	 */
	void removeObserver(StateChangeObserver* observer);
};

/*!
 @class StateChangeObserver
 \brief Used for state change notifications in the Diagram.
 @since 11-17-2015
 */
class StateChangeObserver {
	friend class Diagram;

protected:
	/*!
	 \brief Called when there is a new state, after the state's actionEnty has
	 method called.
	 @param diagram The current Diagram with the new state.
	 */
	virtual void notify(Diagram* diagram) = 0;

public:
	virtual ~StateChangeObserver();
};

/*!
 @class Event
 \brief The event signal for transitions and actions.

 This is used to send an event signal to either a State or Transition.  If sent
 to a State, the State::doAction() method is called only when the Diagram has it
 set as the current state.  If sent to a Transition, the transition is performed
 only when the parent State is current AND all guards are satisfied.
 @since 11-01-2015
 */
class Event {  // many transitions or states may have the same event.
private:
	std::vector<EventObserver*> observers;

public:
	/*!
	 \brief Sends the event signal to all observers.
	 */
	void trigger();

	/*!
	 \brief Adds an observer to be notified.
	 @param observer The Transition or State to be notified.
	 @return The same observer you gave it (why??)
	 */
	EventObserver* addObserver(EventObserver* observer);  // by aggregation

	/*!
	 \brief Removes an observer to be notified.
	 @param observer The Transition or State to no longer be notified.
	 */
	void removeObserver(EventObserver* observer);
};

/*!
 @class EventObserver
 \brief Abstract observer used only for State and Transition.
 @since 11-01-2015
 */
class EventObserver {
	friend class Event;

private:
	int numberOfEvents;

protected:
	/*!
	 \brief Called from an Event trigger().
	 @param event The event that cause the notification.
	 */
	virtual void notify(Event* event) = 0;

public:
	EventObserver();
	virtual ~EventObserver() = 0;

	/*!
	 \brief Checks to see if one or more events notify this observer.
	 @return True if an event notifies this observer, false otherwise.
	 */
	bool hasEvent();
};

/*!
 @class State
 \brief The representation and configuration of a UML state.

 This is used to handle all actions and transitions of of a UML style state.
 Each state contains the set of transitions to other states.  This class may be
 used either on it's own by configuring callback functions, or may be used in an
 inherited class to override and define the actions.
 @since 11-01-2015
 */
class State: public EventObserver {  // event observer for event action
	friend class Diagram;
	friend class Transition;

private:
	Diagram* parent;

	std::vector<Transition*> outgoingTransitions;  // may help organize things?

	CallbackAction _entry;
	CallbackAction _do;
	std::map<Event*, CallbackAction*> _events;
	CallbackAction _exit;

	/*!
	 \brief Event notification.
	 @param event The event that cause the notification.
	 */
	void notify(Event* event);

protected:
	/*! The state's name.
	 */
	std::string label;

	/*!
	 \brief Called when the state becomes the current state in the Diagram.
	 */
	virtual void actionEntry();
	/*!
	 \brief Called on each call Diagram::process() when this state is current in
	 the Diagram.
	 */
	virtual void actionDo();
	/*!
	 \brief Called on each occurrence of an Event when this state is current in the
	 Diagram.
	 */
	virtual void actionEvent(Event* event);
	/*!
	 \brief Called just before transition out of this state, when current in the
	 Diagram.
	 */
	virtual void actionExit();

public:
	/*!
	 \brief Constructor with optional name.
	 \param label State name.
	 */
	State(std::string label = "");
	virtual ~State();

	/*!
	 \brief Sets the callback function for actionEntry().
	 @param cb The callback function to be called.
	 @param parameter The object to be sent to the callback function.
	 */
	void setCallbackEntry(actionCallback cb, void* parameter);
	/*!
	 \brief Sets the callback function for actionDo().
	 @param cb The callback function to be called.
	 @param parameter The object to be sent to the callback function.
	 */
	void setCallbackDo(actionCallback cb, void* parameter);
	/*!
	 \brief Sets the callback function for actionEvent().
	 @param event The event that when triggered will perform the callback when
	 this is the current state.
	 @param cb The callback function to be called.
	 @param parameter The object to be sent to the callback function.
	 */
	void setCallbackEvent(Event* event, actionCallback cb, void* parameter);
	/*!
	 \brief Sets the callback function for actionExit().
	 @param cb The callback function to be called.
	 @param parameter The object to be sent to the callback function.
	 */
	void setCallbackExit(actionCallback cb, void* parameter);

	/*!
	 \brief Returns the set of outgoing transitions.
	 @return The outgoing transitions associated with this state.
	 */
	std::vector<Transition*> getOutgoingTransitions();

	/*!
	 \brief Creates a new Transition to another state.
	 @param dst The State to transition to.
	 @return The newly created Transition.
	 */
	Transition* addTransition(State* dst);

	/*!
	 \brief Removes a Transition from this state.
	 @param transition The Transition to be removed.
	 */
	void removeTransition(Transition* transition);

	/*!
	 \brief Returns the name provided during construction,
	 @return The state label.
	 */
	virtual std::string name();
};

/*!
 @class CompositeState
 \brief The representation and configuration of a UML composite state.

 The composite state contains a sub diagram to help manage more complex state
 representations.
 @since 11-09-2015
 */
class CompositeState: public State {  // event observer for event action
	friend class Diagram;
	friend class Transition;

private:
	Diagram subDiagram;

protected:
	/*!
	 \brief Overridden to reset the diagram.
	 */
	void actionEntry();

	/*!
	 \brief Overridden to perform sub-diagram update.
	 */
	void actionDo();

public:
	/*!
	 \brief Constructor with optional name.
	 @param label State name.
	 */
	CompositeState(std::string label = "");
	~CompositeState();

	/*!
	 \brief Returns the state's sub diagram.
	 @return The sub diagram.
	 */
	Diagram* getDiagram();

	/*!
	 \brief Returns the name provided during construction,
	 @return The state label.
	 */
	std::string name();
};

/*!
 @class Transition
 \brief The representation and configuration of a UML transition.

 This is only creatable by calling State::addTransition() to help ensure a
 properly connected diagram.  This follows the UML style of transition by having
 configurable guards, events, and an action.  If no guard (future event?) is set
 then this will be treated like a completion transition.
 @since 11-01-2015
 */
class Transition: public EventObserver { // event observer for transition performance.
	friend class Diagram;
	friend class State;

private:
	const State* parent;  // src
	State* dst;

	CallbackAction _action;

	std::vector<Guard*> guards;  // guards to check, not memory manage

	Transition(State* src, State* dst);

	/*!
	 \brief Event notification.
	 @param event The event that cause the notification.
	 */
	void notify(Event* event);

	/*!
	 \brief Checks all guards, and returns the ANDed output of all guards.
	 @return Returns the ANDed output of all guards.
	 */
	bool guardsSatisfied();

protected:
	/*!
	 \brief Called when the transition is being performed.
	 */
	void action();

public:
	~Transition();

	/*!
	 \brief Appends a Guard to the transition.

	 This transition is blocked until all guards have been satisfied.  The guard
	 is then contained by composition, so do not free the Guard from memory.
	 @param guard The new Guard to be checked before performing the transition.
	 @return The guard that was just added.
	 */
	Guard* addGuard(Guard* guard);

	/*!
	 \brief Sets the callback function to perform upon transition.
	 @param cb The callback function to be called
	 @param parameter The object to be sent to the callback function.
	 */
	void setCallbackAction(actionCallback cb, void* parameter);

	/*!
	 \brief Sets the event that performs this transition when guards have been
	 satisfied.
	 @param event The event that performs calls transition.
	 */
	void setEvent(Event* event);

	/*!
	 \brief Gets the guards that are used for this transition.
	 @return The set of guards.
	 */
	std::vector<Guard*> getGuards();
};

/*!
 @class Guard
 \brief A representation of a guard in UML.
 @since 11-01-2015
 */
class Guard {
private:
	CallbackGuard _guard;

public:
	Guard();
	virtual ~Guard();

	/*!
	 \brief User-definable guard method.
	 @return True when the guard is satisfied, false otherwise.
	 */
	virtual bool isSatisfied();

	/*!
	 \brief Sets the callback function to perform to check if the guard is met.
	 @param cb The callback function to be called.
	 @param parameter The object to be sent to the callback function.
	 */
	void setGuardCallback(guardCallback cb, void* parameter);

	/*!
	 \brief Gets the callback function to perform to check if the guard is met.
	 @return The callback associated with this guard.
	 */
	CallbackGuard getGuardCallback();
};

}  // End StateChart namespace

}  // End Mogi namespace

#endif
