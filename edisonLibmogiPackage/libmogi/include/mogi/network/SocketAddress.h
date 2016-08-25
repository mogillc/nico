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

#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H
#include <netinet/in.h> // For sockaddr
#include <string>


namespace Mogi {
	namespace Network {

		/*! @class SocketAddress
		 * \brief A SocketAddress encompasses useful functionality associated with C socket addresses.
		 *
		 * SocketAddress can be initialized with a sockaddr, sockaddr_in, etc. structure to allow users to query
		 * information such as port and IP address.
		 *
		 * Additionally, it can be initialized with an IP address/hostname/port
		 * and the appropriate address structure will be resolved using getaddrinfo(). Afterwards, users
		 * can access the raw socketaddr structure for use with socket functions like bind() or sendTo().
		 */
		class SocketAddress {
		public:		
			/*!
			 * \brief Constructs an empty (i.e., invalid) SocketAddress.
			 */
			SocketAddress();

			/*!
			 * \brief Constructs SocketAddress from a sockaddr structure.
			 * @param addr The sockaddr structure from which to initialize this SocketAddress.
			 */
			SocketAddress(const sockaddr& addr);

			/*!
			 * \brief Constructs IPv4 SocketAddress from a sockaddr_in structure.
			 * @param addr The sockaddr_in structure from which to initialize this SocketAddress.
			 */
			SocketAddress(const sockaddr_in& addr);

			/*!
			 * \brief Constructs IPv6 SocketAddress from a sockaddr_in6 structure.
			 * @param addr The sockaddr_in6 structure from which to initialize this SocketAddress.
			 */
			SocketAddress(const sockaddr_in6& addr);

			/*!
			 * \brief Constructs SocketAddress from a sockaddr_storage structure.
			 * @param addr The sockaddr_storage structure from which to initialize this SocketAddress.
			 */
			SocketAddress(const sockaddr_storage& addr);

			/*!
			 * \brief Constructs SocketAddress from a hostname (or IP address) and port.
			 * @param hostname The hostname or IP address.
			 * @param port The port.
			 * @param addressFamily The address family (or domain). Acceptable values are AF_INET (default) and AF_INET6 for IPv4 and IPv6 respectively.
			 */
			SocketAddress(const std::string& hostname, unsigned short int port, int addressFamily = AF_INET);

			/*!
			 * \brief Constructs SocketAddress using this machines IP address and the specified port.
			 * @param port The port.
			 * @param addressFamily The address family (or domain). Acceptable values are AF_INET (default) and AF_INET6 for IPv4 and IPv6 respectively.
			 */
			SocketAddress(unsigned short int port, int addressFamily = AF_INET);

			/*! \brief Copy constructor
			 * @param other The other SocketAddress from which to initialize this SocketAddress.
			 */
			SocketAddress(const SocketAddress& other); // copy constructor

			/*! \brief Assignment operator used to replace this SocketAddress's data with another's data.
			 * @param other The other SocketAddress from which to reinitialize this SocketAddress.
			 */
			SocketAddress& operator=(const SocketAddress& other); // assignment operator

			/*! \brief Returns true if the other SocketAddress uses the same IP address.
			 * @param other The other SocketAddress
			 * @return A boolean value that is true if both SocketAddresses represent the same IP address, and false otherwise.
			 */
			bool equalIP(const SocketAddress& other) const;

			/*! \brief Returns true if the other SocketAddress uses the same port number.
			 * @param other The other SocketAddress
			 * @return A boolean value that is true if both SocketAddresses represent the same port, and false otherwise.
			 */
			bool equalPort(const SocketAddress& other) const;

			/*! \brief Returns this SocketAddress's port.
			 * @param inHostByteOrder A boolean value that the caller sets to true (default) if the port should be returned in host-byte-order.
			 * @return The port associated with this SocketAddress.
			 */
			unsigned short int getPort(bool inHostByteOrder = true) const; 

			/*! \brief Sets the port associated with this SocketAddress.
			 * @param port The new port to set.
			 * @param portInHostByteOrder A boolean value that the caller sets to true (default) if the port is in host-byte-order. This should be set to false if in network byte order.
			 */
			void setPort(unsigned short int port, bool portInHostByteOrder = true); 

			/*! \brief Returns this SocketAddress's IP address.
			 * @return The IP address associated with this SocketAddress.
			 */
			std::string getIPAddress() const;	

			/*! \brief Returns the size of this SocketAddress's IP address. This is necessary due to the size difference between IPv4 and IPv6 addresses.
			 * @return The size in bytes of the IP address associated with this SocketAddress.
			 */
			unsigned short int getIPAddressSize() const;	

			/*! \brief Packs this SocketAddress's IP address into the provided buffer.
			 * @param buffer The pre-allocated buffer into which the IP address will be packed.
			 * @param bufferSize The size of the buffer.
			 * @param inHostByteOrder A boolean value that the caller sets to true (default) if the IP address should be set in host-byte-order.
			 * @return The number of bytes copied into the buffer.
			 */
			unsigned int getIPAddressBytes(void* buffer, unsigned int bufferSize, bool inHostByteOrder = true) const;

			/*! \brief Returns this SocketAddress's family/domain (i.e., AF_INET or AF_INET6 for IPv4 or IPv6 respectively)
			 * @return The family encoded as an integer.
			 */
			unsigned short int getFamily() const;

			/*! \brief Returns a pointer to this SocketAddress's raw C sockaddr structure.
			 * This sockaddr can be used for calls to bind(), sendTo(), etc.
			 * @return A pointer to the raw sockaddr structure.
			 */
			struct sockaddr* getCSockAddr() const;

			/*! \brief Returns the size of the underlying raw sockaddr structure.
			 * @return The size of the underlying raw sockaddr structure.
			 */
			unsigned int getCSockAddrSize() const;

			/*! Returns a string representation (e.g., "192.168.0.2 : 80") of this SocketAddress's IP address and port.
			 * @return A string representation (e.g., "192.168.0.2 : 80") of this SocketAddress's IP address and port.
			 */
			std::string toString() const;

			/*! Returns a boolean flag indicating if this SocketAddress was successfully initialized. For example, it could return false
			 * if initialized with an invalid hostname.
			 * @return A boolean value indicating if this SocketAddress was successfully initialized. The value is true if valid and false otherwise.
			 */
			bool isValid() const;

			/*! \brief Static class function uses getaddrinfo() to resolve a hostname/port and return an initialized SocketAddress.
			 * @return An SocketAddress object initialized using getaddrinfo().
			 */
			static SocketAddress getAddressInfo(const std::string& hostName, const std::string& service, int addressFamily = AF_UNSPEC);
		

		private:
			void useGetAddrInfo(const std::string& host, const std::string& port, int addressFamily);
			struct sockaddr_storage addressStorage; // Raw sockaddr structure
			bool valid; // Flag indicating validity of raw sockaddr structure
		};
	}
}

#endif // SOCKET_ADDRESS_H
