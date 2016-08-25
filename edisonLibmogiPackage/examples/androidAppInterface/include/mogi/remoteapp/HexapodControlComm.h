/*
 * HexapodControlComm.h
 *
 *  Created on: Apr 30, 2016
 *      Author: adrian
 */

#ifndef INCLUDE_MOGI_REMOTEAPP_HEXAPODCONTROLCOMM_H_
#define INCLUDE_MOGI_REMOTEAPP_HEXAPODCONTROLCOMM_H_


#include <mogi/remoteapp/HexapodPayload.h>
#include <string>
#include "mogi/thread.h"
#include "mogi/math/systems.h"
#include "mogi/network/TCPSocket.h"

namespace Mogi {
	namespace RemoteApp {

		/*! @class HexapodControlComm
		 * \brief Client thread that sends control commands to a hexapod server at a defined rate.
		 */
		class HexapodControlComm: public Thread {
		public:
			typedef void (*HexapodStateCallback)(const HexapodPayload&);


		    enum ControlStickID {
		    	ID_LEFT_CONTROL_STICK,
				ID_RIGHT_CONTROL_STICK
		    };

		    enum SliderID {
		    	ID_SLIDER_1,
				ID_SLIDER_2
		    };

			HexapodControlComm(const std::string hostname, unsigned int short port);
			~HexapodControlComm();

			void registerCallback(HexapodStateCallback callback);


		    /*! \brief Sets the x and y values for a specific control stick. These values will be transmitted to
		     * the hexapod server.
		     *
		     * @param controlStickID the ID of the control stick (e.g., ID_LEFT_CONTROL_STICK or ID_RIGHT_CONTROL_STICK)
		     * @param xVal the x value of the control stick
		     * @param yVal the y value of the control stick
		     */
			void setControlStickValues(ControlStickID controlStickID, double xVal, double yVal);

		    /*! \brief Sets the slider value for a specific slider. This value will be transmitted to
		     * the hexapod server.
		     *
		     * @param sliderID the ID of the slider (e.g., ID_SLIDER_1)
		     * @param value the value of the slider
		     */
			void setSliderValues(SliderID sliderID, double value);

		protected:
			void entryAction();
			void doAction();
			void exitAction();

		private:

			long commPeriodMS; // How often to send control packet
			Math::Time loopTime;
			static const long DEFAULT_COMM_PERIOD_MS = 20;


			// Hexapod server information
			std::string serverHostname;
			unsigned short int serverPort;

			// TCP socket object
			Network::TCPSocket* tcpSocket;

		    // Control values transmitted to hexapod
		    double xLeft;
		    double yLeft;
		    double xRight;
		    double yRight;
		    double slider1Value;
		    double slider2Value;
		    long protocolCount;

		    // Packet structures
		    HexapodPacket txPacket;
		    HexapodPacket rxPacket;

		    // JSON key constants for hexapod control
		    static const std::string JSON_KEY_X_LEFT;
		    static const std::string JSON_KEY_Y_LEFT;
		    static const std::string JSON_KEY_X_RIGHT;
		    static const std::string JSON_KEY_Y_RIGHT;
		    static const std::string JSON_KEY_SLIDER_1_VALUE;
		    static const std::string JSON_KEY_SLIDER_2_VALUE;
		    static const std::string JSON_KEY_PROTOCOL_COUNT;

		    // RX buffer
		    static const unsigned int RX_BUFFER_LEN = 1500;
		    unsigned char buffer[RX_BUFFER_LEN];

		    // Callback
		    HexapodStateCallback callback;

			// Prevent value semantics
			HexapodControlComm(const HexapodControlComm&);
			HexapodControlComm& operator= (const HexapodControlComm&);

			std::string createControlObjectString();

			bool sendControlPacket();
			bool receiveStatePacket();
			bool processRXPacket(HexapodPacket& packet);

			void snooze(uint msTime);
		};

	}
}


#endif /* INCLUDE_MOGI_REMOTEAPP_HEXAPODCONTROLCOMM_H_ */
