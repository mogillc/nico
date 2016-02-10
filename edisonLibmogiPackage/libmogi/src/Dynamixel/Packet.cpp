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

#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Dynamixel;

//#define INSTRUCTION_PING (0x01)
//#define INSTRUCTION_READ (0x02)
//#define INSTRUCTION_WRITE (0x03)
//#define INSTRUCTION_REG_WRTIE (0x04)
//#define INSTRUCTION_ACTION (0x05)
//#define INSTRUCTION_RESET (0x06)
//
//#define INSTRUCTION_SYNC_WRITE (0x83)
//
//// Only supported in 2.0:
//#define INSTRUCTION_REBOOT (0x08)
//
//#define INSTRUCTION_STATUS (0x55)
//
//#define INSTRUCTION_SYNC_READ (0x82)
//
//#define INSTRUCTION_BULK_READ (0x92)
//#define INSTRUCTION_BULK_WRITE (0x93)

// This is from ROBOTIS:
//	Calculating transmitted/received packets
//
//	unsigned short update_crc(unsigned short crc_accum, unsigned char
//*data_blk_ptr, unsigned short data_blk_size);
//
//	Output : calculated CRC values
//
//
//
//	Input :
//
//	1) crc_accum : always set at
//
//	2) data_blk_ptr : pointer for packet array transmission or reception
//
//		3) data_blk_size : Packet’s size not subject to CRC See the the
//following for data_blk_size
//
//			data_blk_size = Header(3) + Reserved(1) + ID(1) + Length(2)
//+  Packet Length(?) – CRC(2)  = 3+1+1+2+Pakcet Length-2 = 5 + Packet Length;
//
//			Packet Length = (LEN_H << 8 ) + LEN_L; // refer to packet’s
//#6 and #7
//
//			example :
//
//			unsigned char TxPacket[] = { 0xFF 0xFF 0xFD 0x00 0x01 0x00
//0x07 0x02 0x00 0x00 0x00 0x02  CRC_L  CRC_H }
//
//			An instruction packet is made for transmission. The
//following are the packet’s components
//
//			0xFF 0xFF 0xFD : header
//
//			0x00 : Reserved
//
//			0x01 : ID 1
//
//			0x00 0x07 : size of packet of 7
//
//			0x02 : Read Instruction
//
//			0x00 0x00 : begin from address 0
//
//			0x00 0x02 : Read 2 bytes
//
//			CRC_L CRC_H : CRC values
//
//			Then calculate CRC values with the functions shown below
//
//			CRC = update_crc ( 0, TxPacket , 12 ) ; // 12 = 5 + Packet
//Length(0x00 0x07) = 5+7
//
//			CRC_L = (CRC & 0x00FF);
//
//			CRC_H = (CRC>>8) & 0x00FF;

unsigned short update_crc(unsigned short crc_accum,
		const unsigned char* data_blk_ptr, unsigned short data_blk_size) {
	static unsigned short i, j;
	static unsigned short crc_table[256] = { 0x0000, 0x8005, 0x800F, 0x000A,
			0x801B, 0x001E, 0x0014, 0x8011, 0x8033, 0x0036, 0x003C, 0x8039,
			0x0028, 0x802D, 0x8027, 0x0022, 0x8063, 0x0066, 0x006C, 0x8069,
			0x0078, 0x807D, 0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A,
			0x804B, 0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
			0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF, 0x00FA,
			0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5, 0x80AF, 0x00AA,
			0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093, 0x0096, 0x009C, 0x8099,
			0x0088, 0x808D, 0x8087, 0x0082, 0x8183, 0x0186, 0x018C, 0x8189,
			0x0198, 0x819D, 0x8197, 0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA,
			0x81AB, 0x01AE, 0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA,
			0x81FB, 0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
			0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F, 0x014A,
			0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176, 0x017C, 0x8179,
			0x0168, 0x816D, 0x8167, 0x0162, 0x8123, 0x0126, 0x012C, 0x8129,
			0x0138, 0x813D, 0x8137, 0x0132, 0x0110, 0x8115, 0x811F, 0x011A,
			0x810B, 0x010E, 0x0104, 0x8101, 0x8303, 0x0306, 0x030C, 0x8309,
			0x0318, 0x831D, 0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A,
			0x832B, 0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
			0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C, 0x8359,
			0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5, 0x83CF, 0x03CA,
			0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3, 0x03F6, 0x03FC, 0x83F9,
			0x03E8, 0x83ED, 0x83E7, 0x03E2, 0x83A3, 0x03A6, 0x03AC, 0x83A9,
			0x03B8, 0x83BD, 0x83B7, 0x03B2, 0x0390, 0x8395, 0x839F, 0x039A,
			0x838B, 0x038E, 0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A,
			0x829B, 0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
			0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC, 0x82E9,
			0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5, 0x82DF, 0x02DA,
			0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243, 0x0246, 0x024C, 0x8249,
			0x0258, 0x825D, 0x8257, 0x0252, 0x0270, 0x8275, 0x827F, 0x027A,
			0x826B, 0x026E, 0x0264, 0x8261, 0x0220, 0x8225, 0x822F, 0x022A,
			0x823B, 0x023E, 0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219,
			0x0208, 0x820D, 0x8207, 0x0202 };

	for (j = 0; j < data_blk_size; j++) {
		i = ((unsigned short) (crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}

	return crc_accum;
}
// END ROBOTIS

unsigned char Mogi::Dynamixel::computeChecksum(const unsigned char* buffer) {
	unsigned char checksum = 0;
	int i;
	for (i = 2; i < (3 + buffer[3]); i++)
		checksum += buffer[i];
	return ~(checksum & 0xFF);
}

void Packet::startPacket(unsigned char ID) {
	packet.clear();
	packet.push_back(0xFF);  // 0
	packet.push_back(0xFF);  // 1

	switch (type) {
	case VER_1_0:
		packet.push_back(ID);    //
		packet.push_back(0x00);  //	length
		break;

	case VER_2_0:
		packet.push_back(0xFD);  // header	2
		packet.push_back(0x00);  // reserved	3
		packet.push_back(ID);    //	4
		packet.push_back(0x00);  //	length lsb	5
		packet.push_back(0x00);  //	length msb	6
		break;
	}
}

void Packet::finishPacket() {
	unsigned short CRC;
	switch (type) {
	case VER_1_0:
		packet[3] = packet.size() - 3;
		packet.push_back(computeChecksum(packet.data()));
		break;

	case VER_2_0:
		packet[5] = (unsigned short) (packet.size() - 5) & 0x00FF; //	length lsb
		packet[6] = ((unsigned short) (packet.size() - 5) & 0xFF00) >> 8; //	length msb
		CRC = update_crc(0, packet.data(), packet.size());
		packet.push_back(CRC & 0x00FF);
		packet.push_back((CRC & 0xFF00) >> 8);
		break;
	}
}

const std::vector<unsigned char>& Packet::ping(const unsigned char& ID) {
	startPacket(ID);
	packet.push_back(Instruction::PING);  // same instruction for both protocols

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::read(const unsigned char& ID,
		DataRange range) {
	startPacket(ID);
	packet.push_back(Instruction::READ);

	if (type == VER_1_0) {
		packet.push_back(range.reg);
		packet.push_back(range.length);
	} else {  // version 2.0
		packet.push_back(range.reg & 0xff);
		packet.push_back((range.reg >> 8) & 0xff);
		packet.push_back(range.length & 0xff);
		packet.push_back((range.length >> 8) & 0xff);
	}

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::write(const unsigned char& ID,
		DataRange range, unsigned char* data) {
	startPacket(ID);
	packet.push_back(Instruction::WRITE);

	if (type == VER_1_0) {
		packet.push_back(range.reg);
	} else {  // version 2.0
		packet.push_back(range.reg & 0xff);
		packet.push_back((range.reg >> 8) & 0xff);
	}
	for (int i = 0; i < range.length; i++) {
		packet.push_back(data[i]);
	}

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::regWrite(const unsigned char& ID,
		DataRange range, unsigned char* data) {
	startPacket(ID);
	packet.push_back(Instruction::REG_WRITE);

	if (type == VER_1_0) {
		packet.push_back(range.reg);
	} else {  // version 2.0
		packet.push_back(range.reg & 0xff);
		packet.push_back((range.reg >> 8) & 0xff);
	}
	for (int i = 0; i < range.length; i++) {
		packet.push_back(data[i]);
	}

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::action(const unsigned char& ID) {
	startPacket(ID);
	packet.push_back(Instruction::ACTION); // same instruction for both protocols

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::reset(const unsigned char& ID) {
	startPacket(ID);
	packet.push_back(Instruction::RESET); // same instruction for both protocols

	if (type == VER_2_0) {
		packet.push_back(0xFF);  // Resets all values.  TODO: pass parameter to set this for different reset levels
	}

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::reboot(const unsigned char& ID) {
	if (type == VER_1_0) {  // Only supported in version 2.0, all motors
		packet.clear();
		return packet;
	}

	startPacket(ID);
	packet.push_back(Instruction::REBOOT); // same instruction for both protocols

	finishPacket();
	return packet;
}

const std::vector<unsigned char>& Packet::syncRead(
		std::map<unsigned char, Motor*>* dynamixels, DataRange range) {
	if (type == VER_1_0) {  // Only supported in version 2.0, all motors
		packet.clear();
		return packet;
	}

	startPacket(0xFE);  // broadcast ID
	packet.push_back(Instruction::SYNC_READ);

	packet.push_back(range.reg & 0xff);
	packet.push_back((range.reg >> 8) & 0xff);
	packet.push_back(range.length & 0xff);
	packet.push_back((range.length >> 8) & 0xff);

	for (std::map<unsigned char, Motor*>::iterator it = dynamixels->begin();
			it != dynamixels->end(); it++) {
		Motor* dynamixel = it->second;

		packet.push_back(dynamixel->getByte(REG_ID));  // First is the motor ID
	}

	finishPacket();

	return packet;
}

const std::vector<unsigned char>& Packet::syncWrite(
		std::map<unsigned char, Motor*>* dynamixels, DataRange range) {
	startPacket(0xFE);  // broadcast ID
	packet.push_back(Instruction::SYNC_WRITE);

	if (type == VER_1_0) {
		packet.push_back(range.reg);
		packet.push_back(range.length);
	} else {  // version 2.0
		packet.push_back(range.reg & 0xff);
		packet.push_back((range.reg >> 8) & 0xff);
		packet.push_back(range.length & 0xff);
		packet.push_back((range.length >> 8) & 0xff);
	}

	bool needToUpdate;
	int numberOfUpdates = 0;
	//	for (unsigned int ii = 0; ii < dynamixels.size(); ii++) {
	for (std::map<unsigned char, Motor*>::iterator it = dynamixels->begin();
			it != dynamixels->end(); it++) {
		Motor* dynamixel = it->second;

		needToUpdate = false;
		for (int jj = 0; jj < range.length; jj++) { // check to see if we even need to update this motor:
			if (dynamixel->registers[range.reg + jj]
					!= dynamixel->registersDesired[range.reg + jj]) {
				needToUpdate = true;
				break;
			}
		}
		if (needToUpdate == false) {
			continue;
		}
		numberOfUpdates++;

		packet.push_back(dynamixel->getByte(REG_ID));  // First is the motor ID

		for (int jj = 0; jj < range.length; jj++) {  // Followed by the values
			packet.push_back(dynamixel->registersDesired[range.reg + jj]);
		}
	}

	if (numberOfUpdates == 0) {
		packet.clear();  // Nothing to do!
	} else {
		finishPacket();
	}
	return packet;
}

const std::vector<unsigned char>& Packet::bulkRead(
		std::map<Motor*, DataRange>* range) {
	startPacket(0xFE);  // broadcast ID
	packet.push_back(Instruction::BULK_READ);

	if (type == VER_1_0) {
		packet.push_back(0x00);  // First parameter is 0x00
	}

	int numberOfReads = 0;
	for (std::map<Motor*, DataRange>::iterator it = range->begin();
			it != range->end(); it++) {
		Motor* dynamixel = it->first;

		if (type == VER_1_0) {
			switch (dynamixel->getWord(REG_Model_Number)) {
			case MODEL_MX12W:
			case MODEL_MX28:
			case MODEL_MX64:
			case MODEL_MX106:
				break;

			default:
				continue;
				break;
			}
		}

		if (type == VER_1_0) { // TODO: indivual number of bytes and start register
							   // for each motor
			packet.push_back(it->second.length);
			packet.push_back(dynamixel->getByte(REG_ID)); // First is the motor ID
			packet.push_back(it->second.reg);
		} else {                                          // version 2.0
			packet.push_back(dynamixel->getByte(REG_ID)); // First is the motor ID
			packet.push_back(
					dynamixel->getMappedRegister(it->second.reg) & 0xff);
			packet.push_back(
					(dynamixel->getMappedRegister(it->second.reg) >> 8) & 0xff);
			packet.push_back(it->second.length & 0xff);
			packet.push_back((it->second.length >> 8) & 0xff);
		}
		numberOfReads++;
	}

	if (numberOfReads == 0) {
		packet.clear();  // Nothing to do.  No supported motors...
	} else {
		finishPacket();
	}

	return packet;
}

const std::vector<unsigned char>& Packet::bulkWrite(
		std::map<Motor*, DataRange>* range) {
	if (type == VER_1_0) {  // Only supported in version 2.0, all motors
		packet.clear();
		return packet;
	}
	startPacket(0xFE);  // broadcast ID
	packet.push_back(Instruction::BULK_WRITE);

	bool needToUpdate;
	int numberOfWrites = 0;
	for (std::map<Motor*, DataRange>::iterator it = range->begin();
			it != range->end(); it++) {
		Motor* dynamixel = it->first;

		needToUpdate = false;
		// TODO: indivdual start reg and nubytes for each motor.
		for (int jj = 0; jj < it->second.length; jj++) { // check to see if we even need to update this motor:
			if (dynamixel->registers[it->second.reg + jj]
					!= dynamixel->registersDesired[it->second.reg + jj]) {
				needToUpdate = true;
				break;
			}
		}
		if (needToUpdate == false) {
			continue;
		}

		packet.push_back(dynamixel->getByte(REG_ID));  // First is the motor ID
		packet.push_back(dynamixel->getMappedRegister(it->second.reg) & 0xff);
		packet.push_back(
				(dynamixel->getMappedRegister(it->second.reg) >> 8) & 0xff);
		packet.push_back(it->second.length & 0xff);
		packet.push_back((it->second.length >> 8) & 0xff);
		for (int jj = 0; jj < it->second.length; ++jj) {
			packet.push_back(dynamixel->registersDesired[it->second.reg + jj]);
		}
		numberOfWrites++;
	}

	if (numberOfWrites == 0) {
		packet.clear();  // Nothing to do.  No supported motors...
	} else {
		finishPacket();
	}

	return packet;
}

unsigned short Mogi::Dynamixel::btous(const unsigned char* data) {
	return (((unsigned short) data[1]) << 8) + (unsigned short) data[0];
}

bool Packet::checkPacket(const std::vector<unsigned char>& data) {
	if (type == VER_1_0) {
		if (data.size() < 6) {  // Smallest packet size check.
//			std::cerr << " data.size() < 6 " << std::endl;
			return false;
		} else if (data[0] != 0xFF || data[1] != 0xFF) { // check that the header exists
//			std::cerr << " Bad header" << std::endl;
			return false;
		} else if (data.size() != (int) (data[3] + 4)) { // check that the length matches
//			std::cerr << " data.size() = " << (int) data.size()
//					<< " != (data[3] + 4) = " << (int) (data[3] + 4)
//					<< std::endl;
			return false;
		}

		return computeChecksum(data.data()) == data[3 + data[3]]; // Check the checksum
	}
	//else if (type == VER_2_0) { // always the case
	if (data.size() < 10) {
		//std::cerr << " data.size() < 10 " << std::endl;
		return false;
	} else if (data[0] != 0xFF ||
			   data[1] != 0xFF ||
			   data[2] != 0xFD ||
			   data[3] != 0x00) {  // check that the length matches
		//std::cerr << " Bad header" << std::endl;
		return false;
	} else if (data.size() != (int) btous(data.data() + 5) + 7) {
		//std::cerr << " data.size() != btous(data.data()+5) + 7 " << std::endl;
		return false;
	}

	unsigned short CRC = update_crc(0, data.data(), data.size() - 2);
	int location = btous(data.data() + 5) + 5;

	return CRC == btous(data.data() + location);
}

#ifdef _cplusplus
}
#endif
