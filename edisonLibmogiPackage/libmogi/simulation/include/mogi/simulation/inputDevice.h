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

#ifndef MOGI_INPUTDEVICE_H
#define MOGI_INPUTDEVICE_H

#ifdef SDL2_FOUND
#include <SDL2/SDL.h>
#endif

#include <map>
#include <vector>

#include "event.h"
#include "mogi/math/systems.h"

namespace Mogi {
namespace Simulation {

class MBinputDevice {
public:
	std::vector<Function*> callbackFunctions;

#ifdef SDL2_FOUND
	virtual void handleEvent(const SDL_Event& event) = 0;
#endif
	virtual void beginEvents() = 0;
	virtual void processEvents() = 0;

	virtual ~MBinputDevice() {
	}
	;
};

class Keyboard: public MBinputDevice {
private:
	// std::map<int, MBevent*> eventMapping;
	std::map<unsigned char, MBevent*> pressedEvents;
	std::map<unsigned char, MBevent*> unpressedEvents;
	std::map<unsigned char, MBevent*> risingEdgeEvents;
	std::map<unsigned char, MBevent*> fallingEdgeEvents;

	bool key_state[256];
	bool key_state_previous[256];

	void keyboardCB(unsigned char key);
	void keyboardUpCB(unsigned char key);

public:
	#ifdef SDL2_FOUND
	void handleEvent(const SDL_Event& event);
#endif
	void beginEvents();
	void processEvents();

	void setEvent();

	bool risingEdge(unsigned char key);
	bool fallingEdge(unsigned char key);
	bool keyPressed(unsigned char key);

	void setKeyToFunction(unsigned char key, Function* function);

	Keyboard();
};

class MBmouse: public MBinputDevice {
public:
	#ifdef SDL2_FOUND
	void handleEvent(const SDL_Event& event);
#endif
	void beginEvents();
	void processEvents();
};
}
}

#endif
