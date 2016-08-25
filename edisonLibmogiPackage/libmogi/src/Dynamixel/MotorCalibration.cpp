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

#ifdef DEBUG
#include <iostream>
#endif

#ifdef IDENT_C
static const char* const Dynamixelcalibration_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Dynamixel;

	void _intToBuffer(int& value, unsigned char* buffer) {
		buffer[0] = value & 0x000000FF;
		buffer[1] = (value & 0x0000FF00) >> 8;
		buffer[2] = (value & 0x00FF0000) >> 16;
		buffer[3] = ((long) value & 0xFF000000) >> 24;
	}

	void MotorCalibration::convert(double position, unsigned char* buffer) {
		int temp;

		if (reverse) {
			position *= -1;
		}
		position -= offset;

		switch (type) {
			case MODEL_MX12W:
			case MODEL_MX28:
			case MODEL_MX64:
			case MODEL_MX106:
			case MODEL_L42_10_S300_R:
				// 0-4095 for 0-360 degrees.  new 0 about 180*pi/180
				// result = (4096 - 0)*(p - 0)/(2pi - 0) + 2048 =
				temp = 651.8986469 * position + 2048;
				buffer[0] = temp & 0xFF;
				buffer[1] = (temp & 0xFF00) >> 8;
				break;

			case MODEL_XM430_W210:
			case MODEL_XM430_W350:
				// 0-4095 for 0-360 degrees.  new 0 about 180*pi/180
				// result = (4096 - 0)*(p - 0)/(2pi - 0) + 2048 =

				temp = 651.8986469 * position + 2048;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_DX113:
			case MODEL_DX116:
			case MODEL_DX117:
			case MODEL_AX12:
			case MODEL_AX18A:
			case MODEL_AX12W:

			case MODEL_RX10:
			case MODEL_RX24:
			case MODEL_RX28:
			case MODEL_RX64:
			case MODEL_XL320:
				// 0-1023 for 0-300 degrees.  new 0 about 150*pi/180
				// result = (1024 - 0)*(p - 0)/(300 * pi/180 - 0) + 512 =
				temp = 195.56959407 * position + 512;
				buffer[0] = temp & 0xFF;
				buffer[1] = (temp & 0xFF00) >> 8;
				break;

			case MODEL_EX106PLUS:
				// 0-4095 for 0-250.92 degrees.  new 0 about 125.46*pi/180
				// result = (4095 - 0)*(p - 0)/(250.92 * pi/180 - 0) + 2048
				temp = 935.06383352 * position + 2048;
				buffer[0] = temp & 0x00FF;
				buffer[1] = (temp & 0xFF00) >> 8;
				break;

			case MODEL_EX106:
				// 0-4095 for 0-280.6 degrees.  new 0 about 140.3*pi/180
				// result = (4095 - 0)*(p - 0)/(280.6 * pi/180 - 0) + 2048
				temp = 836.15900608 * position + 2048;
				buffer[0] = temp & 0xFF;
				buffer[1] = (temp & 0xFF00) >> 8;
				break;

			case MODEL_H54_100_S500_R:
			case MODEL_H54_200_S500_R:
				// Resolution: 501900
				temp = 79879.865938 * position;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_H42_20_S300_R:
				// Resolution: 303800
				temp = 303800 / (3.1415926535897 * 2.0) * position;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_M54_60_S250_R:
			case MODEL_M54_40_S250_R:
				// Resolution: 251400
				temp = 251400 / (3.1415926535897 * 2.0) * position;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_M42_10_S260_R:
				// Resolution: 263168
				temp = 263168 / (3.1415926535897 * 2.0) * position;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_L54_50_S290_R:
				// Resolution: 207720
				temp = 207720 / (3.1415926535897 * 2.0) * position;
				_intToBuffer(temp, buffer);
				break;

			case MODEL_L54_50_S500_R:
			case MODEL_L54_30_S400_R:
			case MODEL_L54_30_S500_R:
				// Resolution: 207720
				temp = 288360 / (3.1415926535897 * 2.0) * position;
				_intToBuffer(temp, buffer);
				break;
		}
	}

	double MotorCalibration::invert(unsigned char* buffer) {
		double position = 0.0;

		switch (type) {
			case MODEL_MX12W:
			case MODEL_MX28:
			case MODEL_MX64:
			case MODEL_MX106:
			case MODEL_L42_10_S300_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8);
				position = (position - 2048) / 651.8986469;
				break;

			case MODEL_XM430_W350:
			case MODEL_XM430_W210:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = (position - 2048) / 651.8986469;
				break;

			case MODEL_DX113:
			case MODEL_DX116:
			case MODEL_DX117:
			case MODEL_AX12:
			case MODEL_AX18A:
			case MODEL_AX12W:

			case MODEL_RX10:
			case MODEL_RX24:
			case MODEL_RX28:
			case MODEL_RX64:
			case MODEL_XL320:
				position = (int) buffer[0] + (((int) buffer[1]) << 8);
				position = (position - 512) / 195.56959407;
				break;

			case MODEL_EX106PLUS:
				position = (int) buffer[0] + (((int) buffer[1]) << 8);
				position = (position - 2048) / 935.06383352;
				break;

			case MODEL_EX106:
				position = (int) buffer[0] + (((int) buffer[1]) << 8);
				position = (position - 2048) / 836.15900608;
				break;

			case MODEL_H54_200_S500_R:
			case MODEL_H54_100_S500_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 501900;
				break;

			case MODEL_H42_20_S300_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 303800;
				break;

			case MODEL_M54_60_S250_R:
			case MODEL_M54_40_S250_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 251400;
				break;

			case MODEL_M42_10_S260_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 263168;
				break;

			case MODEL_L54_50_S290_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 207720;
				break;

			case MODEL_L54_50_S500_R:
			case MODEL_L54_30_S400_R:
			case MODEL_L54_30_S500_R:
				position = (int) buffer[0] + (((int) buffer[1]) << 8)
				+ (((int) buffer[2]) << 16) + (((int) buffer[3]) << 24);
				position = position * (3.1415926535897 * 2.0) / 288360;
				break;
		}
		
		position += offset;
		
		if (reverse) {
			position *= -1;
		}
		
		return position;
	}
	
	void MotorCalibration::setup(Model model) {
		type = model;
	}
	
#ifdef _cplusplus
}
#endif
