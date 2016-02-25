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

#ifndef MOGI_DYNAMIXEL_H
#define MOGI_DYNAMIXEL_H

#include <vector>
#include <map>
#include <string>

#ifdef LIBFTDI_FOUND
#include <ftdi.h>
#endif

#include "mogi/thread.h"

namespace Mogi {

/**
 * @namespace Mogi::Dynamixel
 * \brief Tools for Robotis Dynamixel communication.
 */
namespace Dynamixel {

/*! \brief The model types of supported Dynamixel Motors.
 */
typedef enum {
	MODEL_DX113 = 113, /*!< Robotis Dynamixel DX-113. */
	MODEL_DX116 = 116, /*!< Robotis Dynamixel DX-116. */
	MODEL_DX117 = 117, /*!< Robotis Dynamixel DX-117. */

	MODEL_AX12W = 44, /*!< Robotis Dynamixel AX-12W. */
	MODEL_AX12 = 12, /*!< Robotis Dynamixel AX-12. */
	MODEL_AX18A = 18, /*!< Robotis Dynamixel AX-12A. */

	MODEL_RX10 = 10, /*!< Robotis Dynamixel RX-10. */
	MODEL_RX24 = 24, /*!< Robotis Dynamixel RX-24. */
	MODEL_RX28 = 28, /*!< Robotis Dynamixel RX-28. */
	MODEL_RX64 = 64, /*!< Robotis Dynamixel RX-64. */

	MODEL_EX106 = 106, /*!< Robotis Dynamixel EX-106. */
	MODEL_EX106PLUS = 107, /*!< Robotis Dynamixel EX-106+. */

	MODEL_MX12W = 360, /*!< Robotis Dynamixel MX-12W. */
	MODEL_MX28 = 29, /*!< Robotis Dynamixel MX-28. */
	MODEL_MX64 = 310, /*!< Robotis Dynamixel MX-64. */
	MODEL_MX106 = 320, /*!< Robotis Dynamixel MX-106. */

	MODEL_XL320 = 350, /*!< Robotis Dynamixel XL-320. */

	MODEL_H54_100_S500_R = 53768, /*!< Robotis Dynamixel H54-100-S500-R. */
	MODEL_H54_200_S500_R = 54024, /*!< Robotis Dynamixel H54-200-S500-R. */
	MODEL_H42_20_S300_R = 51200, /*!< Robotis Dynamixel H42-20-S300-R. */
	MODEL_M54_60_S250_R = 46352, /*!< Robotis Dynamixel M54-60-S250-R. */
	MODEL_M54_40_S250_R = 46096, /*!< Robotis Dynamixel M54-40-S250-R. */
	MODEL_M42_10_S260_R = 43288, /*!< Robotis Dynamixel M42-10-S260-R. */
	MODEL_L54_50_S500_R = 38152, /*!< Robotis Dynamixel L54-50-S500-R. */
	MODEL_L54_30_S500_R = 37896, /*!< Robotis Dynamixel L54-30-S500-R. */
	MODEL_L54_50_S290_R = 38176, /*!< Robotis Dynamixel L54-50-S290-R. */
	MODEL_L54_30_S400_R = 37928, /*!< Robotis Dynamixel L54-30-S400-R. */
	MODEL_L42_10_S300_R = 35072 /*!< Robotis Dynamixel L42-10-S300-R. */
} Model;

/*! \brief Defines the user configurable register locations of Robotis motors

 Defines the user configurable register locations of Robotis motors.  This list mainly supports the MX series of motors after firmware version 30.
 Note: The numbers represented by this enumeration do not represent the address locations of the dynamixel motors.  These are specific for use by the Motor::getMappedRegister function.
 */
enum {
	REG_Model_Number = 0x8000,
	//	REG_Model_Number_H		= 0x8001,
	REG_Firmware_Ver = 0x8002,
	REG_ID = 0x8003,
	REG_Baud = 0x8004,
	REG_Ret_Delay_Time = 0x8005,
	REG_CW_Ang_Lim = 0x8006,
	//	REG_CW_Ang_Lim_H		= 0x8007,
	REG_CCW_Ang_Lim = 0x8008,
	//	REG_CCW_Ang_Lim_H   	= 0x8009,
	REG_Drive_Mode = 0x800A,
	REG_Temp_Lim = 0x800B,
	REG_Voltage_Lim_Low = 0x800C,
	REG_Voltage_Lim_Hi = 0x800D,
	REG_Torque_Max = 0x800E,
	//	REG_Torque_Max_H		= 0x800F,
	REG_Stat_Ret_Level = 0x8010,
	REG_Alarm_LED = 0x8011,
	REG_Alarm_Shutdown = 0x8012,

	REG_Multi_Turn_Offset = 0x8014,
	//	REG_Multi_Turn_Offset_H	= 0x8015,
	REG_Resolution_Divider = 0x8016,

	REG_Torque_EN = 0x8018,
	REG_LED = 0x8019,
	REG_D_Gain = 0x801A,
	REG_I_Gain = 0x801B,
	REG_P_Gain = 0x801C,

	REG_Goal_Pos = 0x801E,
	//	REG_Goal_Pos_H			= 0x801F,
	REG_Moving_Speed = 0x8020,
	//	REG_Moving_Speed_H		= 0x8021,
	REG_Torque_Lim = 0x8022,
	//	REG_Torque_Lim_H		= 0x8023,
	REG_Present_Pos = 0x8024,
	//	REG_Present_Pos_H		= 0x8025,
	REG_Present_Load = 0x8026,
	//	REG_Present_Load_H		= 0x8027,
	REG_Present_Speed = 0x8028,
	//	REG_Present_Speed_H		= 0x8029,
	REG_Present_Voltage = 0x802A,
	REG_Present_Temp = 0x802B,
	REG_Registered = 0x802C,

	REG_Moving = 0x802E,
	REG_Lock = 0x802F,
	REG_Punch = 0x8030,
	//	REG_Punch_H				= 0x8031,
	REG_GoalAcceleration = 0x8049,

	REG_Hardware_Error = 0x804A,
	REG_Control_Mode = 0x804B,

	REG_Current = 0x8044,
	REG_Torque_Mode_EN = 0x8046,
	REG_Goal_Torque = 0x8047,
	REG_Goal_Acceleration = 0x8049,

	REG_CW_MARGIN = 0x881A,
	REG_CCW_MARGIN = 0x881B,
	REG_CW_SLOPE = 0x881C,
	REG_CCW_SLOPE = 0x801D,

	REG_Model_Info = 0x881E,
	REG_Operating_Mode = 0x881F,
	REG_Homing_Offset = 0x8820,
	REG_Moving_Thresh = 0x8821,
	REG_Accelerate_Lim = 0x8822,
	REG_Velocity_Lim = 0x8823,
	REG_Max_Position_Lim = 0x8824,
	REG_Min_Position_Lim = 0x8825,
	REG_Ext_Port_Mode_1 = 0x8826,
	REG_Ext_Port_Mode_2 = 0x8827,
	REG_Ext_Port_Mode_3 = 0x8828,
	REG_Ext_Port_Mode_4 = 0x8829,

	REG_LED_Red = 0x882A,
	REG_LED_Green = 0x882B,
	REG_LED_Blue = 0x882C,
	REG_Vel_I_Gain = 0x882D,
	REG_Vel_P_Gain = 0x882E,
	REG_Ext_Port_Data_1 = 0x882F,
	REG_Ext_Port_Data_2 = 0x8830,
	REG_Ext_Port_Data_3 = 0x8831,
	REG_Ext_Port_Data_4 = 0x8832,
};

// XL series:
#define REG_XL_Model_Number			(0)
#define REG_XL_Firmware_Ver			(2)
#define REG_XL_ID					(3)
#define REG_XL_Baud					(4)
#define REG_XL_Ret_Delay_Time		(5)
#define REG_XL_CW_Ang_Lim			(6)
#define REG_XL_CCW_Ang_Lim			(8)
#define REG_XL_Control_Mode			(11)
#define REG_XL_Temp_Lim				(12)
#define REG_XL_Voltage_Lim_Low		(13)
#define REG_XL_Voltage_Lim_Hi		(14)
#define REG_XL_Torque_Max			(15)
#define REG_XL_Stat_Ret_Level		(17)
#define REG_XL_Alarm_Shutdown		(18)
#define REG_XL_Torque_EN			(24)
#define REG_XL_LED					(25)
#define REG_XL_D_Gain				(27)
#define REG_XL_I_Gain				(28)
#define REG_XL_P_Gain				(29)
#define REG_XL_Goal_Pos				(30)
#define REG_XL_Goal_Vel				(32)
#define REG_XL_Goal_Torque			(35)
#define REG_XL_Present_Pos			(37)
#define REG_XL_Present_Load			(41)
#define REG_XL_Present_Speed		(39)
#define REG_XL_Present_Voltage		(45)
#define REG_XL_Present_Temp			(46)
#define REG_XL_Registered			(47)
#define REG_XL_Moving				(49)
#define REG_XL_Hard_Error			(50)
#define REG_XL_Punch				(51)

// MX series:
#define REG_MX_Model_Number			(0)
#define REG_MX_Firmware_Ver			(2)
#define REG_MX_ID					(3)
#define REG_MX_Baud					(4)
#define REG_MX_Ret_Delay_Time		(5)
#define REG_MX_CW_Ang_Lim			(6)
#define REG_MX_CCW_Ang_Lim			(8)
#define REG_MX106_Drive_Mode		(10)
#define REG_MX_Temp_Lim				(11)
#define REG_MX_Voltage_Lim_Low		(12)
#define REG_MX_Voltage_Lim_Hi		(13)
#define REG_MX_Torque_Max			(14)
#define REG_MX_Stat_Ret_Level		(16)
#define REG_MX_Alarm_LED			(17)
#define REG_MX_Alarm_Shutdown		(18)
#define REG_MX_Multi_Turn_Offset	(20)
#define REG_MX_Resolution_Divider	(22)
#define REG_MX_Torque_EN			(24)
#define REG_MX_LED					(25)
#define REG_MX_D_Gain				(26)
#define REG_MX_I_Gain				(27)
#define REG_MX_P_Gain				(28)
#define REG_MX_Goal_Pos				(30)
#define REG_MX_Goal_Vel				(32)
#define REG_MX_Goal_Torque			(34)
#define REG_MX_Present_Pos			(36)
#define REG_MX_Present_Speed		(38)
#define REG_MX_Present_Load			(40)
#define REG_MX_Present_Voltage		(42)
#define REG_MX_Present_Temp			(43)
#define REG_MX_Registered			(44)
#define REG_MX_Moving				(46)
#define REG_MX_Lock					(47)
#define REG_MX_Punch				(48)
#define REG_MX106_Current			(68)
#define REG_MX106_Torque_Mode_EN	(70)
#define REG_MX106_Goal_Torque		(71)
#define REG_MX_Goal_Acceleration	(73)

// AX, EX, DX, RX series:
#define REG_AX_Model_Number			(0)
#define REG_AX_Firmware_Ver			(2)
#define REG_AX_ID					(3)
#define REG_AX_Baud					(4)
#define REG_AX_Ret_Delay_Time		(5)
#define REG_AX_CW_Ang_Lim			(6)
#define REG_AX_CCW_Ang_Lim			(8)
#define REG_EX106_Drive_Mode		(10)
#define REG_AX_Temp_Lim				(11)
#define REG_AX_Voltage_Lim_Low		(12)
#define REG_AX_Voltage_Lim_Hi		(13)
#define REG_AX_Torque_Max			(14)
#define REG_AX_Stat_Ret_Level		(16)
#define REG_AX_Alarm_LED			(17)
#define REG_AX_Alarm_Shutdown		(18)

#define REG_AX_Torque_EN			(24)
#define REG_AX_LED					(25)
#define REG_AX_CW_Margin			(26)
#define REG_AX_CCW_Margin			(27)
#define REG_AX_CW_Slope				(28)
#define REG_AX_CCW_Slope			(29)
#define REG_AX_Goal_Pos				(30)
#define REG_AX_Goal_Vel				(32)
#define REG_AX_Goal_Torque			(34)
#define REG_AX_Present_Pos			(36)
#define REG_AX_Present_Speed		(38)
#define REG_AX_Present_Load			(40)
#define REG_AX_Present_Voltage		(42)
#define REG_AX_Present_Temp			(43)
#define REG_AX_Registered			(44)
#define REG_AX_Moving				(46)
#define REG_AX_Lock					(47)
#define REG_AX_Punch				(48)

#define REG_EX106_Current			(56)

// Pro:
#define REG_PRO_Model_Number		(0)
#define REG_PRO_Model_Info			(2)
#define REG_PRO_Firmware_Ver		(6)
#define REG_PRO_ID					(7)
#define REG_PRO_Baud				(8)
#define REG_PRO_Ret_Delay_Time		(9)
#define REG_PRO_Operating_Mode		(11)
#define REG_PRO_Homing_Offset		(13)
#define REG_PRO_Moving_Thresh		(17)
#define REG_PRO_Temp_Lim			(21)
#define REG_PRO_Voltage_Lim_Hi		(22)
#define REG_PRO_Voltage_Lim_Low		(24)
#define REG_PRO_Accelerate_Lim		(26)
#define REG_PRO_Torque_Max			(30)
#define REG_PRO_Velocity_Lim		(32)
#define REG_PRO_Max_Position_Lim	(36)
#define REG_PRO_Min_Position_Lim	(40)
#define REG_PRO_Ext_Port_Mode_1		(44)
#define REG_PRO_Ext_Port_Mode_2		(45)
#define REG_PRO_Ext_Port_Mode_3		(46)
#define REG_PRO_Ext_Port_Mode_4		(47)
#define REG_PRO_Shutdown			(48)
#define REG_PRO_Indirect_Add(x)		((x)+48)	// up to 256

#define REG_PRO_Torque_EN			(562)
#define REG_PRO_LED_Red				(563)
#define REG_PRO_LED_Green			(564)
#define REG_PRO_LED_Blue			(565)
#define REG_PRO_Vel_I_Gain			(586)
#define REG_PRO_Vel_P_Gain			(588)
#define REG_PRO_Pos_P_Gain			(594)
#define REG_PRO_Goal_Pos			(596)
#define REG_PRO_Goal_Vel			(600)
#define REG_PRO_Goal_Torque			(604)
#define REG_PRO_Goal_Acceleration	(606)
#define REG_PRO_Moving				(610)
#define REG_PRO_Present_Pos			(611)
#define REG_PRO_Present_Speed		(615)
#define REG_PRO_Present_Current		(621)
#define REG_PRO_Present_Voltage		(623)
#define REG_PRO_Present_Temp		(625)
#define REG_PRO_Ext_Port_Data_1		(626)
#define REG_PRO_Ext_Port_Data_2		(628)
#define REG_PRO_Ext_Port_Data_3		(630)
#define REG_PRO_Ext_Port_Data_4		(632)
#define REG_PRO_Indirect_Data(x)	((x)+633)	// up to 256
#define REG_PRO_Registered			(890)
#define REG_PRO_Stat_Ret_Level		(891)
#define REG_PRO_Hard_Error			(892)

#define XL_LED_OFF	(0x0)
#define XL_RED		(0x1)
#define XL_GREEN	(0x2)
#define XL_BLUE		(0x4)
#define XL_PINK		(XL_RED | XL_BLUE)
#define XL_CYAN		(XL_GREEN | XL_BLUE)
#define XL_YELLOW	(XL_RED | XL_GREEN)
#define XL_WHITE	(XL_RED | XL_GREEN | XL_BLUE)

#define ERROR_V1_VOLTAGE		(0x01)
#define ERROR_V1_ANGLE_LIMIT	(0x02)
#define ERROR_V1_OVERHEATING	(0x04)
#define ERROR_V1_RANGE			(0x08)
#define ERROR_V1_CHECKSUM		(0x10)
#define ERROR_V1_OVERLOAD		(0x20)
#define ERROR_V1_INSTRUCTION	(0x40)

#define ERROR_V2_RESULT_FAIL	(0x01)
#define ERROR_V2_INSTRUCTION	(0x02)
#define ERROR_V2_CRC			(0x03)
#define ERROR_V2_DATA_RANGE		(0x04)
#define ERROR_V2_DATA_LENGTH	(0x05)
#define ERROR_V2_DATA_LIMIT		(0x06)
#define ERROR_V2_ACCESS			(0x07)

class Motor;
struct Instruction;

/// \cond VERBOSE
class Register {
public:
	int value;
	Register() :
			value(-1) {
	}

	Register & operator =(const int& value) {
		this->value = value;
		return *this;
	}

	operator int() {
		return value;
	}

};
/// \endcond

/*! \struct DataRange
 \brief Represents the location and length of data to read when communicating with motors.
 @since 08-28-2015
 */
struct DataRange {

	/*! The start register.
	 */
	unsigned short reg;

	/*! The number of bytes.
	 */
	unsigned short length;

	/*! The constructor.
	 \param reg The start register.
	 \param length The data length.
	 */
	DataRange(unsigned short reg = 0, unsigned short length = 0) :
			reg(reg), length(length) {
	}
};

/*!
 @class MotorCalibration
 \brief Configured the goal position registers based on a desired output shaft angle.

 This treats motor angles differently than in the data sheet.
 First: UNITS ARE IN RADIANS.
 Second: 0 radian setting will CENTER THE OUTPUT SHAFT.
 This makes things way easier during calibration and robot construction.
 @since 11-21-2014
 */
class MotorCalibration {
public:

	/*!
	 \brief Sets the type of calibration to perform based on the motor model.
	 @param model The motor model.
	 */
	void setup(Model model);

	/*!
	 \brief Converts the angle to the register values.
	 @param position The Angle of the motor in radians.
	 @param buffer The output registers to be written to.  Location of buffer[0], buffer[1] ... buffer[n] will be written.
	 */
	void convert(double position, unsigned char* buffer);

	/*!
	 \brief Converts the registers containing position information to the position in radians.
	 @param buffer The registers containing the angle values.  Values of buffer[0], buffer[1] ... buffer[n] will be read.
	 @return position The Angle of the motor in radians.
	 */
	double invert(unsigned char* buffer);

	/*!
	 \brief Sets the offset of the motor position.

	 The output angle from calling convert will be position-offset.
	 @param offset The angle offset in radians.
	 */
	void setOffset(double offset) {
		this->offset = offset;
	}

	/*!
	 \brief Sets the direction of the motor.

	 If set to be reversed, the output of the convert will become (position-offset) * (-1.0).
	 @param reversed True if the output should be reversed, false if normal.
	 */
	void setReverse(bool reversed) {
		this->reverse = reversed;
	}

	MotorCalibration() :
			type(MODEL_XL320), offset(0), reverse(false) {
	}

private:
	Model type;
	double offset;
	bool reverse;

};

/*!
 @class Packet
 \brief Responsible for creating Dynamixel packets.

 This class may be used to generate packets for all supported Robotis motors.
 @since 12-28-2014
 */
class Packet {
public:

	/*!	\brief The Dynamixel protocol versions.
	 */
	enum Type {
		VER_1_0 = 1, /*!< Dyanmixel Protocol version 1.0. */
		VER_2_0 = 2 /*!< Dyanmixel Protocol version 2.0. */
	};

	Packet() :
			type(VER_1_0) {
	}

	/*!
	 \brief Sets the version of packet to create.
	 @param type The packet version type.
	 */
	void setType(Type type) {
		this->type = type;
	}

	/*!
	 \brief Gets the version of packet being created.
	 @return The packet version type.
	 */
	Type getType() {
		return type;
	}

	/*!
	 \brief Creates a PING packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @return The PING packet.
	 */
	const std::vector<unsigned char>& ping(const unsigned char& ID);

	/*!
	 \brief Creates a READ packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @param range The data range to be handled.
	 @return The READ packet.
	 */
	const std::vector<unsigned char>& read(const unsigned char& ID,
			DataRange range);

	/*!
	 \brief Creates a WRITE packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @param range The data range to be handled.
	 @param data The data to be written.
	 @return The WRITE packet.
	 */
	const std::vector<unsigned char>& write(const unsigned char& ID,
			DataRange range, unsigned char* data);

	/*!
	 \brief Creates a REG WRITE packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @param range The data range to be handled.
	 @param data The data to be written.
	 @return The REG WRITE packet.
	 */
	const std::vector<unsigned char>& regWrite(const unsigned char& ID,
			DataRange range, unsigned char* data);

	/*!
	 \brief Creates a ACTION packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @return The ACTION packet.
	 */
	const std::vector<unsigned char>& action(const unsigned char& ID);

	/*!
	 \brief Creates a RESET packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @return The RESET packet.
	 */
	const std::vector<unsigned char>& reset(const unsigned char& ID);

	/*!
	 \brief Creates a REBOOT packet for a specific motor.
	 @param ID The motor ID for the packet.
	 @return The REBOOT packet.
	 */
	const std::vector<unsigned char>& reboot(const unsigned char& ID);

	/*!
	 \brief Creates a SYNC READ packet for a specific motor.
	 @param dynamixels The set of motors to read.
	 @param range The data range to be handled.
	 @return The SYNC READ packet.
	 */
	const std::vector<unsigned char>& syncRead(
			std::map<unsigned char, Motor*>* dynamixels, DataRange range);

	/*!
	 \brief Creates a SYNC WRITE packet for a specific motor.
	 @param dynamixels The set of motors to write to.
	 @param range The data range to be handled.
	 @return The SYNC WRITE packet.
	 */
	const std::vector<unsigned char>& syncWrite(
			std::map<unsigned char, Motor*>* dynamixels, DataRange range);

	/*!
	 \brief Creates a BULK READ packet for a specific motor.
	 @param ranges The set of dynamixels and data ranges.
	 @return The BULK READ packet.
	 */
	const std::vector<unsigned char>& bulkRead(
			std::map<Motor*, DataRange>* ranges);

	/*!
	 \brief Creates a BULK WRITE packet for a specific motor.
	 @param ranges The set of dynamixels and data ranges.
	 @return The BULK WRITE packet.
	 */
	const std::vector<unsigned char>& bulkWrite(
			std::map<Motor*, DataRange>* ranges);

	/*!
	 \brief Check to see if a packet is correctly formatted.

	 This checks that header exists, the length descriptor is correct, and that the checksum/CRC matches.
	 @param packet The packet to check.
	 @return false if incorrect, true if correct.
	 */
	bool checkPacket(const std::vector<unsigned char>& packet);

private:
	Type type;
	std::vector<unsigned char> packet;

	void startPacket(unsigned char ID);
	void finishPacket();
};

/*!
 @class Interface
 \brief An abstract class to interface with a UART device for Dynamixels.

 This abstract class may be used to send commands to Dynamixel motors.
 This handles the packet generation, parsing, and handles the port.
 The concrete class that inherits from this must specify the method of opening and configuring of the specific UART.
 @since 08-31-2015
 */
class Interface {
public:
	/*! \brief The implemented types of concrete UART interfaces.

	 Used for the create() method.
	 */
	enum {
		#ifdef LIBFTDI_FOUND
		FTDI, /*!< The port is an FTDI device. */
#endif
		COM /*!< The port is a COM port. */
	};

	/*! \brief The status of the UART device.
	 */
	typedef enum {
		NOERROR = 0, /*!< No Error occurred. */
		UNSUPPORTED_COMMAND = -1, /*!< Command unsupported. */
		DISCONNECTED = -2, /*!< The port is disconnected. */
		BAD_LENGTH = -3, /*!< The packet is too short. */
		BAD_CHECKSUM = -4, /*!< There is a bad incoming checksum. */
		EMPTY_PACKET = -5 /*!< The packet is empty or NULL. */
	} Status;

	/*! \brief The status of the UART device.
	 */
	typedef enum {
		STATUS_NOERROR = 0, /*!< No Error occurred. */
		STATUS_RESULT_FAIL, /*!< Failed to process instruction packet. */
		STATUS_INSTRUCTION, /*!< Undefined instruction or Action without Reg Write. */
		STATUS_CRC, /*!< CRC or Checksum mismatch. */
		STATUS_DATA_RANGE, /*!< Value is outside of the register's range. */
		STATUS_DATA_LENGTH, /*!< Data length shorter than the required length, i.e. writing 3 bytes into a 4 byte register. */
		STATUS_DATA_LIMIT, /*!< Data length longer tan the required length of the register. */
		STATUS_ACCESS /*!< Writing to read only, reading from write only, or Torque Enable ROM lock. */
	} StatusPacketError;

private:

	StatusPacketError parseReadStatusPacket(Motor* dynamixel, const std::vector<unsigned char>& buffer, const DataRange& range);

protected:

	/*! Designates if verbose mode is set.
	 */
	bool verbose;

	/*! The baudrate.
	 */
	int baud;

	/*! Designates if the port is open and active.
	 */
	bool currentlyOpen;

	/*! Used for generating dynamixel packets.
	 */
	Packet packetHandler;

	/*!
	 \brief Writes data to the UART transmit.
	 @param data The data to be sent.
	 @return On success, the number of bytes transmitted.  Otherwise the values of ERROR or DISCONNECTED.
	 */
	virtual int writeInterface(std::vector<unsigned char> data) = 0;

	/*!
	 \brief Reads data to the UART receive.
	 @param buffer Where the incoming data will be stored.
	 @param size The number of byte to attempt to read.
	 @return On success, the number of bytes received.  Otherwise the vaues of ERROR or DISCONNECTED.
	 */
	virtual int readInterface(std::vector<unsigned char>* buffer,
			size_t size) = 0;

	/*! \brief Handles the response from the motor.
	 This waits for a maximum of 500uS and then will parse the input.
	 @param buffer The output data from the response.
	 @return The interface status.
	 */
	Interface::Status getResponse(std::vector<unsigned char>* buffer);
public:

	/*! \brief The constructor with a specified baudrate.
	 @param baudrate The desired baudrate.
	 */
	Interface(int baudrate);
	virtual ~Interface()=0;

	/*! \brief Opens and configures the UART device.
	 @param field An optional input field containing port information if needed.  i.e. "/dev/ttyUSB0"
	 @return The status after the attempt to open the port.
	 */
	virtual Status open(const char* field = NULL) = 0;

	/*! \brief Closes the UART device.
	 @return The status after the attempt to open the port.
	 */
	virtual int close() = 0;

	/*! \brief Sends a Dynamixel PING command.

	 This performs a factory reset, setting all registers to their default value.
	 @param dynamixel The dynamixel to be sent the instruction.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status ping(Motor* dynamixel);

	/*! \brief Sends a Dynamixel READ command.

	 This reads the registers of a dynamixel motor.
	 Values may be retrieved by calling Motor::get_value().
	 @param dynamixel The dynamixel to be sent the instruction.
	 @param range The data range to be handled.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status read(Motor* dynamixel, DataRange range);

	/*! \brief Sends a Dynamixel WRITE command.

	 This writes to the registers of a dynamixel motor.
	 Values may be set by calling Motor::set_value().
	 @param dynamixel The dynamixel to be sent the instruction.
	 @param range The data range to be handled.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status write(Motor* dynamixel, DataRange range);

	/*!
	 \brief Sends a Dynamixel REG WRITE command.

	 This writes to the register buffer of a dynamixel motor.  To complete the write to the control table, a followup action() must be performed.
	 Values may be set by calling Motor::set_value().
	 @param dynamixel The dynamixel to be sent the instruction.
	 @param range The data range to be handled.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status regWrite(Motor* dynamixel, DataRange range);

	/*!
	 \brief Sends a Dynamixel ACTION command.

	 This writes values of the register buffer to the actual control table registers from the previous regWrite() call.
	 @param dynamixel The dynamixel to be sent the instruction.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status action(Motor* dynamixel);

	/*!
	 \brief Sends a Dynamixel RESET command.
	 @param dynamixel The dynamixel to be sent the instruction.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status factoryReset(Motor* dynamixel);

	/*!
	 \brief Sends a Dynamixel REBOOT command.

	 This reboots the version 2.0 dynamixel packet based motor.
	 @param dynamixel The dynamixel to be sent the instruction.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status reboot(Motor* dynamixel);

	/*!
	 \brief Sends a Dynamixel SYNC READ command.

	 This reads the same registers on multiple dynamixel 2.0 based motors.
	 Values may be read by calling Motor::get_value().
	 @param dynamixels The set of dynamixels to be sent the instruction.
	 @param range The data range to be handled.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status syncRead(std::map<unsigned char, Motor*>* dynamixels,
			DataRange range);

	/*!
	 \brief Sends a Dynamixel SYNC WRITE command.

	 This writes to the registers of a set of dynamixel motors.
	 Values may be set by calling Motor::set_value().
	 @param dynamixels The set of dynamixels to be sent the instruction.
	 @param range The data range to be handled.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status syncWrite(std::map<unsigned char, Motor*>* dynamixels,
			DataRange range);

	/*!
	 \brief Sends a Dynamixel BULK READ command.

	 This reads varying addresses and lengths across multiple Dynamixel motors.  Motors must use protocol version 2.0 or be in the MX series.
	 Values may be read by calling Motor::get_value().
	 @param ranges The set of dynamixels and ranges to be sent the instruction.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status bulkRead(std::map<Motor*, DataRange>* ranges);

	/*!
	 \brief Sends a Dynamixel BULK WRITE command.

	 This writes to varying addresses and lengths across multiple Dynamixel motors.  Motors must use protocol version 2.0.
	 Values may be read by calling Motor::get_value().
	 @param ranges The set of dynamixels and corresponding data ranges.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status bulkWrite(std::map<Motor*, DataRange>* ranges);

	/*!
	 \brief Processes the instruction.
	 @param instruction The instruction to process.
	 @return The status after the attempt to send the instruction.
	 */
	Interface::Status processInstruction(Instruction& instruction);

	/*!
	 \brief Sets the type of packet to be constructed for each command.
	 @param type The Dynamixel protocol version, either Packet::VER_1_0 or Packet::VER_2_0.
	 */
	void setPacketType(Packet::Type type) {
		packetHandler.setType(type);
	}

	/*!
	 \brief Creates a concrete object from a concrete class.
	 @param type The type of UART device, either FTDI (if dependencies installed) or COM.
	 @param baud The baudrate of the UART device.
	 @return The concrete dynamixel interface object.
	 */
	static Interface* create(int type, int baud);

	/*!
	 \brief Checks to see if the port is open.
	 @return True is open, false otherwise.
	 */
	bool isOpen() {
		return currentlyOpen;
	}

	/*!
	 \brief Enables/disables verbosity for debugging.
	 \param verbose True if verbose, false otherwise.
	 */
	void setVerbose(bool verbose);
};

	#ifdef LIBFTDI_FOUND
/*! \class Ftdi
 \brief A concrete class that inherits from the abstract Interface class, implementing FTDI device handling.

 This class handles the opening, closing, writing and reading of FTDI devices.
 @since 08-31-2015
 */
class Ftdi: public Interface {
private:
	ftdi_context *ftdiContext;

	int writeInterface(std::vector<unsigned char> data);
	int readInterface(std::vector<unsigned char>* data, size_t maxToRead);

public:
	/*! \brief The constructor with a specified baudrate.
	 \param baudrate The desired baudrate.
	 */
	Ftdi(int baudrate) :
			Interface(baudrate), ftdiContext(NULL) {
	}

	/*! \brief Opens the FTDI device.
	 \param field Ignored... should be refactored.
	 \return The status after attempting to open the port.
	 */
	Status open(const char* field = NULL);

	/*! \brief Closes the FTDI device.
	 \return The interface status after attempting to close.
	 */
	int close();
};
#endif

/*!
 @class Com
 \brief A concrete class that inherits from the abstract Interface class, implementing COM ports.

 This class handles the opening, closing, writing and reading of COM devices.
 @since 08-31-2015
 */
class Com: public Interface {
private:
	int fid;

	int writeInterface(std::vector<unsigned char> data);
	int readInterface(std::vector<unsigned char>* buffer, size_t size);

public:
	/*! \brief The constructor with a specified baudrate.
	 \param baudrate The desired baudrate.
	 */
	Com(int baudrate) :
			Interface(baudrate), fid(-1) {
	}

	/*! \brief Opens the com port.
	 \param field The device path ("\dev\ttyUSB0", or "\dev\tty.usbserial", etc.).
	 \return The interface status after attempting to close.
	 */
	Status open(const char* field = NULL);

	/*! \brief Closes the com port.
	 \return The interface status after attempting to close.
	 */
	int close();

};

/*!
 @class Observer
 \brief An abstract observer for when instructions have been sent to the dynamixel.
 @since 08-31-2015
 */
class Observer {
public:
	virtual ~Observer() {
	}

	/*!
	 \brief Called when the instruction has been complete.
	 @param dynamixel The motor that was updated.
	 @param instruction The instruction to that was sent.
	 */
	virtual void update(Motor* dynamixel, const Instruction* instruction) = 0;

	/*!
	 \brief Called when the instruction was attempted but has failed.
	 @param dynamixel The motor that with the failed instruction.
	 @param instruction The instruction that was sent but failed.
	 @param status The status corresponding to the communication failure.
	 */
	virtual void failed(Motor* dynamixel, const Instruction* instruction,
			Interface::Status status) {
	}
};

/*!
 @class Motor
 \brief Represents the state of a dynamixel motor.

 This method may be used to get previously read and set to-be-written values of an attached Dynamixel motor.  It is recommended to only set new values and use in conjunction with the Handler.
 @since 02-12-2013
 */
class Motor {

	/* Friends */
	friend class Handler;
	friend class Packet;
	friend class Interface;

private:

	void update_vals();

	MotorCalibration calibration;

	size_t numberOfRegisters;
	unsigned char *registers;
	unsigned char *registersDesired;

	int status;

	std::vector<Observer*> observers;
	std::vector<Observer*> observersToRemove; // Following from MBjsonKeyObserver, I'm sure there is a better way to do this.

	void notifyObservers(const Instruction *instruction);
	void notifyObserversOfFailure(const Instruction *instruction,
			Interface::Status status);

	/*!
	 This constructor creates a dynamixel with an ID.
	 @param ID The ID of the motor for this dynamixel object.
	 @param model The model to initialize the motor at.
	 */
	Motor(unsigned int ID, Model model = MODEL_XL320);

	// constructors
	Motor();
	~Motor();

	void pruneObservers();

	void buildRegisterMap();

	int getMappedRegister(int reg);

	static std::map<Model, std::map<int, Register> > registerMap;

	void allocateRegisters(size_t size);
public:

	/*!
	 \brief Gets the current shaft angle based on the motor calibration (MotorCalibration).
	 @return angle The current position in radians, where 0 is the center of the motor's range.
	 */
	double getCurrentPosition();

	/*!
	 \brief Sets the appropriate registers for the desired goal output based on the motor calibration (MotorCalibration).
	 @param angle The desired goal position in radians, where 0 is the center of the motor's range.
	 */
	void setGoalPosition(double angle);

	/*!
	 \brief Sets the appropriate registers for the angular rate.
	 @param speed The moving angular rate in RPM.
	 */
	void setMovingSpeed(double speed);

	/*!
	 \brief Sets the calibration parameters for the internal MotorCalibration.
	 @param offset The output angle offset in radians.
	 @param reversed Set to true if the motor output should be reverse.
	 */
	void setCalibration(double offset, bool reversed);

	/*!
	 \brief Returns the byte last read from the dynamixel.
	 @param reg The register of the dynamixel.
	 @return The byte from the last read command.
	 */
	unsigned char getByte(int reg);

	/*!
	 \brief Sets the byte to be written to the dynamixel.
	 @param reg The start register of the dynamixel.
	 @param newValue The new byte value.
	 */
	void setByte(int reg, unsigned char newValue);

	/*!
	 \brief Returns the word last read from the dynamixel.
	 @param reg The register of the dynamixel.
	 @return The word value from the last read command.
	 */
	unsigned short getWord(int reg);

	/*!
	 \brief Sets the word value to be written to the dynamixel.
	 @param reg The start register of the dynamixel.
	 @param newValue The new word value.
	 */
	void setWord(int reg, unsigned short newValue);

	/*!
	 \brief Returns the integer last read from the dynamixel.
	 @param reg The register of the dynamixel.
	 @return The int value from the last read command.
	 */
	int getInt(int reg);

	/*!
	 \brief Sets the integer value to be written to the dynamixel.
	 @param reg The start register of the dynamixel.
	 @param newValue The new int value.
	 */
	void setInt(int reg, int newValue);

	/*!
	 \brief Returns the model of the dynamixel in a readable format.
	 @return The type of dynamixel as a string.
	 */
	std::string getModel();

	/*!
	 \brief Adds an observer to be notified upon instruction completion.
	 \param observer The observer to be notified.
	 */
	void addObserver(Observer* observer);

	/*!
	 \brief Removes an observer from notifications upon instruction completion.
	 \param observer The observer to be removed from notification.
	 */
	void removeObserver(Observer* observer);

	/*!
	 Returns the number of registers for this motor, as determined by the READ command containing the model information.
	 @return The number of registers.
	 */
	int getNumberOfRegisters();
};

/*!
 @class Instruction
 \brief Represents the type of instructions that may be handled by the Handler.
 @since 08-28-2015
 */
struct Instruction {
public:

	/*! \brief Types of supported Dynamixel instructions.
	 */
	enum Type {
		PING = 0x01, /*!< For sending a Ping command. */
		READ = 0x02, /*!< For sending a Read command. */
		WRITE = 0x03, /*!< For sending a Write command. */
		REG_WRITE = 0x04, /*!< For sending a Reg Write command */
		ACTION = 0x05, /*!< For sending an Action command */
		RESET = 0x06, /*!< For sending a Factory Reset command. */
		REBOOT = 0x08, /*!< For sending a Reboot command. Only for Dynamixel version 2.0. */
		STATUS = 0x55, /*!< Designates a Status packet. */
		SYNC_READ = 0x82, /*!< For sending a Sync Read command. Only for Dynamixel version 2.0. */
		SYNC_WRITE = 0x83, /*!< For sending a Sync Write command. */
		BULK_READ = 0x92, /*!< For sending a Bulk Read command. Only for Dynamixel version 2.0 and MX series. */
		BULK_WRITE = 0x93 /*!< For sending a Bulk Write command. Only for Dynamixel version 2.0. */
	};

	/*! The instruction type.
	 */
	Type type;

	/*! The data range of the instruction, if needed.
	 */
	DataRange range;

	/*! The data ranges for each motor for the bulk commands.
	 */
	std::map<Motor*, DataRange> bulkInfo;

	/*! The specific motor for the instruction, if needed.
	 */
	Motor* dynamixel;

	/*! The list of dynamixels for the sync commands.
	 */
	std::map<unsigned char, Motor*>* dynamixelList;

	/* Constructor */
	/*!
	 InstructionLK commands
	 Instruction construction for non BU.
	 @param type The instruction type (PING, READ, WRITE, REG_WRITE, ACTION, RESET, REBOOT, SYNC_READ, or SYNC_WRITE)
	 @param range The data range to work with. (unused for PING, ACTION, RESET, and REBOOT)
	 @param dynamixel The specific dynamixel to write or read from. (unused for SYNC_WRITE and SYNC_READ)
	 @param dynamixelList The set of dynamixels to communicate with. (only used for SYNC_WRITE and SYNC_READ)
	 */
	Instruction(const Type& type, DataRange range = DataRange(0, 0),
			Motor* dynamixel = NULL,
			std::map<unsigned char, Motor*>* dynamixelList = NULL) :
			type(type), range(range), dynamixel(dynamixel), dynamixelList(
					dynamixelList) {
	}

	/*!
	 Instruction
	 Instruction construction for BULK commands.
	 @param type The instruction type (BULK_READ or BULK_WRITE)
	 @param ranges The transfer information for the instruction.
	 */
	Instruction(const Type& type, const std::map<Motor*, DataRange>& ranges) :
			type(type), bulkInfo(ranges), dynamixel(NULL), dynamixelList(NULL) {
	}

	//private:
};

/*!
 @class Handler
 \brief Handles communication queueing of the motors.

 This should be the primary method to communicate the dynamixel motors.  Values of each motor should be set in the Motor class from retrieving the Motor* vector from getDynamixels(), then the specific instruction to be handled may be used by calling pushInstruction().  This class privately contains an Interface to handle the specific UART hardware interface.
 @since 08-28-2015
 */
class Handler: private Mogi::Thread {
private:
	std::map<unsigned char, Motor*> dynamixelList;

	Interface* interface;

	std::vector<Instruction*> instructionQueue;

	//	void InternalThreadEntry();
	void doAction();
	void entryAction();
	void exitAction();
	pthread_mutex_t vectorMutex;

	bool isBusy;

protected:
	/*! The interface baudrate.
	 */
	int baud;

public:

	Handler();
	~Handler();

	/*!
	 \brief Adds an instruction to the FIFO queue.
	 @param instruction The dynamixel instruction to be generated and sent.
	 */
	void pushInstruction(Instruction* instruction);

	/*!
	 \brief Opens the Interface to be used for UART communication.
	 @param interfaceType The type of port to open: Interface::COM or Interface::FTDI.
	 @param baudrate The baudrate of the port.
	 @param field The file path of the COM port to open.  For an FTDI device, this field is unused (NULL).
	 @return The Interface status.
	 */
	Interface::Status openInterface(int interfaceType, int baudrate,
			const char* field = NULL);

	/*!
	 \brief Sets the Interface to be used for UART communication.
	 @param interface The interface to use.
	 */
	void setInterface(Interface* interface);

	/*!
	 \brief Sets the Dyanmixel protocol version to use for motor communication.
	 @param type The type of packet to generate, either version 1.0 or 2.0
	 */
	void setPacketType(Packet::Type type) {
		interface->setPacketType(type);
	}

	/*!
	 \brief Safely adds a dynamixel motor the std::vector of dynamixels.

	 After calling this method a PING is sent to the dynamixel. If the ping command is successfully received by the Dynamixel, the motor is added to the managed list.  If successfully added, all registers are also uploaded and populated into the Motor.
	 @param ID The motor ID to be added.
	 */
	void addDynamixel(unsigned int ID);

	/*!
	 \brief Forcefully adds a dynamixel motor the std::vector of dynamixels.  No ping or upload occurs.
	 @param ID The motor ID to be added.
	 @param model The motor model.
	 */
	void forceAddDynamixel(unsigned int ID, Model model = MODEL_XL320);

	/*!
	 \brief Returns the set of dynamixel motors that have been added by either addDynamixel() or forceAddDynamixel();
	 @return The set of dynamixels currently being handled.
	 */
	std::map<unsigned char, Motor*> getDynamixels();

	/*!
	 \brief Returns the size of the queue of instructions to be processed.
	 @return the number of instructions left to be sent.
	 */
	int queueLength();

	/*!
	 \brief Useful for checking if the thread is busy processing packets.
	 @return True if packets are still being processed, false if idling.
	 */
	bool busy();

};

/*!
 \brief Computes the checksum for a given dynamixel protocol.
 
 This function computes the checksum of arbitrary length for a dynamixel protocol.  The checksum can then be used to finalize the build of a buffer to be sent, or may be used to check an incoming string.
 @param buffer
 The buffer containing a dynamixel protocol
 @return Returns the computed checksum for the dynamixel protocol
 */
unsigned char computeChecksum(const unsigned char *buffer);

/*!
 \brief Prints the error of the given motor
 
 Converts the Robotis error into a readable string.
 @param error The error code of the motor
 @param ID The ID of the motor
 @return The readable string of the returned error.
 */
std::string robotisError(int error, int ID);

/*!
 \brief Converts an unsigned short encoded into a byte array back into an unsigned short.
 @param data The byte array of 2 bytes to convert
 @return The unsigned short value.
 */
unsigned short btous(const unsigned char* data);

/*!
 \brief Converts the motor model into an easy to read string.
 @param model The model of the motor.
 @return The string value of the motor model.
 */
std::string modelToStr(Model model);

} // End Dynamixel namespace

/*! @} End of Mogi group*/

} // End Mogi namespace

#endif
