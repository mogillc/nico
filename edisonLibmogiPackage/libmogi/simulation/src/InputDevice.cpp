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

#include "inputDevice.h"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

Keyboard::Keyboard() {
	for (int i = 0; i < 256; i++) {
		key_state[i] = false;
		key_state_previous[i] = false;
	}
}

void Keyboard::processEvents() {
	beginEvents();
}

void Keyboard::setKeyToFunction(unsigned char key, Function* function) {
	MBevent* newEvent = new MBevent;
	newEvent->setFunction(function);
	pressedEvents[key] = newEvent;
	// eventMapping[key] = newEvent;
}
#ifdef SDL2_FOUND
void Keyboard::handleEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_KEYDOWN:
		keyboardCB(event.key.keysym.sym);
		break;

	case SDL_KEYUP:
		keyboardUpCB(event.key.keysym.sym);
		break;
	}
}
#endif

void Keyboard::beginEvents() {
	for (int i = 0; i < 256; i++) {
		key_state_previous[i] = key_state[i];
	}
}

void Keyboard::keyboardCB(unsigned char key) {
	key_state_previous[key] = key_state[key];
	key_state[key] = true;
	MBevent* keyEvent = pressedEvents[key];
	if (keyEvent) {
		std::cout << "Key " << key << " was pressed and has an event!"
				<< std::endl;
		keyEvent->trigger();
	}
	if (!key_state_previous[key] && key_state[key]) {
		keyEvent = risingEdgeEvents[key];
		if (keyEvent) {
			std::cout << "Rising Key " << key << " has an event!" << std::endl;
		}
	}
}

void Keyboard::keyboardUpCB(unsigned char key) {
	key_state_previous[key] = key_state[key];
	key_state[key] = false;
	MBevent* keyEvent = unpressedEvents[key];
	if (keyEvent) {
		std::cout << "Key " << key << " was pressed and has an event!"
				<< std::endl;
	}
	if (key_state_previous[key] && !key_state[key]) {
		keyEvent = fallingEdgeEvents[key];
		if (keyEvent) {
			std::cout << "Falling Key " << key << " has an event!" << std::endl;
		}
	}
}

bool Keyboard::risingEdge(unsigned char key) {
	return !key_state_previous[key] && key_state[key];
}

bool Keyboard::fallingEdge(unsigned char key) {
	return key_state_previous[key] && !key_state[key];
}

bool Keyboard::keyPressed(unsigned char key) {
	return key_state[key];
}

void MBmouse::processEvents() {
}

	#ifdef SDL2_FOUND
void MBmouse::handleEvent(const SDL_Event& event) {
	switch (event.type) {
	case SDL_MOUSEBUTTONDOWN:
		break;

	case SDL_MOUSEBUTTONUP:
		break;

	case SDL_MOUSEWHEEL:
		break;

	case SDL_MOUSEMOTION:
		break;

	default:
		break;
	}
}
#endif

void MBmouse::beginEvents() {
}

#ifdef _cplusplus
}
#endif
