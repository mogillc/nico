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

#ifndef INCLUDE_MOGI_NETWORK_TCPSERVERSOCKET_H_
#define INCLUDE_MOGI_NETWORK_TCPSERVERSOCKET_H_

#include <string>
#include "mogi/network/TCPSocket.h"

namespace Mogi {
	namespace Network {

		/*! @class TCPServerSocket
		 * \brief A class representing a TCP server socket.
		 */
		class TCPServerSocket: public SocketDescriptor {
		public:
			/*! \brief Constructs a TCP server socket that is bound to the specified local port and the host's IP addresses.
			 * @param localPort The local port to which the TCP server socket is bound.
			 * @param maxConnections The maximum number of pending connections/clients to queue.
			 * @param blocking A boolean value indicating blocking (true) or nonblocking (false) behavior for socket operations such as accept().
			 * @param domain The domain of the TCP socket (e.g., INET for IPv4 or INET6 for IPv6).
			 */
			TCPServerSocket(unsigned short int localPort, unsigned int maxConnections, bool blocking, Domain domain = INET);

			/*! \brief Constructs a TCP server socket that is bound to the specified local port and IP address.
			 * @param localAddress The local interface address to which the TCP server is bound.
			 * @param localPort The local port to which the TCP server socket is bound.
			 * @param maxConnections The maximum number of pending connections/clients to queue.
			 * @param blocking A boolean value indicating blocking (true) or nonblocking (false) behavior for socket operations such as accept().
			 * @param domain The domain of the TCP socket (e.g., INET for IPv4 or INET6 for IPv6).
			 */
			TCPServerSocket(const std::string& localAddress, unsigned short int localPort, unsigned int maxConnections, bool blocking, Domain domain = INET);

			/*! \brief Allows caller to set the maximum number of pending connections/clients to queue.
			 * @param maxConnections The maximum number of pending connections/clients to queue.
			 */
			void listen(unsigned int maxConnections) const;

			/*! \brief Waits (if blocking) for incoming TCP client connections and returns a socket for the newly accepted TCP client.
			 * It is the caller's responsibility TO DELETE the TCPSocket. Use of smart-pointers is recommended.
			 * @return A pointer to the newly accepted TCP client's socket.
			 */
			TCPSocket* accept() const;
		};

	}
}


#endif /* INCLUDE_MOGI_NETWORK_TCPSERVERSOCKET_H_ */
