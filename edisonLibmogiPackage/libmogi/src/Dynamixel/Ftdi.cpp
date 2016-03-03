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
#include <iomanip>	// std::setw(), std::setfill()

#ifdef IDENT_C
static const char* const Dynamixel_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	#ifdef LIBFTDI_FOUND	// This will throw a warning if not installed.

using namespace Mogi;
using namespace Dynamixel;

/*!
 @function ftdi_open
 @abstract Opens an ftdi context
 @discussion This function opens an ftdi device at a specific baudrate, with
 parameters configured for the Dynamixel protocol
 @param ftdic
 The ftdi context to write/read from
 @param baudrate
 The desired baudrate to be set on the open device
 @return Returns 0 on success, -1 on fail.
 */
int ftdi_open(ftdi_context* ftdic, int baudrate);

int Ftdi::writeInterface(std::vector<unsigned char> data) {
	if (!currentlyOpen) {
		return DISCONNECTED;
	}

	if (verbose) {
		std::cout << "Write Interface Buffer: " << std::endl;
		for (int index=0; index<data.size(); index++) {
			std::cout << std::hex << std::setfill('0') << std::setw(2) << data[index] << ' ';
		}
		std::cout << std::endl;
	}

	ftdi_usb_purge_buffers(ftdiContext);
	//	int ret = ftdi_write_data(ftdiContext, data.data(), data.size());
	//	ftdi_usb_purge_tx_buffer(ftdiContext);
	// ftdi_transfer_data_done( ftdiContext);

	ftdi_transfer_control *tc = ftdi_write_data_submit(ftdiContext, data.data(),
			data.size());
	if (tc == NULL) {
		return DISCONNECTED;
	}
	return ftdi_transfer_data_done(tc);  // Drain

	// return ret;

	return NOERROR;
}

int Ftdi::readInterface(std::vector<unsigned char>* buffer, size_t maxSize) {
	int numSent = BAD_LENGTH;
	if (buffer == NULL) {
		return BAD_LENGTH;
	}

	unsigned char tempBuffer[maxSize];
	if (currentlyOpen) {
		numSent = ftdi_read_data(ftdiContext, tempBuffer, maxSize);
		for (int i = 0; i < numSent; i++) {
			buffer->push_back(tempBuffer[i]);
		}
		if (verbose) {
			std::cout << "Read Interface Buffer: [length=" << numSent << "]" << std::endl;
			for (int i = 0; i < numSent; i++) {
				std::cout << std::hex << std::setfill('0') << std::setw(2) << tempBuffer[i] << ' ';
			}
			std::cout << std::endl;
		}
	}

	return numSent;
}

Interface::Status Ftdi::open(const char* field) {
	if ((ftdiContext = ftdi_new()) == 0) {
		//			std::cerr << "Error: Handler::openFTDI(): ftdi_new() failed" <<
		//std::endl;
		currentlyOpen = false;
		return DISCONNECTED;
	} else {
		int test = ftdi_open(ftdiContext, baud); // Now use the flexible baudrate, no more #define values

		if (test != -1) {
			currentlyOpen = true;
			usleep(100000);
			ftdi_usb_purge_buffers(ftdiContext);
		} else {
			currentlyOpen = false;
			// fprintf(stderr, "  - ERROR! Unable to open FTDI device.\n");
			return DISCONNECTED;
		}
	}

	return NOERROR;
}

int Ftdi::close() {
	if (currentlyOpen) {
		int ret;
		if ((ret = ftdi_usb_close(ftdiContext)) < 0) {
			std::cerr << "  - Unable to close ftdi device: " << ret << "("
					<< ftdi_get_error_string(ftdiContext) << ")" << std::endl;
		}

		ftdi_deinit(ftdiContext);
		currentlyOpen = false;
	}
	return NOERROR;
}

int ftdi_open(ftdi_context* ftdic, int baudrate)  // TODO: make this more
												  // general, there is a LOT of
												  // hard coded stuff in here.
		{
	int ret;
	//		bool deviceIs2232D = false;

	if (ftdi_init(ftdic) < 0) {
		std::cerr << "Ftdi: ftdi_init() failed" << std::endl;
		return -1;
	} else {
		// printf("I‎nitialized ftdi_context...\n");
	}

	if ((ret = ftdi_usb_open_desc(ftdic, 0x0403, 0x6001, "Dynamixel", NULL))
			>= 0) {
		// printf("Opened the USB device!\nNow configuring device as 8N1 with %d
		// Baud\n",baudrate);
	} else {
		if (ftdi_usb_open(ftdic, 0x0403, 0x6015) >= 0) {  // Nico support

		} else {
			// printf("Setting channel B on 2232D returned: %d\n",
			// ftdi_set_interface(ftdic, INTERFACE_B));

			if (ftdi_usb_open(ftdic, 0x0403, 0x6010) >= 0) {
				// printf("Opened the USB device!\nNow configuring device as 8N1 with %d
				// Baud\n",baudrate);
				//				deviceIs2232D = true;
			} else {
#ifdef __APPLE__
				ftdi_usb_close(ftdic);
#endif
				ftdi_init(ftdic);
				if (ftdi_usb_open(ftdic, 0x0403, 0x6001) >= 0) {
					// printf("WARNING: Opened unknown USB Device!\n");
					//					deviceIs2232D = false;

				} else {
					// fprintf(stderr, "Error:  - Unable to open ftdi device, returned
					// error: %d (%s)\n", ret, ftdi_get_error_string(ftdic));
					// printf("  - Unable to open ftdi device: %d (%s)\n", ret,
					// ftdi_get_error_string(ftdic));
					return -1;
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////

	// Initialize everything:

	ftdi_usb_reset(ftdic);
	// if (deviceIs2232D) {
	//	printf("Setting channel B on 2232D returned: %d\n",
	//ftdi_set_interface(ftdic, INTERFACE_B));
	//}

	ftdi_set_line_property(ftdic, BITS_8, STOP_BIT_1, NONE);
	ftdi_setflowctrl(ftdic, SIO_DISABLE_FLOW_CTRL);
	ftdi_set_baudrate(ftdic, baudrate);
	ftdi_usb_purge_rx_buffer(ftdic);
	ftdi_usb_purge_tx_buffer(ftdic);
	return 0;
}

	#endif	// FTDI_FOUND

#ifdef _cplusplus
}
#endif


