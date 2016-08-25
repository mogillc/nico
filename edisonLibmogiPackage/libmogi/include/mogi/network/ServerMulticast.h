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

#ifndef INCLUDE_MOGI_REMOTEAPP_SERVERMULTICAST_H_
#define INCLUDE_MOGI_REMOTEAPP_SERVERMULTICAST_H_

#include <string>
#include "mogi/thread.h"
#include "mogi/network/UDPSocket.h"

namespace Mogi {
	namespace Network {

		/*! @class ServerMulticast
		 * \brief A thread that periodically transmits packets containing a TCP server's address to a multicast group.
		 */
		class ServerMulticast : public Thread {
		public:

			/*! \brief Constructs a ServerMulticast thread that transmits a TCP server's hostname, port, and library version to
			 * a designated multicast address.
			 * @param serverPort The TCP server's port that is transmitted (along with hostname and version) to multicast group listeners.
			 * @param multicastPort The multicast group's port.
			 * @param multicastGroupAddress The multicast group's IP address.
			 */
			ServerMulticast(unsigned short int serverPort, unsigned short int multicastPort, const std::string& multicastGroupAddress);

			/*!
			 * \brief This little fella is called whenever ServerMulticast is about to be demolished.
			 */
			~ServerMulticast();

			/*! \brief Allows users to set the rate at which multicast messages are sent.
			 * @param rateInSeconds The desired transmission rate.
			 *
			 * This rate is not guaranteed to be accurate at all ....
			 */
			void setMulticastRate(int rateInSeconds);

			/*! \brief Returns the current transmission rate for multicast messages.
			 * @return The multicast transmission rate in seconds.
			 */
			unsigned int getMulticastRate() const;

		private:
			unsigned short int serverPort; // The identity of the TCP Server port "broadcasted" by this class
			unsigned short int multicastPort; // The multicast port on which clients are listening
			std::string multicastGroupAddress; // The multicast group address on which clients are listening
			SocketAddress multicastDestination; //
			char message[200];
			char hostname[64];
			UDPSocket* udpSocket;
			unsigned int multicastRateSec; // How fast to send multicast packets

			static const unsigned int DEFAULT_MULTICAST_RATE_SEC = 2; // Default multicast rate

			/*!
			 * \brief Overrides Mogi::Thread::entryAction() to setup UDP socket and the multicast message.
			 */
			void entryAction();

			/*!
			 * \brief Overrides Mogi::Thread::doAction() to send multicast packets at a certain rate.
			 */
			void doAction();

			/*!
			 * \brief Overrides Mogi::Thread::exitAction() for cleanup.
			 */
			void exitAction();

			// Prevent value semantics
			ServerMulticast(const ServerMulticast&);
			ServerMulticast& operator=(const ServerMulticast&);
		};
	}
}



#endif /* INCLUDE_MOGI_REMOTEAPP_SERVERMULTICAST_H_ */
