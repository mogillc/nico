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

#ifndef libmogi_Function_h
#define libmogi_Function_h

#include <math.h>
#include <iostream>
#include <vector>

#include "ports.h"

namespace Mogi {
namespace Simulation {

class Function {  // abstract
protected:
	std::vector<MBport*> ports;

public:
	enum {
		increment, multiply, print
	};

	virtual void perform() = 0;
	virtual void setInput(MBtype*) = 0;
	virtual MBtype* getOutput() = 0;

	static Function* create(int type);
};

class MBincrementer: public Function {
public:
	void setInput(MBtype* input) {
		ports[0]->setValue(input);
	}
	;

	MBtype* getOutput() {
		return ports[0]->getValue();
	}

	MBincrementer() {
		MBport* newPort = MBport::create(MBport::input);
		ports.push_back(newPort);
	}
	;
	void perform() {
		MBtype newValue = *ports[0]->getValue();
		newValue = (double) newValue + 1.0;
		newValue = sqrt(newValue);
		ports[0]->setValue(&newValue);
		std::cout << "Value incremented: " << (double) newValue << std::endl;
	}
	;
};

class Print: public Function {
public:
	void setInput(MBtype* input) {
		ports[0]->setValue(input);
	}
	;
	MBtype* getOutput() {
		return ports[0]->getValue();
	}
	Print() {
		MBport* newPort = MBport::create(MBport::input);
		ports.push_back(newPort);
	}
	;
	void perform();
};

class MBmultiplier: public Function {
public:
	void setInput(MBtype*) {
	}
	;
	MBtype* getOutput() {
		return NULL;
	}
	;
	/*
	 MBmultiplier() {
	 MBport *newPort = MBport::create(MBport::input, MBtype::Float);
	 ports.push_back(newPort);
	 MBport::create(MBport::input, MBtype::Float);
	 ports.push_back(newPort);
	 newPort = MBport::create(MBport::output, MBtype::Float);
	 ports.push_back(newPort);
	 };

	 void perform() {
	 *(float *)ports[2]->getValue()->get() = *(float *)ports[0]->getValue()->get()
	 * *(float *)ports[1]->getValue()->get();
	 };
	 */
};
}
}

#endif
