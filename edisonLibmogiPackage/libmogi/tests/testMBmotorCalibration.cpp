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
#include <iostream>
#include "mmath.h"

#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

using namespace Mogi::Dynamixel;

bool testCenters();
bool testZeros();
bool testReverse();
bool testBadMotor();
bool testInversion();

Model models[] = {
		// The motors to be tested.
		MODEL_DX113, MODEL_DX116, MODEL_DX117, MODEL_AX12, MODEL_AX12W,
		MODEL_AX18A, MODEL_RX10, MODEL_RX24, MODEL_RX28, MODEL_RX64,
		MODEL_EX106, MODEL_EX106PLUS, MODEL_L42_10_S300_R, MODEL_MX64,
		MODEL_MX28, MODEL_MX106, MODEL_MX12W, MODEL_XL320, MODEL_H54_200_S500_R,
		MODEL_H54_100_S500_R, MODEL_H42_20_S300_R, MODEL_L54_30_S400_R,
		MODEL_L54_30_S500_R, MODEL_L54_50_S290_R, MODEL_L54_50_S500_R,
		MODEL_M42_10_S260_R, MODEL_M54_40_S250_R, MODEL_M54_60_S250_R, };

int main(int argc, char *argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning motor center tests:" << std::endl;
	allTestsPass = testCenters() ? allTestsPass : false;
	std::cout << " - Beginning motor zero tests:" << std::endl;
	allTestsPass = testZeros() ? allTestsPass : false;
	std::cout << " - Beginning reverse zero tests:" << std::endl;
	allTestsPass = testReverse() ? allTestsPass : false;
	std::cout << " - Beginning bad motor tests:" << std::endl;
	allTestsPass = testBadMotor() ? allTestsPass : false;
	std::cout << " - Beginning inversion tests:" << std::endl;
	allTestsPass = testInversion() ? allTestsPass : false;

	std::cout << " - Done." << std::endl;
	if (allTestsPass) {
		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

bool testBadMotor() {
	bool allTestsPass = true;
	MotorCalibration mc;
	unsigned char result[32];
	result[0] = 24;
	result[1] = 87;

	std::cout << "Testing " << modelToStr((Model) -1) << " ............. ";
	mc.setup((Model) (-1));
	mc.convert(0, result);
	if (result[0] == 24 && result[1] == 87) {
		std::cout << "Passed" << std::endl;
	} else {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	}

	return allTestsPass;
}

bool testCenters() {  // currently a very weak test
	bool allTestsPass = true;
	MotorCalibration mc;
	unsigned char result[32];

	for (int i = 0; i < sizeof(models) / sizeof(Model); i++) {
		mc.setup(models[i]);
		result[0] = 34;
		result[1] = 31;
		result[2] = 55;
		result[3] = 56;

		unsigned char expected[] = { 1, 1, 1, 1 };

		switch (models[i]) {
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
			expected[0] = 0;
			expected[1] = 2;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_MX106:
		case MODEL_MX12W:
		case MODEL_MX28:
		case MODEL_MX64:
		case MODEL_EX106PLUS:
		case MODEL_EX106:
		case MODEL_L42_10_S300_R:
			expected[0] = 0;
			expected[1] = 8;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_H54_200_S500_R:
		case MODEL_H54_100_S500_R:
		case MODEL_H42_20_S300_R:
		case MODEL_L54_30_S400_R:
		case MODEL_L54_30_S500_R:
		case MODEL_L54_50_S290_R:
		case MODEL_L54_50_S500_R:
		case MODEL_M42_10_S260_R:
		case MODEL_M54_40_S250_R:
		case MODEL_M54_60_S250_R:
			expected[0] = 0;
			expected[1] = 0;
			expected[2] = 0;
			expected[3] = 0;
			break;
		}

		std::cout << "Testing " << modelToStr(models[i]) << " ";
		for (int j = 0; j < (20 - modelToStr(models[i]).length()); j++) {
			std::cout << ".";
		}
		std::cout << " ";

		mc.convert(0, result);
		if (result[0] == expected[0] && result[1] == expected[1]
				&& result[2] == expected[2] && result[3] == expected[3]) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED" << std::endl;
			allTestsPass = false;
		}
	}

	return allTestsPass;
}

bool testZeros() {  // currently a very weak test
	bool allTestsPass = true;
	MotorCalibration mc;
	unsigned char result[32];

	double angle;

	for (int i = 0; i < sizeof(models) / sizeof(Model); i++) {
		mc.setup(models[i]);
		result[0] = 34;
		result[1] = 31;
		result[2] = 55;
		result[3] = 56;

		unsigned char expected[] = { 0, 0, 0, 0 };

		switch (models[i]) {
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
			angle = -150 * MOGI_PI / 180.0;
			expected[0] = 0;
			expected[1] = 0;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_MX106:
		case MODEL_MX12W:
		case MODEL_MX28:
		case MODEL_MX64:
		case MODEL_L42_10_S300_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0;
			expected[1] = 0;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_EX106PLUS:
			angle = -125.46 * MOGI_PI / 180.0;
			expected[0] = 0;
			expected[1] = 0;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_EX106:
			angle = -140.3 * MOGI_PI / 180.0;
			expected[0] = 0;
			expected[1] = 0;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_H54_200_S500_R:
		case MODEL_H54_100_S500_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xBA;
			expected[1] = 0x2B;
			expected[2] = 0xFC;
			expected[3] = 0xFF;
			break;

		case MODEL_H42_20_S300_R:  // FFFDAEA4
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xA4;
			expected[1] = 0xAE;
			expected[2] = 0xFD;
			expected[3] = 0xFF;
			break;

		case MODEL_L54_50_S500_R:
		case MODEL_L54_30_S400_R:  // FFFDCCCC
		case MODEL_L54_30_S500_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xCC;
			expected[1] = 0xCC;
			expected[2] = 0xFD;
			expected[3] = 0xFF;
			break;

		case MODEL_L54_50_S290_R:  // FFFE6A4C
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x4C;
			expected[1] = 0x6A;
			expected[2] = 0xFE;
			expected[3] = 0xFF;
			break;

		case MODEL_M42_10_S260_R:  // FFFDFE00
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x00;
			expected[1] = 0xFE;
			expected[2] = 0xFD;
			expected[3] = 0xFF;
			break;

		case MODEL_M54_40_S250_R:  // FFFE14FC
		case MODEL_M54_60_S250_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xFC;
			expected[1] = 0x14;
			expected[2] = 0xFE;
			expected[3] = 0xFF;
			break;
		}

		std::cout << "Testing " << modelToStr(models[i]) << " ";
		for (int j = 0; j < (20 - modelToStr(models[i]).length()); j++) {
			std::cout << ".";
		}
		std::cout << " ";

		mc.convert(angle, result);
		if (result[0] == expected[0] && result[1] == expected[1]
				&& result[2] == expected[2] && result[3] == expected[3]) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED" << std::endl;
			std::cout << " | -- result[0] = " << (int) result[0]
					<< "\texpected[0] = " << (int) expected[0] << std::endl;
			std::cout << " | -- result[1] = " << (int) result[1]
					<< "\texpected[1] = " << (int) expected[1] << std::endl;
			std::cout << " | -- result[2] = " << (int) result[2]
					<< "\texpected[2] = " << (int) expected[2] << std::endl;
			std::cout << " | -- result[3] = " << (int) result[3]
					<< "\texpected[3] = " << (int) expected[3] << std::endl;
			allTestsPass = false;
		}
	}

	return allTestsPass;
}

bool testReverse() {
	bool allTestsPass = true;
	MotorCalibration mc;
	unsigned char result[32];

	mc.setReverse(true);

	double angle;

	for (int i = 0; i < sizeof(models) / sizeof(Model); i++) {
		mc.setup(models[i]);
		result[0] = 34;
		result[1] = 31;
		result[2] = 55;
		result[3] = 56;

		unsigned char expected[] = { 0, 0, 0, 0 };

		switch (models[i]) {
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
			angle = -150 * MOGI_PI / 180.0;
			expected[0] = 0xFF;
			expected[1] = 0x03;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_MX106:
		case MODEL_MX12W:
		case MODEL_MX28:
		case MODEL_MX64:
		case MODEL_L42_10_S300_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xFF;
			expected[1] = 0x0F;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_EX106PLUS:
			angle = -125.46 * MOGI_PI / 180.0;
			expected[0] = 0xFF;
			expected[1] = 0x0F;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_EX106:
			angle = -140.3 * MOGI_PI / 180.0;
			expected[0] = 0xFF;
			expected[1] = 0x0F;
			expected[2] = 55;  // untouched
			expected[3] = 56;  // untouched
			break;

		case MODEL_H54_200_S500_R:  // 0003D446
		case MODEL_H54_100_S500_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x46;
			expected[1] = 0xD4;
			expected[2] = 0x03;
			expected[3] = 0x00;
			break;

		case MODEL_H42_20_S300_R:  // 0002515C
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x5C;
			expected[1] = 0x51;
			expected[2] = 0x02;
			expected[3] = 0x00;
			break;

		case MODEL_L54_50_S500_R:
		case MODEL_L54_30_S400_R:  // 00023334
		case MODEL_L54_30_S500_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x34;
			expected[1] = 0x33;
			expected[2] = 0x02;
			expected[3] = 0x00;
			break;

		case MODEL_L54_50_S290_R:  // 000195B4
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0xB4;
			expected[1] = 0x95;
			expected[2] = 0x01;
			expected[3] = 0x00;
			break;

		case MODEL_M42_10_S260_R:  // 00020200
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x00;
			expected[1] = 0x02;
			expected[2] = 0x02;
			expected[3] = 0x00;
			break;

		case MODEL_M54_40_S250_R:  // 0001EB04
		case MODEL_M54_60_S250_R:
			angle = -180 * MOGI_PI / 180.0;
			expected[0] = 0x04;
			expected[1] = 0xEB;
			expected[2] = 0x01;
			expected[3] = 0x00;
			break;
		}

		std::cout << "Testing " << modelToStr(models[i]) << " ";
		for (int j = 0; j < (20 - modelToStr(models[i]).length()); j++) {
			std::cout << ".";
		}
		std::cout << " ";

		mc.convert(angle, result);
		if (result[0] == expected[0] && result[1] == expected[1]
				&& result[2] == expected[2] && result[3] == expected[3]) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED" << std::endl;
			std::cout << " | -- result[0] = " << (int) result[0]
					<< "\texpected[0] = " << (int) expected[0] << std::endl;
			std::cout << " | -- result[1] = " << (int) result[1]
					<< "\texpected[1] = " << (int) expected[1] << std::endl;
			std::cout << " | -- result[2] = " << (int) result[2]
					<< "\texpected[2] = " << (int) expected[2] << std::endl;
			std::cout << " | -- result[3] = " << (int) result[3]
					<< "\texpected[3] = " << (int) expected[3] << std::endl;
			allTestsPass = false;
		}
	}

	return allTestsPass;
}

bool testInversion() {
	bool allTestsPass = true;
	MotorCalibration mc;
	unsigned char buffer[4];

	mc.setReverse(true);

	double expected;
	double result;

	double offset = 0.01;
	mc.setOffset(offset);

	for (int i = 0; i < sizeof(models) / sizeof(Model); i++) {
		mc.setup(models[i]);
		buffer[0] = 34;
		buffer[1] = 31;
		buffer[2] = 55;
		buffer[3] = 56;

		mc.setReverse(true);

		switch (models[i]) {
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
			mc.setReverse(false);
			expected = 0 * MOGI_PI / 180.0 + offset;
			buffer[0] = 0x00;
			buffer[1] = 0x02;
			break;

		case MODEL_MX106:
		case MODEL_MX12W:
		case MODEL_MX28:
		case MODEL_MX64:
		case MODEL_L42_10_S300_R:
			mc.setReverse(false);
			expected = 0.0 * MOGI_PI / 180.0 + offset;
			buffer[0] = 0x00;
			buffer[1] = 0x08;
			break;

		case MODEL_EX106PLUS:
			mc.setReverse(false);
			expected = 0.0 * MOGI_PI / 180.0 + offset;
			buffer[0] = 0x00;
			buffer[1] = 0x08;
			break;

		case MODEL_EX106:
			mc.setReverse(false);
			expected = 0.0 * MOGI_PI / 180.0 + offset;
			buffer[0] = 0x00;
			buffer[1] = 0x08;
			break;

		case MODEL_H54_200_S500_R:  // 0003D446
		case MODEL_H54_100_S500_R:
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0x46;
			buffer[1] = 0xD4;
			buffer[2] = 0x03;
			buffer[3] = 0x00;
			break;

		case MODEL_H42_20_S300_R:  // 0002515C
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0x5C;
			buffer[1] = 0x51;
			buffer[2] = 0x02;
			buffer[3] = 0x00;
			break;

		case MODEL_L54_50_S500_R:
		case MODEL_L54_30_S400_R:  // 00023334
		case MODEL_L54_30_S500_R:
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0x34;
			buffer[1] = 0x33;
			buffer[2] = 0x02;
			buffer[3] = 0x00;
			break;

		case MODEL_L54_50_S290_R:  // 000195B4
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0xB4;
			buffer[1] = 0x95;
			buffer[2] = 0x01;
			buffer[3] = 0x00;
			break;

		case MODEL_M42_10_S260_R:  // 00020200
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0x00;
			buffer[1] = 0x02;
			buffer[2] = 0x02;
			buffer[3] = 0x00;
			break;

		case MODEL_M54_40_S250_R:  // 0001EB04
		case MODEL_M54_60_S250_R:
			expected = -180 * MOGI_PI / 180.0 - offset;
			buffer[0] = 0x04;
			buffer[1] = 0xEB;
			buffer[2] = 0x01;
			buffer[3] = 0x00;
			break;
		}

		std::cout << "Testing " << modelToStr(models[i]) << " ";
		for (int j = 0; j < (20 - modelToStr(models[i]).length()); j++) {
			std::cout << ".";
		}
		std::cout << " ";

		result = mc.invert(buffer);
		if (result == expected) {
			std::cout << "Passed" << std::endl;
		} else {
			std::cout << "FAILED" << std::endl;
			std::cout << " | -- result = " << result << "\texpected = "
					<< expected << std::endl;
			allTestsPass = false;
		}
	}

	return allTestsPass;
}
