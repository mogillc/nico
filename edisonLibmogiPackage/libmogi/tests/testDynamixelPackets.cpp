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

#include "dynamixel.h"
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

using namespace Mogi;
using namespace Dynamixel;

bool equal(const std::vector<unsigned char>&,
		const std::vector<unsigned char>&);
bool testPackets();
bool testPackets_ver2();
bool testBadPackets();

int main(int argc, char* argv[]) {
	bool allTestsPassed = true;
	//	Handler* dynamixelHandler = new Handler;

	std::cout << "Beginning tests..." << std::endl;
	allTestsPassed = testPackets() ? allTestsPassed : false;
	allTestsPassed = testPackets_ver2() ? allTestsPassed : false;
	allTestsPassed = testBadPackets() ? allTestsPassed : false;

	if (allTestsPassed) {
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
	//
	//	if(!dynamixelHandler->openInterface(Interface::FTDI, 1000000,
	//"/dev/tty.usbmodem1411")) {
	//		//return -1;
	//	}
	//	dynamixelHandler->setPacketType(Packet::VER_2_0);
	//
	//	for (int i = 1; i < 20; i++) {
	//		dynamixelHandler->addDynamixel(i);
	//	}
	//	//dynamixelHandler.addDynamixel(6);
	//	//dynamixelHandler.addDynamixel(12);
	//	//dynamixelHandler.addDynamixel(18);
	//
	//	std::vector<Motor*> dynamixels = dynamixelHandler->getDynamixels();
	//
	//	std::cout << "Found " << dynamixels.size() << " motors:" << std::endl;
	//	for (int i = 0; i < dynamixels.size(); i++) {
	//		std::cout << " - Motor " << dynamixels[i]->ID << "\ttype:" <<
	//dynamixels[i]->model << std::endl;
	//
	//		dynamixels[i]->setGoalPosition(0);
	//		// This sets everything to be in the middle
	//		dynamixels[i]->setMovingSpeed(30);
	//	}
	//
	//	usleep(100000);
	////	if(dynamixelHandler->handle() != Handler::NOERROR) {
	////		std::cout << "ERROR: on dynamixelHandler.handle()" << std::endl;
	////	}
	//	Instruction *instruction = new Instruction;
	//	instruction->type = Instruction::SYNC_WRITE;
	//	instruction->numBytes = 2;
	//	instruction->startRegister = 30;
	//	dynamixelHandler->pushInstruction(instruction);
	//	//usleep(1000000);
	//	for (int i = 0; i < 4; i++) {
	//	//	usleep(1000000);
	//		for (int j = 0; j < 8; j++) {
	//			usleep(200);
	//			dynamixelHandler->pushInstruction(new
	//Instruction((Instruction::Type)j, i, 2));
	//		}
	//
	//	}
	//
	//
	////	std::cout << std::endl << "force adding dynamixels:" << std::endl;
	////	for (int i = 1; i < 7; i++) {
	////		dynamixelHandler.forceAddDynamixel(i);
	////	}
	//
	//	dynamixels = dynamixelHandler->getDynamixels();
	//	for (int i = 0; i < dynamixels.size(); i++) {
	//
	//	//	dynamixels[i]->writeValue(25, 1);
	//	}
	//
	//
	//	std::cout << "Done." << std::endl;
	//	//sleep(1);
	//	delete dynamixelHandler;
	//	//pthread_exit(NULL);
	//	return 0;
}

bool testPackets() {
	bool Passed = true;
	std::cout << "Testing Dynamixel 1.0 " << std::endl;

	Packet dynamixelPacket;

	std::cout << "\tTesting ping packet ......... ";
	std::vector<unsigned char> test = dynamixelPacket.ping(1);
	std::vector<unsigned char> expected;
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x01);
	expected.push_back(0x02);
	expected.push_back(0x01);
	expected.push_back(0xFB);

	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting read packet ......... ";
	test = dynamixelPacket.read(1, DataRange(0, 3));
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x01);
	expected.push_back(0x04);
	expected.push_back(0x02);
	expected.push_back(0x00);
	expected.push_back(0x03);
	expected.push_back(0xF5);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting write packet ........ ";
	unsigned char buffer[64] = { 0x00, 0x02 };
	test = dynamixelPacket.write(1, DataRange(0x08, 2), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x01);
	expected.push_back(0x05);
	expected.push_back(0x03);
	expected.push_back(0x08);
	expected.push_back(0x00);
	expected.push_back(0x02);
	expected.push_back(0xEC);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting sync write packet ... ";
	std::map<unsigned char, Motor*> dynamixels;
	Handler handler;
	for (unsigned int i = 0; i < 4; i++) {
		//		dynamixels.push_back( new Motor(i) );
		handler.forceAddDynamixel(i);
	}
	dynamixels = handler.getDynamixels();
	dynamixels[0]->setWord(0x1E, 0x0010);
	dynamixels[0]->setWord(0x20, 0x0150);
	//	dynamixels[0]->registers[0x1E] = 0x10;
	//	dynamixels[0]->registers[0x1F] = 0x00;
	//	dynamixels[0]->registers[0x20] = 0x50;
	//	dynamixels[0]->registers[0x21] = 0x01;

	dynamixels[1]->setWord(0x1E, 0x0220);
	dynamixels[1]->setWord(0x20, 0x0360);
	//	dynamixels[1]->registers[0x1E] = 0x20;
	//	dynamixels[1]->registers[0x1F] = 0x02;
	//	dynamixels[1]->registers[0x20] = 0x60;
	//	dynamixels[1]->registers[0x21] = 0x03;

	dynamixels[2]->setWord(0x1E, 0x0030);
	dynamixels[2]->setWord(0x20, 0x0170);
	//	dynamixels[2]->registers[0x1E] = 0x30;
	//	dynamixels[2]->registers[0x1F] = 0x00;
	//	dynamixels[2]->registers[0x20] = 0x70;
	//	dynamixels[2]->registers[0x21] = 0x01;

	dynamixels[3]->setWord(REG_Goal_Pos, 0x0220);
	dynamixels[3]->setWord(REG_Moving_Speed, 0x0380);
	//	dynamixels[3]->registers[0x1E] = 0x20;
	//	dynamixels[3]->registers[0x1F] = 0x02;
	//	dynamixels[3]->registers[0x20] = 0x80;
	//	dynamixels[3]->registers[0x21] = 0x03;
	test = dynamixelPacket.syncWrite(&dynamixels, DataRange(0x1E, 4));

	expected.clear();
	expected.push_back(0XFF);
	expected.push_back(0XFF);
	expected.push_back(0XFE);
	expected.push_back(0X18);
	expected.push_back(0X83);

	expected.push_back(0X1E);
	expected.push_back(0X04);
	expected.push_back(0X00);
	expected.push_back(0X10);
	expected.push_back(0X00);

	expected.push_back(0X50);
	expected.push_back(0X01);
	expected.push_back(0X01);
	expected.push_back(0X20);
	expected.push_back(0X02);

	expected.push_back(0X60);
	expected.push_back(0X03);
	expected.push_back(0X02);
	expected.push_back(0X30);
	expected.push_back(0X00);

	expected.push_back(0X70);
	expected.push_back(0X01);
	expected.push_back(0X03);
	expected.push_back(0X20);
	expected.push_back(0X02);

	expected.push_back(0X80);
	expected.push_back(0X03);
	expected.push_back(0X12);
	// std::cout << "Length:" << expected.size() << std::endl;
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bulk read packet .... ";

	Handler handlerBulkRead;
	handlerBulkRead.forceAddDynamixel(1, MODEL_MX106);
	handlerBulkRead.forceAddDynamixel(5, MODEL_AX12); // this is unsupported and
													  // will not be come a part
													  // of the packet
	handlerBulkRead.forceAddDynamixel(2, MODEL_MX28);
	dynamixels = handlerBulkRead.getDynamixels();

	std::map<Motor*, DataRange> dataRanges;
	dataRanges[dynamixels[1]] = DataRange(REG_Goal_Pos, 2);
	dataRanges[dynamixels[5]] = DataRange(0x16, 8);
	dataRanges[dynamixels[2]] = DataRange(REG_Present_Pos, 2);

	test = dynamixelPacket.bulkRead(&dataRanges);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFE);  // ID, broadcast
	expected.push_back(0x09);  // acket length
	expected.push_back(0x92);  // BULK_READ
	expected.push_back(0x00);  // 0x00
	expected.push_back(0x02);  // 1) NumBytes first motor
	expected.push_back(0x01);  // 1) ID of first motor
	expected.push_back(0x1E);  // 1) Start Address first motor
	expected.push_back(0x02);  // 2) NumBytes second motor
	expected.push_back(0x02);  // 2) ID of the second motor
	expected.push_back(0x24);  // 2) Start address second motor
	expected.push_back(0x1D);  // checksum

	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	// The following functionality is handled in the Handler
	//	std::cout << "\tTesting re-write sync ........ ";
	//	test = dynamixelPacket.syncWrite(dynamixels, 0x1E, 4);
	//	if (test.size() == 0) {
	//		std::cout << "Passed" << std::endl;
	//	} else {
	//		std::cout << "FAILED" << std::endl;
	//		Passed = false;
	//	}

	std::cout << "\tTesting check packet ........ ";
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x01);
	expected.push_back(0x05);
	expected.push_back(0x03);
	expected.push_back(0x08);
	expected.push_back(0x00);
	expected.push_back(0x02);
	expected.push_back(0xEC);
	if (!dynamixelPacket.checkPacket(expected)) {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "\tTesting check packet ........ ";
	expected.clear();
	expected.push_back(0XFF);
	expected.push_back(0XFF);
	expected.push_back(0XFE);
	expected.push_back(0X18);
	expected.push_back(0X83);
	expected.push_back(0X1E);
	expected.push_back(0X04);
	expected.push_back(0X00);
	expected.push_back(0X10);
	expected.push_back(0X00);
	expected.push_back(0X50);
	expected.push_back(0X01);
	expected.push_back(0X01);
	expected.push_back(0X20);
	expected.push_back(0X02);
	expected.push_back(0X60);
	expected.push_back(0X03);
	expected.push_back(0X02);
	expected.push_back(0X30);
	expected.push_back(0X00);
	expected.push_back(0X70);
	expected.push_back(0X01);
	expected.push_back(0X03);
	expected.push_back(0X21);
	expected.push_back(0X02);
	expected.push_back(0X80);
	expected.push_back(0X03);
	expected.push_back(0X12);
	if (dynamixelPacket.checkPacket(expected)) {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	} else {
		std::cout << "Passed" << std::endl;
	}


	std::cout << "\tTesting reg write packet .... ";
	buffer[0] = 0x14;
	buffer[1] = 0x02;
	test = dynamixelPacket.regWrite(0x64, DataRange(0x08, 2), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x64);
	expected.push_back(0x05);
	expected.push_back(0x04);
	expected.push_back(0x08);
	expected.push_back(0x14);
	expected.push_back(0x02);
	expected.push_back(116);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting action packet ....... ";
	test = dynamixelPacket.action(0x78);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0x78);
	expected.push_back(0x02);
	expected.push_back(0x05);
	expected.push_back(128);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting reset packet ........ ";
	test = dynamixelPacket.reset(0xA8);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xA8);
	expected.push_back(0x02);
	expected.push_back(0x06);
	expected.push_back(79);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting reboot packet ....... ";
	test = dynamixelPacket.reboot(0xA8);
	expected.clear();
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting sync packet ......... ";
	test = dynamixelPacket.syncRead(NULL, DataRange(3,4));
	expected.clear();
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bulk write packet ... ";
	test = dynamixelPacket.bulkWrite(NULL);
	expected.clear();
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	return Passed;
}

bool testPackets_ver2() {
	bool Passed = true;

	std::cout << "Testing Dynamixel 2.0 " << std::endl;

	Packet dynamixelPacket;
	dynamixelPacket.setType(Packet::VER_2_0);

	std::cout << "\tTesting ping packet ......... ";
	std::vector<unsigned char> test = dynamixelPacket.ping(1);
	std::vector<unsigned char> expected;
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x03);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x01);  // instruction
	expected.push_back(25);    // CRC h
	expected.push_back(78);    // CRC l
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting read packet ......... ";
	test = dynamixelPacket.read(7, DataRange(0, 3));
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x07);  // ID
	expected.push_back(0x07);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x02);  // instruction
	expected.push_back(0x00);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0x03);  // read length lsb
	expected.push_back(0x00);  // read length msb
	expected.push_back(54);
	expected.push_back(183);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting write packet ........ ";
	unsigned char buffer[64] = { 0x00, 0x02 };
	test = dynamixelPacket.write(1, DataRange(0x08, 2), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x07);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x03);  // instruction
	expected.push_back(0x08);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0x00);  // first byte
	expected.push_back(0x02);  // second byte
	expected.push_back(85);
	expected.push_back(253);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting read 2 packet ....... ";
	// unsigned char buffer[64] = {0x00, 0x02};
	test = dynamixelPacket.read(1, DataRange(0x00, 2));
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x07);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x02);  // instruction
	expected.push_back(0x00);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0x02);  // first byte
	expected.push_back(0x00);  // second byte
	expected.push_back(33);  // 0x21
	expected.push_back(81);  // 0x51
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting write LED packet .... ";
	// unsigned char buffer[64] = {0x00, 0x02};
	buffer[0] = 4;
	test = dynamixelPacket.write(1, DataRange(25,1), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x06);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x03);  // instruction
	expected.push_back(0x19);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0x04);  // first byte
	expected.push_back(0x31);  // checksum1
	expected.push_back(0x62);  // checksum2
	//expected.push_back(33);  // 0x21
	//expected.push_back(81);  // 0x51
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting write goal packet ... ";
	// unsigned char buffer[64] = {0x00, 0x02};
	buffer[0] = 0xC8;
	buffer[1] = 0x00;
	test = dynamixelPacket.write(1, DataRange(30,2), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x07);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x03);  // instruction
	expected.push_back(0x1E);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0xC8);  // first byte
	expected.push_back(0x00);  // second byte
	expected.push_back(0x50);  // checksum1
	expected.push_back(0xF5);  // checksum2
	//expected.push_back(33);  // 0x21
	//expected.push_back(81);  // 0x51
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting sync write packet 2 . ";
	std::map<unsigned char, Motor*> dynamixels;
	Handler handler;
	for (unsigned int i = 0; i <= 6; i++) {
		//		dynamixels.push_back( new Motor(i) );
		handler.forceAddDynamixel(i);
	}
	dynamixels = handler.getDynamixels();
	dynamixels[0]->setWord(0x19, 1);
	dynamixels[1]->setWord(0x19, 6);
	dynamixels[2]->setWord(0x19, 2);
	dynamixels[3]->setWord(0x19, 5);
	dynamixels[4]->setWord(0x19, 7);
	dynamixels[5]->setWord(0x19, 3);
	dynamixels[6]->setWord(0x19, 4);

	test = dynamixelPacket.syncWrite(&dynamixels, DataRange(0x19, 1));

	expected.clear();
	expected.push_back(0XFF);
	expected.push_back(0XFF);
	expected.push_back(0XFD);
	expected.push_back(0X00);
	expected.push_back(0XFE);
	expected.push_back(0X15);
	expected.push_back(0X00);
	expected.push_back(0X83);

	expected.push_back(0X19);	// register lsb
	expected.push_back(0X00);	// register msb
	expected.push_back(0X01);	// length lsb?
	expected.push_back(0X00);	// length msb

	expected.push_back(0X00);	// ID 1
	expected.push_back(0X01);	// data 1

	expected.push_back(0X01);	// ID 2
	expected.push_back(0X06);	// data 2

	expected.push_back(0X02);	// ID 3
	expected.push_back(0X02);	// data 3

	expected.push_back(0X03);	// ID 4
	expected.push_back(0X05);	// data 4

	expected.push_back(0X04);	// ID 5
	expected.push_back(0X07);	// data 5

	expected.push_back(0X05);	// ID 6
	expected.push_back(0X03);	// data 6

	expected.push_back(0X06);	// ID 7
	expected.push_back(0X04);	// data 7

	expected.push_back(0X28);
	expected.push_back(0XE0);

	// std::cout << "Length:" << expected.size() << std::endl;
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}


	//	255, 255, 253, 0, 1, 6, 0, 2, 5, 1
	//	std::cout << "\tTesting read 2 from Nick  packet ........ ";
	//	//unsigned char buffer[64] = {0x00, 0x02};
	//	test = dynamixelPacket.read(1, 0x00, 2);
	//	expected.clear();
	//	expected.push_back(0xFF);
	//	expected.push_back(0xFF);
	//	expected.push_back(0xFD);
	//	expected.push_back(0x00);	// end header
	//	expected.push_back(0x01);	// ID
	//	expected.push_back(0x06);	// length lsb
	//	expected.push_back(0x00);	// length msb
	//	expected.push_back(0x02);	// instruction
	//	expected.push_back(0x05);	// address lsb
	//	expected.push_back(0x01);	// address msb
	//	expected.push_back(0x02);	// first byte
	//	expected.push_back(0x00);	// second byte
	//	expected.push_back(33);//0x21
	//	expected.push_back(81);//0x51
	//	if (equal(test, expected)) {
	//		std::cout << "Passed" << std::endl;
	//	} else {
	//		std::cout << "FAILED" << std::endl;
	//		Passed = false;
	//	}

	//	std::cout << "\tTesting bulk read packet .... ";
	//
	//	Handler handlerBulkRead;
	//	handlerBulkRead.forceAddDynamixel(1, MODEL_H42_20_S300_R);
	//	handlerBulkRead.forceAddDynamixel(5, MODEL_M54_40_S250_R);	// this is
	//unsupported and will not be come a part of the packet
	//	handlerBulkRead.forceAddDynamixel(2, MODEL_XL320);
	//	std::map<unsigned char, Motor*> dynamixels =
	//handlerBulkRead.getDynamixels();
	//	std::map<Motor*, DataRange> dataRanges;
	//	dataRanges[dynamixels[1]] = DataRange(REG_Goal_Pos, 2);
	//	dataRanges[dynamixels[5]] = DataRange(0x16, 8);
	//	dataRanges[dynamixels[2]] = DataRange(REG_Present_Pos, 2);
	//	test = dynamixelPacket.bulkRead(&dataRanges);
	//	expected.clear();
	//	expected.push_back(0xFF);
	//	expected.push_back(0xFF);
	//	expected.push_back(0xFD);
	//	expected.push_back(0x00);	// end header
	//	expected.push_back(0xFE);	// broadcast ID
	//	expected.push_back(0x12);	// packet length
	//	expected.push_back(0x00);	// packet length msb
	//	expected.push_back(0x92);	// BULK_READ
	//
	//	expected.push_back(0x01);	// 1) ID of first motor
	//	expected.push_back(0x54);	// 1) Start Address first motor
	//	expected.push_back(0x02);	// 1) Start Address first motor hsb
	//	expected.push_back(0x02);	// 1) NumBytes first motor
	//	expected.push_back(0x00);	// 1) NumBytes first motor
	//
	//	expected.push_back(0x05);	// 2) ID
	//	expected.push_back(0x16);	// 2) Start Address
	//	expected.push_back(0x00);	// 2) Start Address hsb
	//	expected.push_back(0x08);	// 2) NumBytes
	//	expected.push_back(0x00);	// 2) NumBytes hsb
	//
	//	expected.push_back(0x02);	// 3) ID
	//	expected.push_back(0x25);	// 3) Start Address
	//	expected.push_back(0x00);	// 3) Start Address hsb
	//	expected.push_back(0x02);	// 3) NumBytes
	//	expected.push_back(0x00);	// 3) NumBytes hsb
	//
	//	expected.push_back(92);	// checksum
	//	expected.push_back(183);	// checksum
	//
	//	if (equal(test, expected)) {
	//		std::cout << "Passed" << std::endl;
	//	} else {
	//		std::cout << "FAILED" << std::endl;
	//		Passed = false;
	//	}

	std::cout << "\tTesting check packet ........ ";
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x01);  // ID
	expected.push_back(0x03);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x01);  // instruction
	expected.push_back(25);    // CRC h
	expected.push_back(78);    // CRC l
	if (!dynamixelPacket.checkPacket(expected)) {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::cout << "\tTesting reg write packet .... ";
	buffer[0] = 0x14;
	buffer[1] = 0x02;
	test = dynamixelPacket.regWrite(0x64, DataRange(0x08, 2), buffer);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x64);  // ID
	expected.push_back(0x07);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x04);  // instruction
	expected.push_back(0x08);  // address lsb
	expected.push_back(0x00);  // address msb
	expected.push_back(0x14);  // first byte
	expected.push_back(0x02);  // second byte
	expected.push_back(116);
	expected.push_back(85);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting action packet ....... ";
	test = dynamixelPacket.action(0x78);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0x78);  // ID
	expected.push_back(0x03);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x05);  // instruction
	expected.push_back(21);
	expected.push_back(58);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting reset packet ........ ";
	test = dynamixelPacket.reset(0xA8);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0xA8);  // ID
	expected.push_back(0x04);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x06);  // instruction
	expected.push_back(0xFF);  // reset level
	expected.push_back(30);
	expected.push_back(84);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		for (std::vector<unsigned char>::iterator it = test.begin(); it != test.end(); it++) {
			std::cout << std::hex << " " << (int)*it;
		}
		std::cout << std::endl;
		for (std::vector<unsigned char>::iterator it = expected.begin(); it != expected.end(); it++) {
			std::cout << std::hex << " " << (int)*it;
		}
		std::cout << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting reboot packet ....... ";
	test = dynamixelPacket.reboot(0xC0);
	expected.clear();
	expected.push_back(0xFF);
	expected.push_back(0xFF);
	expected.push_back(0xFD);
	expected.push_back(0x00);  // end header
	expected.push_back(0xC0);  // ID
	expected.push_back(0x03);  // length lsb
	expected.push_back(0x00);  // length msb
	expected.push_back(0x08);  // instruction
	expected.push_back(14);
	expected.push_back(218);
	if (equal(test, expected)) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	return Passed;
}

bool testBadPackets() {
	bool Passed = true;

	std::cout << "Testing Bad Packets V1 " << std::endl;

	std::vector<unsigned char> packet;
	Packet packetCreator;
	packetCreator.setType(Packet::VER_1_0);

	std::cout << "\tTesting short packet ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 1 ........ ";
	packet.clear();
	packet.push_back(0xFE);
	packet.push_back(0xFF);
	packet.push_back(0x01);
	packet.push_back(0x02);
	packet.push_back(0x01);
	packet.push_back(0xFB);

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 2 ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFE);
	packet.push_back(0x01);
	packet.push_back(0x02);
	packet.push_back(0x01);
	packet.push_back(0xFB);

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad length .......... ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);
	packet.push_back(0x01);
	packet.push_back(0x02);
	packet.push_back(0x01);
	packet.push_back(0x02);
	packet.push_back(0xFB);

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}


	std::cout << "Testing Bad Packets V2 " << std::endl;
	packetCreator.setType(Packet::VER_2_0);

	std::cout << "\tTesting short packet ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 1 ........ ";
	packet.clear();
	packet.push_back(0xFE);
	packet.push_back(0xFF);
	packet.push_back(0xFD);
	packet.push_back(0x00);  // end header
	packet.push_back(0x01);  // ID
	packet.push_back(0x03);  // length lsb
	packet.push_back(0x00);  // length msb
	packet.push_back(0x01);  // instruction
	packet.push_back(25);    // CRC h
	packet.push_back(78);    // CRC l

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 2 ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xEF);
	packet.push_back(0xFD);
	packet.push_back(0x00);  // end header
	packet.push_back(0x01);  // ID
	packet.push_back(0x03);  // length lsb
	packet.push_back(0x00);  // length msb
	packet.push_back(0x01);  // instruction
	packet.push_back(25);    // CRC h
	packet.push_back(78);    // CRC l

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 3 ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);
	packet.push_back(0xFF);
	packet.push_back(0x00);  // end header
	packet.push_back(0x01);  // ID
	packet.push_back(0x03);  // length lsb
	packet.push_back(0x00);  // length msb
	packet.push_back(0x01);  // instruction
	packet.push_back(25);    // CRC h
	packet.push_back(78);    // CRC l

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad header 4 ........ ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);
	packet.push_back(0xFD);
	packet.push_back(0x01);  // end header
	packet.push_back(0x01);  // ID
	packet.push_back(0x03);  // length lsb
	packet.push_back(0x00);  // length msb
	packet.push_back(0x01);  // instruction
	packet.push_back(25);    // CRC h
	packet.push_back(78);    // CRC l

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	std::cout << "\tTesting bad length .......... ";
	packet.clear();
	packet.push_back(0xFF);
	packet.push_back(0xFF);
	packet.push_back(0xFD);
	packet.push_back(0x00);  // end header
	packet.push_back(0x01);  // ID
	packet.push_back(0x03);  // length lsb
	//packet.push_back(0x00);  // length msb
	packet.push_back(0x01);  // instruction
	packet.push_back(25);    // CRC h
	packet.push_back(78);    // CRC l

	if (packetCreator.checkPacket(packet) == false) {	// check that this fails correctly
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		Passed = false;
	}

	
	return Passed;
}

bool equal(const std::vector<unsigned char>& a,
		const std::vector<unsigned char>& b) {
	if (a.size() != b.size()) {
		std::cout << "Size Mismatch " << a.size() << " != " << b.size() << ": ";
		return false;
	}

	for (unsigned int i = 0; i < a.size(); i++) {
		if (a[i] != b[i]) {
			std::cout << "Element mismatch at " << i << ", " << (int) a[i]
					<< " != " << (int) b[i] << ": ";
			return false;
		}
	}

	return true;
}
