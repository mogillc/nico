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

#include "dynamixel.h"

#include <unistd.h>
//#include <iostream>

#ifdef IDENT_C
static const char* const Dynamixel_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Dynamixel;

Handler::Handler() :
		interface(NULL), isBusy(false) {
	pthread_mutex_init(&vectorMutex, NULL);
}

//Handler& Handler::operator=(const Handler&) {
//	interface = NULL;
//	return *this;
//}
//
//Handler::Handler(const Handler&) {
//	interface = NULL;
//}

Handler::~Handler() {
	while (busy())
		;  // ake sure we are at the end of the task loop before performing a
		   // resume()

	stop();

	lock();

	for (std::map<unsigned char, Motor*>::iterator it = dynamixelList.begin();
			it != dynamixelList.end(); it++) {
		delete it->second;
	}
	dynamixelList.clear();
	for (std::vector<Instruction*>::iterator it = instructionQueue.begin();
			it != instructionQueue.end(); it++) {
		delete (Instruction*) *it;
	}
	instructionQueue.clear();
	if (interface != NULL) {
		delete interface;
	}
	unlock();
	pthread_mutex_destroy(&vectorMutex);
}

Interface::Status Handler::openInterface(int interfaceType, int baudrate,
		const char* field) {
	baud = baudrate;

	interface = Interface::create(interfaceType, baud);
	Interface::Status ret;
	if ((ret = interface->open(field)) == Interface::NOERROR) {
		if (!start()) {
			ret = Interface::DISCONNECTED; // TODO: make this a more appropriate type
		}
	} else {
		// std::cerr << "StartInternalThread() returned: " << StartInternalThread()
		// << std::endl;
		// std::cerr << "Error: Handler::openInterface() interface->open() failed"
		// << std::endl;
	}

	return ret;
}

void Handler::setInterface(Interface* newInterface) {
	//		kill();
	WaitForInternalThreadToExit();

	if (interface != NULL) {
		interface->close();
		delete interface;
		interface = NULL;
	}

	interface = newInterface;
	start();  // TODO: check the return and handle properly.
}

std::map<unsigned char, Motor*> Handler::getDynamixels() {
	return dynamixelList;
}

void Handler::addDynamixel(unsigned int ID) {
	Motor* dynamixel;
	for (std::map<unsigned char, Motor*>::iterator it = dynamixelList.begin();
			it != dynamixelList.end(); it++) {
		dynamixel = it->second;
		if (ID == dynamixel->getByte(REG_ID)) {
			// error
			//#ifdef DEBUG
			//					std::cout << "Error: Handler: ID
			//" << ID <<  " has already been added!" << std::endl;
			//#endif
			return;
		}
	}

	class SuccessfulObserver: public Observer {
	public:
		Handler* dynamixelHandler;
		SuccessfulObserver(Handler* dynamixelHandler) :
				dynamixelHandler(dynamixelHandler) {
		}
		~SuccessfulObserver() {
		}
		;

		void update(Motor* dynamixel, const Instruction* instruction) {
			if (instruction->type == Instruction::PING) {
				// Step 1 complete, check for a successful ping response
				// Step 2: Read the model information:
				dynamixelHandler->dynamixelList[dynamixel->getByte(REG_ID)] =
						dynamixel;
				dynamixelHandler->pushInstruction(
						new Instruction(Instruction::READ, DataRange(0, 2),
								dynamixel));  // read the model information
			} else if (instruction->type == Instruction::READ
					&& instruction->range.reg == 0
					&& instruction->range.length == 2) {
				// Step 3: With the known model information, update the rest of the
				// state
				dynamixelHandler->pushInstruction(
						new Instruction(Instruction::READ,
								DataRange(2,
										dynamixel->getNumberOfRegisters() - 2),
								dynamixel));  // read the rest of the info
				dynamixel->removeObserver(this);
				delete this;
			}
		}

		void failed(Motor* dynamixel, const Instruction* instruction,
				Interface::Status status) {
			dynamixel->removeObserver(this);
			delete dynamixel;
			delete this;
		}
	};

	// std::cerr << "Allocating dynamixel of ID " << ID << std::endl;
	dynamixel = new Motor(ID);

	// std::cerr << "Adding an observer to motor ID " << ID << std::endl;
	dynamixel->addObserver(new SuccessfulObserver(this));

	// std::cerr << "Sending a ping command for motor of ID " << ID << std::endl;
	pushInstruction(
			new Instruction(Instruction::PING, DataRange(0, 0), dynamixel));

	////		dynamixel->assignInterface( interface );
	////		interface->ping(ID)
	//		if(interface->ping(dynamixel) == Interface::NOERROR) {
	//			this->
	//
	//		} else {
	////#ifdef DEBUG
	////					std::cout << "Error: Handler: ID " << dynamixel->ID <<  "
	///could not be loaded! :(" << std::endl;
	////#endif
	//			delete dynamixel;
	//		}
}

int Handler::queueLength() {
	return instructionQueue.size();
}

void Handler::forceAddDynamixel(unsigned int ID, Model model) {
	Motor* dynamixel = new Motor(ID, model);
	//		dynamixel->assignInterface( interface );
	//		dynamixelList.push_back(dynamixel);
	dynamixelList[ID] = dynamixel;
}

void Handler::pushInstruction(Instruction* instruction) {
	isBusy = true;
	// std::cerr << " locking for pushing...: " << std::endl;
	// lock();
	pthread_mutex_lock(&vectorMutex);
	instructionQueue.push_back(instruction);
	// std::cerr << " Pushed back, new size: " << instructionQueue.size() <<
	// std::endl;
	resume();
	// unlock();
	pthread_mutex_unlock(&vectorMutex);
}
void Handler::entryAction() {
	lock();
}

//	void Handler::InternalThreadEntry() {
//		//std::cout << "Thread began!" << std::endl;
//		lock();
//		terminate = false;
//		while (!terminate) {
void Handler::doAction() {
	// std::cout << "Handler::doAction()" << std::endl;

	while (instructionQueue.size() > 0) {
		pthread_mutex_lock(&vectorMutex);
		Instruction instruction = **instructionQueue.begin();
		delete *instructionQueue.begin();
		instructionQueue.erase(instructionQueue.begin());
		pthread_mutex_unlock(&vectorMutex);

		// std::cout << "Processing instruction " << instruction.type << " reg:" <<
		// instruction.startRegister << " num:" << instruction.numBytes <<
		// std::endl;

		Interface::Status status;

		instruction.dynamixelList = &dynamixelList;
		status = interface->processInstruction(instruction);
	}

	if (instructionQueue.size() == 0) {
		isBusy = false;
	}

	pause();  // pause this until a new command comes in
}
void Handler::exitAction() {
	unlock();
}

bool Handler::busy() {
	return isBusy && interface->isOpen();
}

#ifdef _cplusplus
}
#endif
