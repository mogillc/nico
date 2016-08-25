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

#ifndef INCLUDE_MOGI_NETWORK_CONNECTEDSOCKET_H_
#define INCLUDE_MOGI_NETWORK_CONNECTEDSOCKET_H_

#include "mogi/network/SocketDescriptor.h"
#include "mogi/network/DatagramPacket.h"

namespace Mogi {
	namespace Network {

		/*! @class ConnectedSocket
		 * \brief A ConnectedSocket supports connections to remote hosts.
		 */
		class ConnectedSocket: public SocketDescriptor {
		public:
			virtual ~ConnectedSocket();

			/*! \brief Connects this socket to a remote host (i.e., an endpoint).
			 * @param endpoint The address (port & hostname) of the remote host.
			 */
			void connect(const SocketAddress& endpoint);

			/*! \brief Sends an array of bytes to the connected remote host.
			 * @param bytes A pointer to the first element of an array of bytes to send.
			 * @param size The size of the array of bytes.
			 * @param sendAll Set to true (default) to force transmission of all (amount = size) bytes. If false, send() may not send all bytes.
			 * @return The number of bytes actually sent.
			 */
			int send(const void* bytes, int size, bool sendAll = true) const;

			/*! \brief Receives an array of bytes from the connected remote host.
			 * @param bytes A pre-allocated array of bytes into which the received bytes will be copied.
			 * @param size The size of the array.
			 * @param receiveAll Set to true to force reception of all (amount equal to "size") bytes. If false (default), receive() may receive less than size bytes.
			 */
			int receive(void* bytes, int size, bool receiveAll = false) const;

			/*! \brief Returns the port of the connected remote host.
			 * @return The remote port.
			 */
			unsigned short int getRemotePort();

			/*! \brief Returns the address of the connected remote host.
			 * @return The remote address.
			 */
			std::string getRemoteIPAddress();

			/*! \brief Returns the SocketAddress (port and ip address) of the connected remote host.
			 * @return The SocketAddress identifying the remote host.
			 */
			SocketAddress getRemoteSocketAddress();

		protected:
			/*! \brief Constructs a socket with the given domain, type, and protocol.
			 * @param domain The domain (i.e., INET for IPv4 or INET6 for IPv6) of the socket.
			 * @param type The socket's type (i.e., DGRAM for datagram and STREAM for streaming sockets).
			 * @param protocol The socket's protocol (i.e., TCP or UDP)
			 */
			ConnectedSocket(Domain domain, Type type, Protocol protocol);

			/*! \brief Constructs a socket from an existing socket descriptor.
			 * @param sockfd The existing socket descriptor.
			 * @param domain The domain (i.e., INET for IPv4 or INET6 for IPv6) of the socket.
			 * @param type The socket's type (i.e., DGRAM for datagram and STREAM for streaming sockets).
			 * @param protocol The socket's protocol (i.e., TCP or UDP).
			 * @param options The options denoting the current state of the socket (e.g., connected, closed, bound).
			 */
			ConnectedSocket(int sockfd, Domain domain, Type type, Protocol protocol, unsigned char options = 0);
		};

	} /* namespace Network */
} /* namespace Mogi */

#endif /* INCLUDE_MOGI_NETWORK_CONNECTEDSOCKET_H_ */
