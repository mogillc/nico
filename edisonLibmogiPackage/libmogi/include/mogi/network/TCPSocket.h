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

#ifndef TCP_CLIENT_SOCKET_H
#define TCP_CLIENT_SOCKET_H

#include "mogi/network/ConnectedSocket.h"

namespace Mogi {
	namespace Network {	

		/*! @class TCPSocket
		 * \brief A class representing a TCP socket supporting data transmission and reception.
		 * The send() and receive() methods necessary for communication are inherited from ConnectedSocket.
		 */
		class TCPSocket: public ConnectedSocket {
		public:

			/*! \brief Constructs an unconnected TCPSocket.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 */
			TCPSocket(Domain domain = INET);

			/*! \brief Constructs a TCPSocket connected to the specified remote host.
			 * @param remoteHost The hostname or IP address of the remote host.
			 * @param remotePort The port of the remote host.
			 * @param blocking A boolean value the enables blocking (true) or nonblocking (false) behavior for send(), receive(), etc.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 */
			TCPSocket(const std::string& remoteHost, unsigned short int remotePort, bool blocking, Domain domain = INET);
		private:
			friend class TCPServerSocket;

			/*! \brief Constructs a TCPSocket object from an existing socket descriptor. This is a private constructor used primarily by TCPServerSocket.
			 * @param sockfd The initialized raw socket descriptor.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 * @param options The options denoting the current state of the socket (e.g., connected, closed, bound).
			 */
			TCPSocket(int sockfd, Domain domain, unsigned char options = 0);

			// Prevent value semantics
			TCPSocket(const TCPSocket&);
			TCPSocket& operator=(const TCPSocket&);
		
		};
	}
}


#endif // TCP_CLIENT_SOCKET_H
