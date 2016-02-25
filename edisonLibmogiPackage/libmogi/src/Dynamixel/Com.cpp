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

#if defined(__APPLE__)
#ifndef BUILD_FOR_IOS
#include <IOKit/serial/ioss.h>
#endif
#else
#include <linux/serial.h>
//#include <asm/termios.h> // TODO look into this for non-standar baudrates with BOTHER
#endif

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>

#ifdef IDENT_C
static const char* const Dynamixel_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Dynamixel;

	int Com::writeInterface(std::vector<unsigned char> data) {
		if (!currentlyOpen) {
			return DISCONNECTED;
		}

		if (verbose) {
			std::cerr << "Outgoing Buffer:";
			for(int i = 0; i < data.size(); i++)
			{
				std::cerr << " " << (int)data.at(i);
			}
			std::cerr << std::endl;
		}

		// std::cerr << " In Com::writeInterface()" << std::endl;
		// tcflush(fid, TCSAFLUSH);
		// tcflush(fid, TCSANOW);

		int result = ::write(fid, data.data(), data.size());
		tcdrain(fid);
		// std::cerr << " Wrote " << result << " bytes" << std::endl;
		return result;
	}

	int Com::readInterface(std::vector<unsigned char>* buffer, size_t maxSize) {
		// std::cerr << " In Com::readInterface()" << std::endl;
		int numSent = BAD_LENGTH;
		if (buffer == NULL) {
			return EMPTY_PACKET;
		}
		usleep(10000);
		unsigned char tempBuffer[maxSize];
		if (currentlyOpen) {
			numSent = ::read(fid, tempBuffer, maxSize);
			for (int i = 0; i < numSent; i++) {
				buffer->push_back(tempBuffer[i]);
			}
		}

		return numSent;
	}

//	int openPort(const char* field, int params) {
//		return open(field, params, 0x777);
//	}

	// long	glStartTime	= 0;
	// float	gfRcvWaitTime	= 0.0f;
	// float	gfByteTransTime	= 0.0f;

	// char	gDeviceName[20];

	//	void dxl_hal_close()
	//	{
	//		if(fid != -1)
	//		close(fid);
	//		fid = -1;
	//	}

	static int rate_to_constant(int baudrate) {
#define B(x) case x: return B##x
		switch(baudrate) {
				B(50);     B(75);     B(110);    B(134);    B(150);
				B(200);    B(300);    B(600);    B(1200);   B(1800);
				B(2400);   B(4800);   B(9600);   B(19200);  B(38400);
				B(57600);  B(115200); B(230400);
#ifdef B460800
				B(460800); B(500000);
				B(576000); B(921600); B(1000000);B(1152000);B(1500000);
#endif
			default: return 0;
		}
#undef B
	}

	Interface::Status Com::open(const char* field) {
		struct termios newtio;
		//		std::cerr << "Opening: " << field << std::endl;
		if ((fid = ::open(field, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
			// std::cerr << "device open error: " << field << std::endl;
			close();
			return DISCONNECTED;
		}

		int speed = rate_to_constant(this->baud);

#ifdef __linux__

		if (speed == 0) {	// Non-working on Intel Edison, suggest looking into BOTHER in the future.  Currently B1000000 is defined, so no rush.

			struct serial_struct serinfo;
			/* Custom divisor */
			//serinfo.reserved_char[0] = 0;

			if(ioctl(fid, TIOCGSERIAL, &serinfo) < 0) {
				std::cerr << "Cannot get serial info" << std::endl;
				return DISCONNECTED;
			}

			serinfo.flags &= ~ASYNC_SPD_MASK;
			serinfo.flags |= ASYNC_SPD_CUST;
			serinfo.custom_divisor = (serinfo.baud_base + (this->baud / 2)) / this->baud;
			if (serinfo.custom_divisor < 1)
				serinfo.custom_divisor = 1;
			double closestSpeed = (double)serinfo.baud_base / (double)serinfo.custom_divisor;

			if (closestSpeed < this->baud * 98 / 100 || closestSpeed > this->baud * 102 / 100) {
				std::cerr << "Cannot set serial port speed to " << this->baud << ", Closest possible is " <<  closestSpeed << std::endl;
			}

			if(ioctl(fid, TIOCSSERIAL, &serinfo) < 0) {
				std::cerr << "Unable to set baudrate for " << field << ", Error: " << strerror(errno) << std::endl;
				//return DISCONNECTED;
			}
			if (ioctl(fid, TIOCGSERIAL, &serinfo) < 0) {
				std::cerr << "Unable to get serial info second time :(" << std::endl;
			}
//#ifdef BOTHER
//			struct termios2 tio;
//
//			ioctl(fid, TCGETS2, &tio);
//			tio.c_cflag &= ~CBAUD;
//			tio.c_cflag |= BOTHER;
//			tio.c_ispeed = 12345;
//			tio.c_ospeed = 12345;
//			ioctl(fid, TCSETS2, &tio);
//#endif
		}



#endif

		//		std::cerr << "Setting termios" << std::endl;

		tcgetattr(fid, &newtio);
		cfmakeraw(&newtio);
		//		memset(&newtio, 0, sizeof(newtio));

		newtio.c_cflag = CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = 0;
		newtio.c_lflag = 0;
		newtio.c_cc[VTIME] = 0;  // time-out 값 (TIME * 0.1초) 0 : disable
		newtio.c_cc[VMIN] = 0;

		cfsetispeed(&newtio, speed ?: B38400);
		cfsetospeed(&newtio, speed ?: B38400);
		// apply our settings
		int ret = tcsetattr(fid, TCSANOW, &newtio);
		//	ret = ioctl(fid, TIOCSETA, &newtio); // alternative ?
		if (ret)
			std::cerr << "Error: tcsetattr(TCSAFLUSH)" << std::endl;

#ifdef __APPLE__
#ifndef BUILD_FOR_IOS
		// this allows higher (non-standard) baud rates, apparently not supported (on darwin) via termios
		//std::cerr << "Attempting custom Baudrate stuff:" << std::endl;
		const int TGTBAUD = this->baud;
		ret = ioctl(fid, IOSSIOSPEED, &TGTBAUD); // as opposed to setting it in theTermios ?
		if (ret) {
			std::cerr << "Could not set baudrate!" << std::endl;
			//dispError("ioctl(IOSSIOSPEED)",ret,errno);
			return DISCONNECTED;
		}
#endif
#endif

		currentlyOpen = true;
		return NOERROR;
	}

	int closePort(int fid) {
		if (fid != -1)
			if (close(fid) == -1) {
				return Com::DISCONNECTED;
			}
		fid = -1;
		return Com::NOERROR;
	}
	
	int Com::close() {
		if (closePort(fid)) {
			return DISCONNECTED;
		}
		return NOERROR;
	}
	
#ifdef _cplusplus
}
#endif
