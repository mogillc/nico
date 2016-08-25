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

#ifdef IDENT_C
static const char* const Dynamixel_C_Id = "$Id$";
#endif

#include "dynamixel.h"
#include <string.h>
#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Dynamixel;

	void Motor::allocateRegisters(size_t size) {
		if (size != numberOfRegisters) {
			// allocate new size
			unsigned char* newRegisters = new unsigned char[size];
			unsigned char* newRegistersDesired = new unsigned char[size];

			// copy previous values
			memcpy(newRegisters, registers,
				   size < numberOfRegisters ? size : numberOfRegisters);
			memcpy(newRegistersDesired, registersDesired,
				   size < numberOfRegisters ? size : numberOfRegisters);

			// delete old allocations, if needed
			unsigned char* toDelete = registers;
			registers = newRegisters;
			if (toDelete != NULL) {
				delete[] toDelete;
			}

			toDelete = registersDesired;
			registersDesired = newRegistersDesired;
			if (toDelete != NULL) {
				delete[] toDelete;
			}

			numberOfRegisters = size;
		}
	}

	/* Constructors: */
	Motor::Motor(unsigned int ident, Model model) :
	numberOfRegisters(2) {
		buildRegisterMap();
		registers = new unsigned char[2];
		registersDesired = new unsigned char[2];
		registers[0] = model & 0x00FF;
		registers[1] = (model & 0xFF00) >> 8;

		update_vals();

		for (unsigned int i = 2; i < numberOfRegisters; i++) {
			registers[i] = 255;
			registersDesired[i] = 254;
		}

		registers[getMappedRegister(REG_ID)] = ident;
		registersDesired[getMappedRegister(REG_ID)] = ident;
	}

	// Copy constructor
	//Motor::Motor(const Motor& param) :
	//		numberOfRegisters(0), registers(NULL), registersDesired(NULL) {
	//	buildRegisterMap();
	//	allocateRegisters(param.numberOfRegisters);
	//}
	//
	//Motor& Motor::operator=(const Motor& param) {
	//	allocateRegisters(param.numberOfRegisters);
	//
	//	return *this;
	//}

	// default constructor:
	//	Motor::Motor()
	//	:numberOfRegisters(0), registers(NULL), registersDesired(NULL)
	//	{
	//		buildRegisterMap();
	//		allocateRegisters(52);
	//
	//		registers[0] = 94;	// default is XL320
	//		registers[1] = 1;
	//		for( int i = 2; i < numberOfRegisters; i++) {
	//			registers[i] = -1;
	//			registersDesired[i] = -2;
	//		}
	//	}

	/* Default Destructor: */
	Motor::~Motor() {
		if (registers != NULL) {
			delete[] registers;
		}

		if (registers != NULL) {
			delete[] registersDesired;
		}
	}

	void Motor::setGoalPosition(double angle) {
		calibration.convert(angle,
							&registersDesired[getMappedRegister(REG_Goal_Pos)]);
	}

	double Motor::getCurrentPosition() {
		return calibration.invert(&registers[getMappedRegister(REG_Present_Pos)]);
	}

	void Motor::setMovingSpeed(double speed) {
		int result = speed;
		int reg = getMappedRegister(REG_Moving_Speed);
		registersDesired[reg] = result & 0x00ff;
		registersDesired[reg + 1] = result & 0xff00;
	}

	unsigned char Motor::getByte(int reg) {
		return registers[getMappedRegister(reg)];
	}

	void Motor::setByte(int reg, unsigned char newValue) {
		reg = getMappedRegister(reg);
		if (reg < 0 || reg >= numberOfRegisters) {
			std::cerr << "Error, register out of range reg = " << reg
			<< " for motor ID "
			<< (int) registers[getMappedRegister(REG_ID)] << ", model: "
			<< getModel() << std::endl;
		} else {
			registersDesired[reg] = newValue;
		}
	}

	unsigned short Motor::getWord(int reg) {
		reg = getMappedRegister(reg);
		return (int) registers[reg] + (((int) registers[reg + 1]) << 8);
	}

	void Motor::setWord(int reg, unsigned short newValue) {
		reg = getMappedRegister(reg);
		registersDesired[reg] = newValue & 0xFF;
		registersDesired[reg + 1] = (newValue >> 8) & 0xFF;
	}

	int Motor::getInt(int reg) {
		reg = getMappedRegister(reg);
		return (int) registers[reg] | (((int) registers[reg + 1]) << 8)
		| (((int) registers[reg + 2]) << 16)
		| (((int) registers[reg + 3]) << 24);
	}

	void Motor::setInt(int reg, int newValue) {
		reg = getMappedRegister(reg);
		registersDesired[reg] = newValue & 0xFF;
		registersDesired[reg + 1] = (newValue >> 8) & 0xFF;
		registersDesired[reg + 2] = (newValue >> 16) & 0xFF;
		registersDesired[reg + 3] = (newValue >> 24) & 0xFF;
	}

	void Motor::setCalibration(double offset, bool reversed) {
		calibration.setOffset(offset);
		calibration.setReverse(reversed);
	}

	void Motor::update_vals() {
		//		unsigned char currentID = get_value(REG_ID);
		Model model = (Model) getWord(0);
		calibration.setup(model);

		unsigned int newNumberOfRegisterLength = 0;
		switch (model) {
			case MODEL_AX12:
			case MODEL_AX12W:
			case MODEL_AX18A:
			case MODEL_RX10:
			case MODEL_RX24:
			case MODEL_RX28:
			case MODEL_RX64:
			case MODEL_DX113:
			case MODEL_DX116:
			case MODEL_DX117:
				newNumberOfRegisterLength = 50;
				break;

			case MODEL_EX106:
			case MODEL_EX106PLUS:
				newNumberOfRegisterLength = 58;
				break;

			case MODEL_XL320:
				newNumberOfRegisterLength = 52;
				break;

			case MODEL_MX12W:
			case MODEL_MX28:
			case MODEL_MX64:
			case MODEL_MX106:
				newNumberOfRegisterLength = 75;
				break;

			case MODEL_H42_20_S300_R:
			case MODEL_H54_100_S500_R:
			case MODEL_H54_200_S500_R:
			case MODEL_M42_10_S260_R:
			case MODEL_M54_40_S250_R:
			case MODEL_M54_60_S250_R:
			case MODEL_L42_10_S300_R:
			case MODEL_L54_30_S400_R:
			case MODEL_L54_30_S500_R:
			case MODEL_L54_50_S290_R:
			case MODEL_L54_50_S500_R:
				newNumberOfRegisterLength = 894;
				break;

			case MODEL_XM430_W350:
			case MODEL_XM430_W210:
				newNumberOfRegisterLength = 147;
				break;

		}
		allocateRegisters(newNumberOfRegisterLength);

		//		set_value(REG_ID, currentID);
	}

	std::string Mogi::Dynamixel::modelToStr(Model model) {
		switch (model) {
			case MODEL_DX113:
				return "DX113";
			case MODEL_DX116:
				return "DX116";
			case MODEL_DX117:
				return "DX117";

			case MODEL_AX12W:
				return "AX12W";
			case MODEL_AX12:
				return "AX12";
			case MODEL_AX18A:
				return "AX18A";

			case MODEL_RX10:
				return "RX10";
			case MODEL_RX24:
				return "RX24";
			case MODEL_RX28:
				return "RX28";
			case MODEL_RX64:
				return "RX64";

			case MODEL_EX106:
				return "EX106";
			case MODEL_EX106PLUS:
				return "EX106+";

			case MODEL_MX12W:
				return "MX12W";
			case MODEL_MX28:
				return "MX28";
			case MODEL_MX64:
				return "MX64";
			case MODEL_MX106:
				return "MX106";

			case MODEL_XL320:
				return "XL320";

			case MODEL_H54_100_S500_R:
				return "H54_100_S500_R";
			case MODEL_H54_200_S500_R:
				return "H54_200_S500_R";
			case MODEL_H42_20_S300_R:
				return "H42_20_S300_R";
			case MODEL_M54_60_S250_R:
				return "M54_60_S250_R";
			case MODEL_M54_40_S250_R:
				return "M54_40_S250_R";
			case MODEL_M42_10_S260_R:
				return "M42_10_S260_R";
			case MODEL_L54_50_S500_R:
				return "L54_50_S500_R";
			case MODEL_L54_30_S500_R:
				return "L54_30_S500_R";
			case MODEL_L54_50_S290_R:
				return "L54_50_S290_R";
			case MODEL_L54_30_S400_R:
				return "L54_30_S400_R";
			case MODEL_L42_10_S300_R:
				return "L42_10_S300_R";

			case MODEL_XM430_W210:
				return "XM430_W210";
			case MODEL_XM430_W350:
				return "XM430_W350";
		}

		return "Unknown";
	}

	std::string Motor::getModel() {
		return modelToStr((Model) getWord(REG_Model_Number));
	}

	void Motor::addObserver(Observer* observer) {
		bool checkIfExists = false;
		for (std::vector<Observer*>::iterator it = observers.begin();
			 it != observers.end(); it++) {
			if (*it == observer) {
				checkIfExists = true;
				break;
			}
		}
		if (checkIfExists == false) {
			observers.push_back(observer);
		}
	}

	void Motor::removeObserver(Observer* observer) {
		observersToRemove.push_back(observer);
	}

	void Motor::pruneObservers() {
		if (observersToRemove.size() > 0) {
			for (std::vector<Observer*>::iterator it = observersToRemove.begin();
				 it != observersToRemove.end(); it++) {
				for (std::vector<Observer*>::iterator it2 = observers.begin();
					 it2 != observers.end(); it2++) {
					if (*it == *it2) {
						observers.erase(it2);
						break;
					}
				}
			}
			observersToRemove.clear();
		}
	}

	void Motor::notifyObservers(const Instruction* instruction) {
		pruneObservers();
		for (std::vector<Observer*>::iterator it = observers.begin();
			 it != observers.end(); it++) {
			(*it)->update(this, instruction);
		}
	}

	void Motor::notifyObserversOfFailure(const Instruction* instruction,
										 Interface::Status status) {
		pruneObservers();
		for (std::vector<Observer*>::iterator it = observers.begin();
			 it != observers.end(); it++) {
			(*it)->failed(this, instruction, status);
		}
	}

	int Motor::getMappedRegister(int reg) {
		//		std::cerr << "getMappedRegister( " << reg << ") = " <<
		//std::endl;
		int result = reg;

		if (result >= 0x8000) {
			std::map<Model, std::map<int, Register> >::iterator it =
			registerMap.find( (Model) ((int) registers[0] | (((int) registers[1]) << 8)));

			if (it != registerMap.end()) {
				std::map<int, Register>::iterator it2 = it->second.find(reg);
				if (it2 != it->second.end()) {
					result = it2->second;
				}
			} else {
				result -= 0x8000; // What is this register?  OR are we in a different
				// motor type?
			}
		}

		//		std::cerr << " - " << result << std::endl;

		return result;
	}

	std::map<Model, std::map<int, Register> > Motor::registerMap;

	void Motor::buildRegisterMap() {
		if (registerMap.size() == 0) {
			{  // XL320:
				registerMap[MODEL_XL320][REG_Model_Number] = REG_XL_Model_Number;
				registerMap[MODEL_XL320][REG_Firmware_Ver] = REG_XL_Firmware_Ver;
				registerMap[MODEL_XL320][REG_ID] = REG_XL_ID;
				registerMap[MODEL_XL320][REG_Baud] = REG_XL_Baud;
				registerMap[MODEL_XL320][REG_Ret_Delay_Time] = REG_XL_Ret_Delay_Time;
				registerMap[MODEL_XL320][REG_CW_Ang_Lim] = REG_XL_CW_Ang_Lim;
				registerMap[MODEL_XL320][REG_CCW_Ang_Lim] = REG_XL_CCW_Ang_Lim;
				registerMap[MODEL_XL320][REG_Control_Mode] = REG_XL_Control_Mode;
				registerMap[MODEL_XL320][REG_Temp_Lim] = REG_XL_Temp_Lim;
				registerMap[MODEL_XL320][REG_Voltage_Lim_Low] = REG_XL_Voltage_Lim_Low;
				registerMap[MODEL_XL320][REG_Voltage_Lim_Hi] = REG_XL_Voltage_Lim_Hi;
				registerMap[MODEL_XL320][REG_Torque_Max] = REG_XL_Torque_Max;
				registerMap[MODEL_XL320][REG_Stat_Ret_Level] = REG_XL_Stat_Ret_Level;
				registerMap[MODEL_XL320][REG_Alarm_Shutdown] = REG_XL_Alarm_Shutdown;
				registerMap[MODEL_XL320][REG_Torque_EN] = REG_XL_Torque_EN;
				registerMap[MODEL_XL320][REG_LED] = REG_XL_LED;
				registerMap[MODEL_XL320][REG_D_Gain] = REG_XL_D_Gain;
				registerMap[MODEL_XL320][REG_I_Gain] = REG_XL_I_Gain;
				registerMap[MODEL_XL320][REG_P_Gain] = REG_XL_P_Gain;
				registerMap[MODEL_XL320][REG_Goal_Pos] = REG_XL_Goal_Pos;
				registerMap[MODEL_XL320][REG_Moving_Speed] = REG_XL_Goal_Vel;
				registerMap[MODEL_XL320][REG_Torque_Lim] = REG_XL_Goal_Torque;
				registerMap[MODEL_XL320][REG_Present_Pos] = REG_XL_Present_Pos;
				registerMap[MODEL_XL320][REG_Present_Load] = REG_XL_Present_Load;
				registerMap[MODEL_XL320][REG_Present_Speed] = REG_XL_Present_Speed;
				registerMap[MODEL_XL320][REG_Present_Voltage] = REG_XL_Present_Voltage;
				registerMap[MODEL_XL320][REG_Present_Temp] = REG_XL_Present_Temp;
				registerMap[MODEL_XL320][REG_Registered] = REG_XL_Registered;
				registerMap[MODEL_XL320][REG_Moving] = REG_XL_Moving;
				registerMap[MODEL_XL320][REG_Hardware_Error] = REG_XL_Hard_Error;
				registerMap[MODEL_XL320][REG_Punch] = REG_XL_Punch;
			}

			{  // MX-series:
				Model MX_series[] = { MODEL_MX106, MODEL_MX12W, MODEL_MX28,
					MODEL_MX64 };

				for (int i = 0; i < sizeof(MX_series) / sizeof(*MX_series); i++) {
					registerMap[MX_series[i]][REG_Model_Number] = 	REG_MX_Model_Number;
					registerMap[MX_series[i]][REG_Firmware_Ver] = 	REG_MX_Firmware_Ver;
					registerMap[MX_series[i]][REG_ID] = REG_MX_ID;
					registerMap[MX_series[i]][REG_Baud] = REG_MX_Baud;
					registerMap[MX_series[i]][REG_Ret_Delay_Time] = 	REG_MX_Ret_Delay_Time;
					registerMap[MX_series[i]][REG_CW_Ang_Lim] = REG_MX_CW_Ang_Lim;
					registerMap[MX_series[i]][REG_CCW_Ang_Lim] = REG_MX_CCW_Ang_Lim;
					registerMap[MX_series[i]][REG_Temp_Lim] = REG_MX_Temp_Lim;
					registerMap[MX_series[i]][REG_Voltage_Lim_Low] = 	REG_MX_Voltage_Lim_Low;
					registerMap[MX_series[i]][REG_Voltage_Lim_Hi] = 	REG_MX_Voltage_Lim_Hi;
					registerMap[MX_series[i]][REG_Torque_Max] = REG_MX_Torque_Max;
					registerMap[MX_series[i]][REG_Stat_Ret_Level] = 	REG_MX_Stat_Ret_Level;
					registerMap[MX_series[i]][REG_Alarm_LED] = REG_MX_Alarm_LED;
					registerMap[MX_series[i]][REG_Alarm_Shutdown] = 	REG_MX_Alarm_Shutdown;
					registerMap[MX_series[i]][REG_Multi_Turn_Offset] = 	REG_MX_Multi_Turn_Offset;
					registerMap[MX_series[i]][REG_Resolution_Divider] = 	REG_MX_Resolution_Divider;
					registerMap[MX_series[i]][REG_Torque_EN] = REG_MX_Torque_EN;
					registerMap[MX_series[i]][REG_LED] = REG_MX_LED;
					registerMap[MX_series[i]][REG_D_Gain] = REG_MX_D_Gain;
					registerMap[MX_series[i]][REG_I_Gain] = REG_MX_I_Gain;
					registerMap[MX_series[i]][REG_P_Gain] = REG_MX_P_Gain;
					registerMap[MX_series[i]][REG_Goal_Pos] = REG_MX_Goal_Pos;
					registerMap[MX_series[i]][REG_Moving_Speed] = REG_MX_Goal_Vel;
					registerMap[MX_series[i]][REG_Torque_Lim] = REG_MX_Goal_Torque;
					registerMap[MX_series[i]][REG_Present_Pos] = REG_MX_Present_Pos;
					registerMap[MX_series[i]][REG_Present_Speed] = 	REG_MX_Present_Speed;
					registerMap[MX_series[i]][REG_Present_Load] = 	REG_MX_Present_Load;
					registerMap[MX_series[i]][REG_Present_Voltage] = 	REG_MX_Present_Voltage;
					registerMap[MX_series[i]][REG_Present_Temp] = 	REG_MX_Present_Temp;
					registerMap[MX_series[i]][REG_Registered] = REG_MX_Registered;
					registerMap[MX_series[i]][REG_Moving] = REG_MX_Moving;
					registerMap[MX_series[i]][REG_Lock] = REG_MX_Lock;
					registerMap[MX_series[i]][REG_Punch] = REG_MX_Punch;
					registerMap[MX_series[i]][REG_Goal_Acceleration] = 	REG_MX_Goal_Acceleration;
				}
				registerMap[MODEL_MX106][REG_Drive_Mode] = REG_MX106_Drive_Mode;
				registerMap[MODEL_MX106][REG_Current] = REG_MX106_Current;
				registerMap[MODEL_MX106][REG_Torque_Mode_EN] = REG_MX106_Torque_Mode_EN;
				registerMap[MODEL_MX106][REG_Goal_Torque] = REG_MX106_Goal_Torque;
			}

			{  // AX/RX/DX/EX-series:
				Model AX_series[] = { MODEL_AX12, MODEL_AX12W, MODEL_AX18A,
					MODEL_RX10, MODEL_RX24, MODEL_RX28, MODEL_RX64, MODEL_DX113,
					MODEL_DX116, MODEL_DX117, MODEL_EX106, MODEL_EX106PLUS, };

				for (int i = 0; i < sizeof(AX_series) / sizeof(*AX_series); i++) {
					registerMap[AX_series[i]][REG_Model_Number] = 	REG_AX_Model_Number;
					registerMap[AX_series[i]][REG_Firmware_Ver] = 	REG_AX_Firmware_Ver;
					registerMap[AX_series[i]][REG_ID] = REG_AX_ID;
					registerMap[AX_series[i]][REG_Baud] = REG_AX_Baud;
					registerMap[AX_series[i]][REG_Ret_Delay_Time] = 	REG_AX_Ret_Delay_Time;
					registerMap[AX_series[i]][REG_CW_Ang_Lim] = REG_AX_CW_Ang_Lim;
					registerMap[AX_series[i]][REG_CCW_Ang_Lim] = REG_AX_CCW_Ang_Lim;
					registerMap[AX_series[i]][REG_Temp_Lim] = REG_AX_Temp_Lim;
					registerMap[AX_series[i]][REG_Voltage_Lim_Low] = 	REG_AX_Voltage_Lim_Low;
					registerMap[AX_series[i]][REG_Voltage_Lim_Hi] = 	REG_AX_Voltage_Lim_Hi;
					registerMap[AX_series[i]][REG_Torque_Max] = REG_AX_Torque_Max;
					registerMap[AX_series[i]][REG_Stat_Ret_Level] = 	REG_AX_Stat_Ret_Level;
					registerMap[AX_series[i]][REG_Alarm_LED] = REG_AX_Alarm_LED;
					registerMap[AX_series[i]][REG_Alarm_Shutdown] = 	REG_AX_Alarm_Shutdown;

					registerMap[AX_series[i]][REG_Torque_EN] = REG_AX_Torque_EN;
					registerMap[AX_series[i]][REG_LED] = REG_AX_LED;
					registerMap[AX_series[i]][REG_CW_MARGIN] = REG_AX_CW_Margin;
					registerMap[AX_series[i]][REG_CCW_MARGIN] = REG_AX_CCW_Margin;
					registerMap[AX_series[i]][REG_CW_SLOPE] = REG_AX_CW_Slope;
					registerMap[AX_series[i]][REG_CCW_SLOPE] = REG_AX_CCW_Slope;
					registerMap[AX_series[i]][REG_Goal_Pos] = REG_AX_Goal_Pos;
					registerMap[AX_series[i]][REG_Moving_Speed] = REG_AX_Goal_Vel;
					registerMap[AX_series[i]][REG_Torque_Lim] = REG_AX_Goal_Torque;
					registerMap[AX_series[i]][REG_Present_Pos] = REG_AX_Present_Pos;
					registerMap[AX_series[i]][REG_Present_Speed] = 	REG_AX_Present_Speed;
					registerMap[AX_series[i]][REG_Present_Load] = 	REG_AX_Present_Load;
					registerMap[AX_series[i]][REG_Present_Voltage] = 	REG_AX_Present_Voltage;
					registerMap[AX_series[i]][REG_Present_Temp] = 	REG_AX_Present_Temp;
					registerMap[AX_series[i]][REG_Registered] = REG_AX_Registered;
					registerMap[AX_series[i]][REG_Moving] = REG_AX_Moving;
					registerMap[AX_series[i]][REG_Lock] = REG_AX_Lock;
					registerMap[AX_series[i]][REG_Punch] = REG_AX_Punch;
				}

				registerMap[MODEL_EX106][REG_Current] = REG_EX106_Current;
				registerMap[MODEL_EX106PLUS][REG_Current] = REG_EX106_Current;
			}

			{  // Pro-series:
				Model AX_series[] = { MODEL_H42_20_S300_R, MODEL_H54_100_S500_R,
					MODEL_H54_200_S500_R, MODEL_M54_40_S250_R,
					MODEL_M54_60_S250_R, MODEL_L54_30_S400_R,
					MODEL_L54_30_S500_R, MODEL_L54_50_S290_R,
					MODEL_L54_50_S500_R, };
				for (int i = 0; i < sizeof(AX_series) / sizeof(*AX_series); i++) {
					registerMap[AX_series[i]][REG_Model_Number] = 	REG_PRO_Model_Number;
					registerMap[AX_series[i]][REG_Model_Info] = REG_PRO_Model_Info;
					registerMap[AX_series[i]][REG_Firmware_Ver] = 	REG_PRO_Firmware_Ver;
					registerMap[AX_series[i]][REG_ID] = REG_PRO_ID;
					registerMap[AX_series[i]][REG_Baud] = REG_PRO_Baud;
					registerMap[AX_series[i]][REG_Ret_Delay_Time] = 	REG_PRO_Ret_Delay_Time;
					registerMap[AX_series[i]][REG_Operating_Mode] = 	REG_PRO_Operating_Mode;
					registerMap[AX_series[i]][REG_Homing_Offset] = 	REG_PRO_Homing_Offset;
					registerMap[AX_series[i]][REG_Moving_Thresh] = 	REG_PRO_Moving_Thresh;
					registerMap[AX_series[i]][REG_Temp_Lim] = REG_PRO_Temp_Lim;
					registerMap[AX_series[i]][REG_Voltage_Lim_Hi] = 	REG_PRO_Voltage_Lim_Hi;
					registerMap[AX_series[i]][REG_Voltage_Lim_Low] = 	REG_PRO_Voltage_Lim_Low;
					registerMap[AX_series[i]][REG_Accelerate_Lim] = 	REG_PRO_Accelerate_Lim;
					registerMap[AX_series[i]][REG_Torque_Max] = REG_PRO_Torque_Max;
					registerMap[AX_series[i]][REG_Velocity_Lim] = 	REG_PRO_Velocity_Lim;
					registerMap[AX_series[i]][REG_Max_Position_Lim] = 	REG_PRO_Max_Position_Lim;
					registerMap[AX_series[i]][REG_Min_Position_Lim] = 	REG_PRO_Min_Position_Lim;
					registerMap[AX_series[i]][REG_Ext_Port_Mode_1] = 	REG_PRO_Ext_Port_Mode_1;
					registerMap[AX_series[i]][REG_Ext_Port_Mode_2] = 	REG_PRO_Ext_Port_Mode_2;
					registerMap[AX_series[i]][REG_Ext_Port_Mode_3] = 	REG_PRO_Ext_Port_Mode_3;
					registerMap[AX_series[i]][REG_Ext_Port_Mode_4] = 	REG_PRO_Ext_Port_Mode_4;
					registerMap[AX_series[i]][REG_Alarm_Shutdown] = 	REG_PRO_Shutdown;
					//					for (int x = 1; x <= 256; x++) {
					//						registerMap[AX_series[i]][REG_Indirect_Add(x)]
					//= REG_PRO_Indirect_Add(x);
					//					}

					registerMap[AX_series[i]][REG_Torque_EN] = REG_PRO_Torque_EN;
					registerMap[AX_series[i]][REG_LED_Red] = REG_PRO_LED_Red;
					registerMap[AX_series[i]][REG_LED_Green] = REG_PRO_LED_Green;
					registerMap[AX_series[i]][REG_LED_Blue] = REG_PRO_LED_Blue;
					registerMap[AX_series[i]][REG_Vel_I_Gain] = REG_PRO_Vel_I_Gain;
					registerMap[AX_series[i]][REG_Vel_P_Gain] = REG_PRO_Vel_P_Gain;
					registerMap[AX_series[i]][REG_P_Gain] = REG_PRO_Pos_P_Gain;
					registerMap[AX_series[i]][REG_Goal_Pos] = REG_PRO_Goal_Pos;
					registerMap[AX_series[i]][REG_Moving_Speed] = REG_PRO_Goal_Vel;
					registerMap[AX_series[i]][REG_Goal_Torque] = 	REG_PRO_Goal_Torque;
					registerMap[AX_series[i]][REG_Goal_Acceleration] = 	REG_PRO_Goal_Acceleration;
					registerMap[AX_series[i]][REG_Moving] = REG_PRO_Moving;
					registerMap[AX_series[i]][REG_Present_Pos] = 	REG_PRO_Present_Pos;
					registerMap[AX_series[i]][REG_Present_Speed] = 	REG_PRO_Present_Speed;
					registerMap[AX_series[i]][REG_Current] = 	REG_PRO_Present_Current;
					registerMap[AX_series[i]][REG_Present_Voltage] = 	REG_PRO_Present_Voltage;
					registerMap[AX_series[i]][REG_Present_Temp] = 	REG_PRO_Present_Temp;
					registerMap[AX_series[i]][REG_Ext_Port_Data_1] = 	REG_PRO_Ext_Port_Data_1;
					registerMap[AX_series[i]][REG_Ext_Port_Data_2] = 	REG_PRO_Ext_Port_Data_2;
					registerMap[AX_series[i]][REG_Ext_Port_Data_3] = 	REG_PRO_Ext_Port_Data_3;
					registerMap[AX_series[i]][REG_Ext_Port_Data_4] = 	REG_PRO_Ext_Port_Data_4;
					//					registerMap[AX_series[i]][REG_Model_Number]
					//= REG_PRO_Indirect_Data(x)	((x)+633)	// up to 256
					registerMap[AX_series[i]][REG_Registered] = REG_PRO_Registered;
					registerMap[AX_series[i]][REG_Stat_Ret_Level] = 	REG_PRO_Stat_Ret_Level;
					registerMap[AX_series[i]][REG_Hardware_Error] = 	REG_PRO_Hard_Error;
				}

				{  // XM-series:
					Model XM_series[] = { MODEL_XM430_W210, MODEL_XM430_W350 };
					for (int i = 0; i < sizeof(XM_series) / sizeof(*XM_series); i++) {
						registerMap[XM_series[i]][REG_Model_Number] = 	REG_XM_Model_Number;
						registerMap[XM_series[i]][REG_Model_Info] = REG_XM_Model_Info;
						registerMap[XM_series[i]][REG_Firmware_Ver] = 	REG_XM_Firmware_Ver;
						registerMap[XM_series[i]][REG_ID] = REG_XM_ID;
						registerMap[XM_series[i]][REG_Baud] = REG_XM_Baud;
						registerMap[XM_series[i]][REG_Ret_Delay_Time] = 	REG_XM_Ret_Delay_Time;
						registerMap[XM_series[i]][REG_Operating_Mode] = 	REG_XM_Operating_Mode;
						registerMap[XM_series[i]][REG_Homing_Offset] = 	REG_XM_Homing_Offset;
						registerMap[XM_series[i]][REG_Moving_Thresh] = 	REG_XM_Moving_Threshold;
						registerMap[XM_series[i]][REG_Temp_Lim] = REG_XM_Temp_Lim;
						registerMap[XM_series[i]][REG_Voltage_Lim_Hi] = 	REG_XM_Voltage_Lim_Hi;
						registerMap[XM_series[i]][REG_Voltage_Lim_Low] = 	REG_XM_Voltage_Lim_Low;
						registerMap[XM_series[i]][REG_Accelerate_Lim] = 	REG_XM_Accelerate_Lim;
						//registerMap[XM_series[i]][REG_Torque_Max] = REG_XM_Torque_Max;
						registerMap[XM_series[i]][REG_Velocity_Lim] = 	REG_XM_Velocity_Lim;
						registerMap[XM_series[i]][REG_Max_Position_Lim] = 	REG_XM_Max_Position_Lim;
						registerMap[XM_series[i]][REG_Min_Position_Lim] = 	REG_XM_Min_Position_Lim;
						registerMap[XM_series[i]][REG_Alarm_Shutdown] = 	REG_XM_Shutdown;
						//					for (int x = 1; x <= 256; x++) {
						//						registerMap[XM_series[i]][REG_Indirect_Add(x)]
						//= REG_XM_Indirect_Add(x);
						//					}

						registerMap[XM_series[i]][REG_Torque_EN] = REG_XM_Torque_EN;
						registerMap[XM_series[i]][REG_LED] = REG_XM_LED;
						registerMap[XM_series[i]][REG_Vel_I_Gain] = REG_XM_Vel_I_Gain;
						registerMap[XM_series[i]][REG_Vel_P_Gain] = REG_XM_Vel_P_Gain;
						registerMap[XM_series[i]][REG_P_Gain] = REG_XM_Pos_P_Gain;
						registerMap[XM_series[i]][REG_Goal_Pos] = REG_XM_Goal_Pos;
						registerMap[XM_series[i]][REG_Moving_Speed] = REG_XM_Goal_Vel;
						//					registerMap[XM_series[i]][REG_Goal_Torque] = 	REG_XM_Goal_Torque;
						//					registerMap[XM_series[i]][REG_Goal_Acceleration] = 	REG_XM_Goal_Acceleration;
						registerMap[XM_series[i]][REG_Moving] = REG_XM_Moving;
						registerMap[XM_series[i]][REG_Present_Pos] = 	REG_XM_Present_Pos;
						registerMap[XM_series[i]][REG_Present_Speed] = 	REG_XM_Present_Speed;
						registerMap[XM_series[i]][REG_Current] = 	REG_XM_Present_Current;
						registerMap[XM_series[i]][REG_Present_Voltage] = 	REG_XM_Present_Voltage;
						registerMap[XM_series[i]][REG_Present_Temp] = 	REG_XM_Present_Temp;
						//					registerMap[XM_series[i]][REG_Ext_Port_Data_1] = 	REG_XM_Ext_Port_Data_1;
						//					registerMap[XM_series[i]][REG_Ext_Port_Data_2] = 	REG_XM_Ext_Port_Data_2;
						//					registerMap[XM_series[i]][REG_Ext_Port_Data_3] = 	REG_XM_Ext_Port_Data_3;
						//					registerMap[XM_series[i]][REG_Ext_Port_Data_4] = 	REG_XM_Ext_Port_Data_4;
						//					registerMap[XM_series[i]][REG_Model_Number]
						//= REG_XM_Indirect_Data(x)	((x)+633)	// up to 256
						//					registerMap[XM_series[i]][REG_Registered] = REG_XM_Registered;
						//					registerMap[XM_series[i]][REG_Stat_Ret_Level] = 	REG_XM_Stat_Ret_Level;
						//					registerMap[XM_series[i]][REG_Hardware_Error] = 	REG_XM_Hard_Error;
					}
				}
			}
		}
	}
	
	int Motor::getNumberOfRegisters() {
		return numberOfRegisters;
	}
	
#ifdef _cplusplus
}
#endif
