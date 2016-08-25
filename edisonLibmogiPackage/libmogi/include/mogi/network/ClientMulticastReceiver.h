/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                          Author: Adrian Lizarraga                          *
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

#ifndef INCLUDE_MOGI_REMOTEAPP_CLIENTMULTICASTRECEIVER_H_
#define INCLUDE_MOGI_REMOTEAPP_CLIENTMULTICASTRECEIVER_H_

#include <string>
#include "mogi/thread.h"
#include "mogi/network/UDPSocket.h"
#include "mogi/app/json.h"

namespace Mogi {
	namespace Network {

		/*! @class ClientMulticastReceiver
		 * \brief Thread that listens for multicast packets containing server information.
		 * User is notified via a callback function.
		 */
		class ClientMulticastReceiver: public Thread {
		public:
			/*! @struct ServerInfo
			 * \brief Contains TCP server's information
			 */
			struct ServerInfo {
				/*!
				 * \brief TCP server's port
				 */
				unsigned short int port;
				/*!
				 * \brief TCP server's hostname
				 */
				std::string hostname;
				/*!
				 * \brief TCP server's library version
				 */
				std::string version;

				std::string ipAddress;
			};

			/*!
			 * \brief Function type used for callback
			 */
			typedef void (*ServerInfoCallback)(const ServerInfo&);

			/*! \brief Constructs a ClientMulticastReceiver object that listens for multicast packets
			 * at the designated port and multicast group address.
			 * @param listenPort The local port
			 * @param multicastGroupAddress The multicast group
			 */
			ClientMulticastReceiver(unsigned short int listenPort, const std::string& multicastGroupAddress);

			/*!
			 * \brief Destructor
			 */
			~ClientMulticastReceiver();

			/*! \brief Used by user to register a callback that's
			 * called by this thread upon reception of TCP server's connection information.
			 * @param callback The callback function called to provide user with server information.
			 */
			void registerCallback(ServerInfoCallback callback);

			/*! \brief Returns the last received server information.
			 * @return The last received server information.
			 */
			ServerInfo getServerInfo();

		protected:
			/*!
			 * \brief Overrides Mogi::Thread::entryAction() to initialize socket and data.
			 */
			void entryAction();

			/*!
			 * \brief Overrides Mogi::Thread::doAction(). Function checks if multicast packets have been received.
			 * If valid server information has been received, the user's callback function is called with the server information.
			 */
			void doAction();

			/*!
			 * \brief Overrides Mogi::Thread::exitAction(). Functions cleans up.
			 */
			void exitAction();

		private:
			/*!
			 * \brief Default thread sleep time
			 */
			static const unsigned int SLEEP_TIME_MS = 1000;

			/*!
			 * \brief Default buffer size (in bytes) for reception buffer.
			 */
			static const unsigned int RX_BUFFER_SIZE = 200;

			/*!
			 * \brief The multicast group address on which server information will be received.
			 */
			std::string multicastGroupAddress;

			/*!
			 * \brief The local listening port for multicast packets.
			 */
			unsigned short int listenPort;

			/*!
			 * \brief The last know server information
			 */
			ServerInfo serverInfo;

			/*!
			 * \brief Default JSON key in the received multicast packet for the server's port.
			 */
			static const std::string SERVER_PORT_KEY;

			/*!
			 * \brief Default JSON key in the received multicast packet for the server's hostname.
			 */
			static const std::string SERVER_HOSTNAME_KEY;

			/*!
			 * \brief Default JSON key in the received multicast packet for the server's library version.
			 */
			static const std::string SERVER_VERSION_KEY;

			/*!
			 * \brief The user's callback function
			 */
			ServerInfoCallback callback;

			/*!
			 * \brief The UDP socket for receiving multicast packets.
			 */
			UDPSocket* udpSocket;

			/*!
			 * \brief The buffer for storing received multicast data.
			 */
			char rxBuffer[RX_BUFFER_SIZE];

			/*!
			 * \brief The JSON subject that parses JSON payloads (from multicast packets) and
			 * notifies JsonValueObservers.
			 */
			App::JsonSubject jsonSubject;

			/*!
			 * \brief The vector of JsonValueObservers. This is used for memory management.
			 */
			std::vector<App::JsonValueObserver*> valueObservers;

			/*!
			 * \brief Causes the current thread to sleep for the specified amount of time.
			 * @param msTime The time duration (in milliseconds) to sleep.
			 */
			void snooze(unsigned int msTime);

			/*!
			 * \brief Cleans up this object's heap-allocated memory.
			 */
			void releaseMemory();

			/*!
			 * \brief Copy constructor is disabled.
			 */
			ClientMulticastReceiver(const ClientMulticastReceiver& other);

			/*!
			 * \brief Assignment operator is disabled.
			 */
			ClientMulticastReceiver& operator=(const ClientMulticastReceiver& other);
		};

	}
}


#endif /* INCLUDE_MOGI_REMOTEAPP_CLIENTMULTICASTRECEIVER_H_ */
