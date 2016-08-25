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

#ifndef SOCKET_DESCRIPTOR_H
#define SOCKET_DESCRIPTOR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string>
#include <exception>

#include "mogi/network/SocketAddress.h"

namespace Mogi {
	namespace Network {

		/*! \class SocketException
		 * \brief Exception type thrown by SocketDescriptor classes.
		 */
		class SocketException : public std::exception {
		public:
			/*! \brief Create a SocketException with a message.
			 * @param message The exception message.
			 */
			SocketException(const std::string& message);

			/*!
			 * \brief Destructor (RIP)
			 */
			~SocketException() throw() {}

			/*! \brief Returns the exception message
			 * @return The exception message
			 */
			const char* what();
		private:
			std::string exceptionMessage; // The exception message
		};

		/*! \class SocketDescriptor
		 * \brief Base class representing a network socket.
		 * Supports IPv4 and IPv6 domains and UDP and TCP protocols.
		 */
		class SocketDescriptor {
		public:
			/*! \enum Type
			 *  \brief Specifies the socket's type (datagram or streaming)
			 */
			enum Type {
				DGRAM = SOCK_DGRAM, // Datagram
				STREAM = SOCK_STREAM // Stream
			};

			/*! \enum Protocol
			 * \brief Specifies the socket's protocol (UDP or TCP)
			 */
			enum Protocol {
				UDP = IPPROTO_UDP, // UDP
				TCP = IPPROTO_TCP // TCP
			};

			/*! \enum Domain
			 * \brief Specifies the socket's domain (IPv4 or IPv6)
			 */
			enum Domain {
				INET = PF_INET, // IP version 4
				INET6 = PF_INET6 // IP version 6
			};

			/*!
			 * \brief Destructor closes the underlying socket.
			 */
			~SocketDescriptor() {
				if (this->isValid()) {
					this->close();
				}
			}

			/*! \brief Returns the raw socket descriptor
			 * @return The integer representing a socket descriptor.
			 */
			int getRaw() const {
				return this->socketDescriptor;
			}

			/*! \brief Returns the socket's domain.
			 * @return The socket's domain (e.g., INET or INET6).
			 */
			Domain getSocketDomain() const {
				return this->socketDomain;
			}

			/*! \brief Returns the socket's type.
			 * @return The socket's type (e.g., DGRAM or STREAM).
			 */
			Type getSocketType() const {
				return this->socketType;
			}

			/*! \brief Returns the socket's protocol.
			 * @return The socket's protocol (e.g., UDP or TCP).
			 */
			Protocol getSocketProtocol() const  {
				return this->socketProtocol;
			}

			/*! \brief Returns a boolean specifying the validity of the underlying socket descriptor.
			 * @return A boolean value that's true if the socket descriptor is valid, and false otherwise.
			 */
			bool isValid() const {
				return (this->socketDescriptor != -1);
			}

			/*! \brief Sets the socket to blocking or nonblocking. This affects sending, receiving, accepting, etc.
			 * @param nonblocking Boolean value should be set to true for nonblocking behavior and false for blocking behavior.
			 */
			void setNonBlocking(bool nonblocking);

			/*! \brief Function used to specify if a port can be reused by a socket.
			 * @param on Boolean value set to true if reuse should be allowed, and false otherwise.
			 */
			bool setReuseAddress(bool on);

			/*! \brief Binds a socket to a local endpoint (port and address)
			 * @param bindpoint The local endpoint to which the socket will be bound.
			 */
			bool bind(const SocketAddress& bindpoint);

			/*! \brief Closes the socket.
			 */
			void close();

			/*! \brief Returns a boolean value indicating if the socket is currently bound to a user-specified local endpoint.
			 * @return A boolean value indicating if socket is currently bound (true) or not (false).
			 */
			bool isBound() const;

			/*! \brief Returns a boolean value indicating if the socket is currently connected to a remote endpoint.
			 * @return A boolean value indicating if socket is currently connected (true) or not (false).
			 */
			bool isConnected() const;

			/*! \brief Returns a boolean value indicating if the socket is currently closed.
			 * @return A boolean value indicating if socket is currently closed (true) or not (false).
			 */
			bool isClosed() const;

			/*! \brief Returns a boolean value indicating if the socket is capable of reusing a port.
			 * @return A boolean value indicating if socket is capable (true) of reusing a port or not (false).
			 */
			bool getReuseAddress() const;

			/*! \brief Returns the port to which the socket is bound.
			 * @return An unsigned integer representing the local port to which the socket is bound.
			 */
			unsigned short int getLocalPort();

			/*! \brief Returns the IP address to which the socket is bound.
			 * @return An std::string representing the local address to which the socket is bound.
			 */
			std::string getLocalIPAddress();

			/*! \brief Returns the SocketAddress (port and IP address) to which the socket is bound.
			 * @return A SocketAddress object representing the local port and IP address to which the socket is bound.
			 */
			SocketAddress getLocalSocketAddress();

		protected:
			/*!
			 * \brief The raw socket file descriptor.
			 */
			int socketDescriptor;

			/*!
			 * \brief The socket's IP domain.
			 */
			Domain socketDomain;

			/*!
			 * \brief The socket's type (e.g., DGRAM or STREAM).
			 */
			Type socketType;

			/*!
			 * \brief The socket's protocol (e.g., UDP or TCP).
			 */
			Protocol socketProtocol;

			/*!
			 * \brief The socket's last known remote endpoint.
			 */
			SocketAddress remoteAddress;

			/*!
			 * \brief The socket's last known local bind point.
			 */
			SocketAddress localAddress;

			/*!
			 * \brief Stores status flags in a byte.
			 */
			unsigned char statusFlags;

			/*!
			 * \brief Status flag indicating the socket is bound.
			 * TODO: All sockets are bound to a local address even if user doesn't call bind().
			 * Therefore, this may be a useless flag.
			 */
			static const unsigned char SOCKET_BOUND = 0x01;

			/*!
			 * \brief Status flag indicating the socket is connected to a remote host.
			 */
			static const unsigned char SOCKET_CONNECTED = 0x02;

			/*!
			 * \brief Status flag indicating the socket is closed.
			 */
			static const unsigned char SOCKET_CLOSED = 0x04;

			/*!
			 * \brief Status flag indicating the socket is non-blocking.
			 */
			static const unsigned char SOCKET_NONBLOCKING = 0x08;

			/*! \brief Constructs a SocketDescriptor. This constructor throws a SocketException if unable to create a socket via socket().
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 * @param type The socket's type (e.g., DGRAM for datagram or STREAM for streaming).
			 * @param protocol The socket's protocol (e.g., UDP or TCP).
			 */
			SocketDescriptor(Domain domain, Type type, Protocol protocol): socketDomain(domain), socketType(type), socketProtocol(protocol), statusFlags(0) {
				// Create socket here
				if ((socketDescriptor = ::socket(domain, type, protocol)) < 0) {
					// Throw exception?
					throw SocketException("Failed to create socket");
				}
			}

			/*! \brief Constructs a SocketDescriptor from an existing socket file descriptor.
			 * @param sockfd The existing socket file descriptor.
			 * @param domain The domain (e.g., INET for IPv4 or INET6 for IPv6) of the socket. The default value is INET.
			 * @param type The socket's type (e.g., DGRAM for datagram or STREAM for streaming).
			 * @param protocol The socket's protocol (e.g., UDP or TCP).
			 * @param options A byte indicating the socket's status flags.
			 */
			SocketDescriptor(int sockfd, Domain domain, Type type, Protocol protocol, unsigned char options = 0) : socketDescriptor(sockfd), socketDomain(domain), socketType(type), socketProtocol(protocol), statusFlags(options){

			}

			/*!
			 * \brief Sets a flag option on or off.
			 * @param options The options to turn off or on (e.g., SOCKET_BOUND | SOCKET_NONBLOCKING).
			 * @param on A boolean flag indicating whether to turn the options on (true) or off (false).
			 */
			void setFlagOptions(const unsigned char options, bool on) {
				if (on) {
					this->statusFlags |= options;
				}
				else {
					this->statusFlags &= ~options;
				}
			}

			/*!
			 * \brief Returns the status of an option.
			 * @param option The option whose status to query.
			 * @return A boolean flag indicating whether the option is on (true) or off (false).
			 */
			bool getFlagOption(const unsigned char option) const {
				return this->statusFlags & option;
			}

		private:
			// Prevent value semantics
			SocketDescriptor(const SocketDescriptor&);
			SocketDescriptor& operator=(const SocketDescriptor&);

		};

	}
}


#endif // SOCKET_DESCRIPTOR_H
