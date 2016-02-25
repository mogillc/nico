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
#include <sstream>

#define DEBUGMODE

#include <iostream>

#define MAXTRIES 3

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Dynamixel;

Interface* Interface::create(int type, int baud) {
	Interface* interface = NULL;

	switch (type) {
			#ifdef LIBFTDI_FOUND
	case FTDI:
		interface = new Ftdi(baud);
		interface->baud = baud;
		interface->packetHandler.setType(Packet::VER_1_0);
		break;
#endif

	case COM:
		interface = new Com(baud);
		interface->baud = baud;
		interface->packetHandler.setType(Packet::VER_1_0);
		break;

	default:
		break;
	}

	return interface;
}
	Interface::Interface(int baudrate)
	: verbose(false), currentlyOpen(false) {
		baud = baudrate;
	}
Interface::~Interface() {
}

Interface::Status Interface::ping(Motor* dynamixel) {
	std::vector<unsigned char> buffer = packetHandler.ping(
			dynamixel->getByte(REG_ID));

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;
	if ((status = getResponse(&buffer)) == NOERROR) {
		if (packetHandler.getType() == Packet::VER_1_0) {
			dynamixel->status = buffer[4];
		} else {
			dynamixel->status = buffer[8];
		}
		return NOERROR;
	}

	return status;
}

Interface::StatusPacketError Interface::parseReadStatusPacket(Motor* dynamixel,
		const std::vector<unsigned char>& buffer, const DataRange& range) {
	unsigned char ID;
	if (range.reg == 0 || range.reg == 1) {  // This will change the mapping of
											 // the ID which breaks things in the
											 // Pro series
		ID = dynamixel->getByte(REG_ID);
	}
	if (packetHandler.getType() == Packet::VER_1_0) {
		dynamixel->status = buffer[4];
		for (unsigned int i = 0; i < range.length; i++) {
			dynamixel->registers[i + range.reg] = buffer[i + 5];
		}
	} else {
		dynamixel->status = buffer[8];
		for (unsigned int i = 0; i < range.length; i++) {
			dynamixel->registers[i + range.reg] = buffer[i + 9];
		}
	}
	if (range.reg == 0 || range.reg == 1) {  // This will change the mapping of
											 // the ID which breaks things in the
											 // Pro series
		dynamixel->registers[dynamixel->getMappedRegister(REG_ID)] = ID;
		dynamixel->registersDesired[dynamixel->getMappedRegister(REG_ID)] = ID;
		dynamixel->update_vals();
	}

	return STATUS_NOERROR;
}

Interface::Status Interface::read(Motor* dynamixel, DataRange range) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	std::vector<unsigned char> buffer = packetHandler.read(
			dynamixel->getByte(REG_ID), range);

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;

	if ((status = getResponse(&buffer)) == NOERROR) {
		parseReadStatusPacket(dynamixel, buffer, range);
	}

	return status;
}

Interface::Status Interface::write(Motor* dynamixel, DataRange range) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	std::vector<unsigned char> buffer = packetHandler.write(
			dynamixel->getByte(REG_ID), range,
			dynamixel->registersDesired + range.reg);

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;
	if ((status = getResponse(&buffer)) == NOERROR) {
		if (packetHandler.getType() == Packet::VER_1_0) {
			dynamixel->status = buffer[4];
		} else {
			dynamixel->status = buffer[8];
		}

		for (int i = range.reg; i < range.reg + range.length; i++) {
			dynamixel->registers[i] = dynamixel->registersDesired[i];
		}
	}

	return status;
}

Interface::Status Interface::regWrite(Motor* dynamixel, DataRange range) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	std::vector<unsigned char> buffer = packetHandler.regWrite(
			dynamixel->getByte(REG_ID), range,
			dynamixel->registersDesired + range.reg);

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;
	if ((status = getResponse(&buffer)) == NOERROR) {
		if (packetHandler.getType() == Packet::VER_1_0) {
			dynamixel->status = buffer[4];
		} else {
			dynamixel->status = buffer[8];
		}

		for (int i = range.reg; i < range.reg + range.length; i++) {
			dynamixel->registers[i] = dynamixel->registersDesired[i];
		}
	}

	return status;
}

Interface::Status Interface::action(Motor* dynamixel) {
	std::vector<unsigned char> buffer = packetHandler.action(
			dynamixel->getByte(REG_ID));

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	return NOERROR;
}

Interface::Status Interface::factoryReset(Motor* dynamixel) {
	std::vector<unsigned char> buffer = packetHandler.reset(
			dynamixel->getByte(REG_ID));

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;
	if ((status = getResponse(&buffer)) == NOERROR) {
		if (packetHandler.getType() == Packet::VER_1_0) {
			dynamixel->status = buffer[4];
		} else {
			dynamixel->status = buffer[8];
		}

		dynamixel->setByte(REG_ID, 1); // TODO: set up a better reset of all factory defaults
		dynamixel->registers[dynamixel->getMappedRegister(REG_ID)] = 1;

		return NOERROR;
	}

	return status;
}

Interface::Status Interface::reboot(Motor* dynamixel) {
	std::vector<unsigned char> buffer = packetHandler.reboot(
			dynamixel->getByte(REG_ID));

	if (writeInterface(buffer) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;
	if ((status = getResponse(&buffer)) == NOERROR) {
		if (packetHandler.getType() == Packet::VER_1_0) {
			dynamixel->status = buffer[4];
		} else {
			dynamixel->status = buffer[8];
		}
		return NOERROR;
	}

	return status;
}

Interface::Status Interface::syncRead(
		std::map<unsigned char, Motor*>* dynamixels, DataRange range) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	if (dynamixels->size() <= 0) {
		return NOERROR;
	}

	std::vector<unsigned char> buffer = packetHandler.syncRead(dynamixels, range);  // all dynamixels in 2.0 supported

	if (buffer.size() == 0) {
		return NOERROR;
	}

	int ret;
	if ((ret = writeInterface(buffer)) != buffer.size()) {
		return BAD_LENGTH;
	}
//	usleep(2000000);
	Interface::Status status = NOERROR;
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels->begin(); it != dynamixels->end(); it++) {
		Motor* dynamixel = it->second;

		Interface::Status status2;
		if ((status2 = getResponse(&buffer)) == NOERROR) {
			parseReadStatusPacket(dynamixel, buffer, range);
		} else {
			status = status2;  // this is stupid
		}
	}

	return status;
}

Interface::Status Interface::syncWrite(
		std::map<unsigned char, Motor*>* dynamixels, DataRange range) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	if (dynamixels->size() <= 0) {
		return NOERROR;
	}

	std::vector<unsigned char> buffer = packetHandler.syncWrite(dynamixels,
			range);

	if (buffer.size() == 0) {
		return NOERROR;
	}

	int ret;
	if ((ret = writeInterface(buffer)) != buffer.size()) {
		return BAD_LENGTH;
	}

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels->begin();
			it != dynamixels->end(); it++) {
		Motor* dynamixel = it->second;

		for (int i = range.reg; i < range.reg + range.length; i++) {
			dynamixel->registers[i] = dynamixel->registersDesired[i];
		}
		//			memcpy(dynamixel->registers + range.reg, dynamixel->registersDesired
		//+ range.reg, range.length);
	}

	// no response packets

	return NOERROR;
}

Interface::Status Interface::bulkRead(std::map<Motor*, DataRange>* ranges) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	if (ranges->size() <= 0) {
		return NOERROR;
	}

	std::vector<unsigned char> buffer = packetHandler.bulkRead(ranges);

	if (buffer.size() == 0) {
		return UNSUPPORTED_COMMAND;  // This is specific for a BULK READ packet
	}

	int ret;
	if ((ret = writeInterface(buffer)) != buffer.size()) {
		return BAD_LENGTH;
	}

	Interface::Status status;

	for (std::map<Motor*, DataRange>::iterator it = ranges->begin();
			it != ranges->end(); it++) {
		Motor* dynamixel = it->first;
		Interface::Status status2;
		if ((status2 = getResponse(&buffer)) == NOERROR) {
			parseReadStatusPacket(dynamixel, buffer, it->second);
		} else {
			status = status2;  // this is stupid
		}
	}

	return status;
}

Interface::Status Interface::bulkWrite(std::map<Motor*, DataRange>* ranges) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	if (ranges->size() <= 0) {
		return NOERROR;
	}

	std::vector<unsigned char> buffer = packetHandler.bulkWrite(ranges);

	if (buffer.size() == 0) {
		return UNSUPPORTED_COMMAND;
	}

	int ret;
	if ((ret = writeInterface(buffer)) != buffer.size()) {
		return BAD_LENGTH;
	}

	for (std::map<Motor*, DataRange>::iterator it = ranges->begin();
			it != ranges->end(); ++it) {
		Motor* dynamixel = it->first;
		for (int i = it->second.reg; i < it->second.reg + it->second.length;
				++i) {
			dynamixel->registers[i] = dynamixel->registersDesired[i];
		}
	}

	return NOERROR;
}

Interface::Status Interface::getResponse(std::vector<unsigned char>* buffer) {
	int length = 0;
	// int tries = 0;
	if (buffer == NULL) {
		return EMPTY_PACKET;
	}

	buffer->clear();
	// usleep(100 * tries);
	usleep(5000);

	unsigned int numBytes;
	switch (packetHandler.getType()) {
	case Packet::VER_1_0:
		numBytes = 4;
		break;

	case Packet::VER_2_0:
		numBytes = 7;
		break;
	}

	length = readInterface(buffer, numBytes);

	if (length > 0 && buffer->at(0) != 0xFF) {  // HACK: there is an issue in
												// dynamixel 1.0 where the first
												// byte is 0x00
		buffer->erase(buffer->begin());
		length -= 1;
		length += readInterface(buffer, 1);
	}
	//		std::cout << "buffer size = " << buffer->size() << std::endl;
	if (length == numBytes) {
		switch (packetHandler.getType()) {
		case Packet::VER_1_0:
			length += readInterface(buffer, buffer->at(numBytes - 1));
			break;

		case Packet::VER_2_0:
			length += readInterface(buffer, btous(buffer->data() + 5));
			break;
		}

	} else {
#ifdef DEBUGMODE
		if (verbose) {
		// printf("Error: robotisRead(): length<1\n");
		std::cerr << "Error: Interface::getResponse(): Read " << length << " bytes:";
		for (int i = 0; i < buffer->size(); i++) {
			std::cerr << (int)buffer->at(i) << " ";
//			fprintf(stderr, "0x%02X ",(unsigned int)buffer->at(i));
			std::cerr << " " << (int)buffer->at(i);
		}
		std::cerr << std::endl;
		}
#endif

		return BAD_LENGTH;
	}

	#ifdef DEBUGMODE
	if (verbose) {

	std::cerr << "Incoming Buffer:";
			for(int i = 0; i < buffer->size(); i++)
			{
	//			printf("0x%02X ",(unsigned int)buffer->at(i));
				std::cerr << " " << (int)buffer->at(i);
			}
	std::cerr << std::endl;
	}
	#endif

	if (!packetHandler.checkPacket(*buffer)) {
		// std::cout << "Checksum error?" << std::endl;
		// std::cout << "Checksum Failed. Checksum:" <<
		// (int)buffer->at(3+buffer->at(3)) <<", Calculated:" <<
		// (int)computechecksum(buffer->data()) << std::endl;
		// for(int i = 0; i < buffer->size(); i++)
		//	std::cout << std::hex << (int)buffer->at(i) << " ";
		// std::cout << std::endl;
		return BAD_CHECKSUM;
	}
	//		else {
	//			robotisError(buffer->at(4), buffer->at(2));
	//		}

	return NOERROR;
}

Interface::Status Interface::processInstruction(Instruction& instruction) {
	Status status;
	switch (instruction.type) {
	case Instruction::PING:
		status = this->ping(instruction.dynamixel);
		break;

	case Instruction::READ:
		status = this->read(instruction.dynamixel,
				DataRange(
						instruction.dynamixel->getMappedRegister(
								instruction.range.reg),
						instruction.range.length));
		break;

	case Instruction::WRITE:
		status = this->write(instruction.dynamixel,
				DataRange(
						instruction.dynamixel->getMappedRegister(
								instruction.range.reg),
						instruction.range.length));
		break;

	case Instruction::REG_WRITE:
		status = this->regWrite(instruction.dynamixel,
				DataRange(
						instruction.dynamixel->getMappedRegister(
								instruction.range.reg),
						instruction.range.length));
		break;

	case Instruction::ACTION:
		status = this->action(instruction.dynamixel);
		break;

	case Instruction::RESET:
		status = this->factoryReset(instruction.dynamixel);
		break;

	case Instruction::REBOOT:
		status = this->factoryReset(instruction.dynamixel);
		break;

	case Instruction::STATUS:
		status = UNSUPPORTED_COMMAND;
		break;

	case Instruction::SYNC_READ:
		// HACK: This uses the first motor to get the register.  Not a great
		// method.
		status =
				this->syncRead(instruction.dynamixelList,
						DataRange(
								instruction.dynamixelList->begin()->second->getMappedRegister(
										instruction.range.reg),
								instruction.range.length));
		break;

	case Instruction::SYNC_WRITE:
		// HACK: This uses the first motor to get the register.  Not a great
		// method.
		status =
				this->syncWrite(instruction.dynamixelList,
						DataRange(
								instruction.dynamixelList->begin()->second->getMappedRegister(
										instruction.range.reg),
								instruction.range.length));
		break;

	case Instruction::BULK_READ:
		status = this->bulkRead(&instruction.bulkInfo);
		break;

	case Instruction::BULK_WRITE:
		status = this->bulkWrite(&(instruction.bulkInfo));
		break;
	}

	if (status == NOERROR) {
		if (instruction.type == Instruction::SYNC_WRITE
				|| instruction.type == Instruction::SYNC_READ) {
			for (std::map<unsigned char, Motor*>::iterator it =
					instruction.dynamixelList->begin();
					it != instruction.dynamixelList->end(); it++) {
				it->second->notifyObservers(&instruction);
			}
		} else if (instruction.type == Instruction::BULK_WRITE
				|| instruction.type == Instruction::BULK_READ) {
			for (std::map<Motor*, DataRange>::iterator it =
					instruction.bulkInfo.begin();
					it != instruction.bulkInfo.end(); it++) {
				it->first->notifyObservers(&instruction);
			}
		} else {
			instruction.dynamixel->notifyObservers(&instruction);
		}
	} else {
		if (instruction.type == Instruction::SYNC_WRITE
				|| instruction.type == Instruction::SYNC_READ) {
			for (std::map<unsigned char, Motor*>::iterator it =
					instruction.dynamixelList->begin();
					it != instruction.dynamixelList->end(); it++) {
				it->second->notifyObserversOfFailure(&instruction, status);
			}
		} else if (instruction.type == Instruction::BULK_WRITE
				|| instruction.type == Instruction::BULK_READ) {
			for (std::map<Motor*, DataRange>::iterator it =
					instruction.bulkInfo.begin();
					it != instruction.bulkInfo.end(); it++) {
				it->first->notifyObserversOfFailure(&instruction, status);
			}
		} else {
			instruction.dynamixel->notifyObserversOfFailure(&instruction,
					status);
		}
	}

	return status;
}

	void Interface::setVerbose(bool verbose) {
		this->verbose = verbose;
	}

std::string robotisError(int error, int ID) {
	std::stringstream result;
	result << "Motor ID " << ID << " Errors:" << std::endl;

	if ((error >> 6) & 1)
		result << " - Instruction Error\n";
	if ((error >> 5) & 1)
		result << " - Overload Error\n";
	if ((error >> 4) & 1)
		result << " - Checksum Error\n";
	if ((error >> 3) & 1)
		result << " - Range Error\n";
	if ((error >> 2) & 1)
		result << " - Overheating Error\n";
	if ((error >> 1) & 1)
		result << " - Angle Limit Error\n";
	if (error & 1)
		result << " - Input Voltage Error\n";

	return result.str();
}

#ifdef _cplusplus
}
#endif
