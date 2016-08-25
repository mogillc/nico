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

#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

#include "mogi/network/ConnectedSocket.h"
#include "mogi/network/DatagramPacket.h"

namespace Mogi {
	namespace Network {	

		/*! @class UDPSocket
		 * \brief A class representing a UDP socket.
		 *
		 * Why does this class inherit from ConnectedSocket if UDP is a "connectionless" protocol? Well, I'm glad you asked.
		 * Although UDP sockets can not be connected to a remote host, the socket library allows users to take advantage of a pseudo-connection
		 * to allow the use of send() and recv() instead of sendTo() and recvFrom(). The destination address used for the call to connect() is simply
		 * used within send() and recv() blah blah blah.
		 *
		 * Also, if users call connect on UDPSockets, then UDPSocket and TCPSocket can be use polymorphically as ConnectedSockets.
		 */
		class UDPSocket: public ConnectedSocket {
		public:

			/*! \brief Constructs an unbound UDPSocket.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 */
			UDPSocket(Domain domain = INET);

			/*! \brief Constructs a UDPSocket bound to the specified local interface and port.
			 * @param localAddress The local interface address to which this socket is bound.
			 * @param localPort The local port to which this socket is bound.
			 * @param blocking A boolean value the enables blocking (true) or nonblocking (false) behavior for send(), receive(), etc.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 */
			UDPSocket(const std::string& localAddress, unsigned short int localPort, bool blocking, Domain domain = INET);

			/*! \brief Constructs a UDPSocket bound to the specified local port and the host's IP address.
			 * @param localPort The local port to which this socket is bound.
			 * @param blocking A boolean value the enables blocking (true) or nonblocking (false) behavior for send(), receive(), etc.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 */
			UDPSocket(unsigned short int localPort, bool blocking, Domain domain = INET);

			/*! Sets the UDPSocket's multicast TTL.
			 * @param multicastTTL The multicast time-to-live setting.
			 * @return An integer error code. A value of zero implies success while a value less than zero implies an error. See: setsockopt().
			 */
			int setMulticastTTL(unsigned char multicastTTL);

			/*! Sets the UDPSocket's multicast group.
			 * @param multicastGroup The IP address of the multicast group to join.
			 * @return An integer error code. A value of zero implies success while a value less than zero implies an error. See: setsockopt().
			 */
			int joinMulticastGroup(const std::string& multicastGroup);

			/*! \brief Sends a datagram packet.
			 * @param packet The datagram packet containing both data and destination.
			 *
			 * If UDPSocket is set as blocking, this function may block. Otherwise, if set to nonblocking
			 * this function immediately returns with EAGAIN/EWOULDBLOCK in case of an error.
			 */
			int sendTo(const DatagramPacket& packet) const;

			/*! \brief Sends an array of bytes to a destination
			 * @param bytes The pointer to data array's first element.
			 * @param size The size of the data array.
			 * @param destination The SocketAddress representing the data's destination.
			 *
			 * If UDPSocket is set as blocking, this function may block. Otherwise, if set to nonblocking
			 * this function immediately returns with EAGAIN/EWOULDBLOCK in case of an error.
			 */
			int sendTo(const void* bytes, int size, const SocketAddress& destination) const;

			/*! \brief Tries to receive data which is copied to the specified buffer. The data's source is set as well.
			 * @param bytes The buffer into which received data will be copied.
			 * @param size The size of the bytes buffer.
			 * @param source A reference to the SocketAddress object into which the data's source will be stored.
			 * @return The number of bytes received and copied into the buffer. See: recvfrom().
			 *
			 * If UDPSocket is set as blocking, this function blocks until data is received or an error occurs. Otherwise, this
			 * function immediately returns if UDPSocket is nonblocking.
			 */
			int receiveFrom(void* bytes, int size, SocketAddress& source) const;

			/*! \brief Tries to receive data which is copied to the specified packet's buffer. The data's source is set as well.
			 * @param packet The packet into which the received data and source is stored.
			 * @return The number of bytes received and copied into the buffer. See: recvfrom().
			 *
			 * If UDPSocket is set as blocking, this function blocks until data is received or an error occurs. Otherwise, this
			 * function immediately returns if UDPSocket is nonblocking.
			 */
			int receiveFrom(DatagramPacket& packet) const;

		private:

			//bool initialize(const std::string& host, const std::string& port);

			UDPSocket(const UDPSocket&);
			UDPSocket& operator=(const UDPSocket&);
		
		};
	}
}


#endif // UDP_SOCKET_H
