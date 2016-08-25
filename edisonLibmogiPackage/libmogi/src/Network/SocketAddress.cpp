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

#include "mogi/network/SocketAddress.h"
#include <arpa/inet.h> // For inet_ntop() and inet_pton()
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <sstream>

using namespace Mogi::Network;

////////////////////////////////////////////
////         CONSTRUCTORS               ////
////////////////////////////////////////////

SocketAddress::SocketAddress(): valid(false) {

}

SocketAddress::SocketAddress(const sockaddr& addr): valid(true) {
	memcpy(&(this->addressStorage), &addr, sizeof(struct sockaddr));
}

SocketAddress::SocketAddress(const sockaddr_in& addr): valid(true) {
	memcpy(&(this->addressStorage), &addr, sizeof(struct sockaddr_in));
}

SocketAddress::SocketAddress(const sockaddr_in6& addr): valid(true) {
	memcpy(&(this->addressStorage), &addr, sizeof(struct sockaddr_in6));
}

SocketAddress::SocketAddress(const sockaddr_storage& addr): valid(true) {
	memcpy(&(this->addressStorage), &addr, sizeof(struct sockaddr_storage));
}

SocketAddress::SocketAddress(const std::string& hostname, unsigned short int port, int addressFamily): valid(true) {
	std::stringstream ss;
	ss << port;

	useGetAddrInfo(hostname, ss.str(), addressFamily);
/*	if (addressFamily == AF_INET) {
		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		inet_pton(AF_INET, ipAddr.c_str(),  &(addr.sin_addr));

		memcpy(&(this->addressStorage), &addr, sizeof(addr));
	}
	else if (addressFamily == AF_INET6) {
		struct sockaddr_in6 addr;

		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ipAddr.c_str(),  &(addr.sin6_addr));

		memcpy(&(this->addressStorage), &addr, sizeof(addr));
	}
	else {
		std::cerr << "SocketAddress() - unknown family " << addressFamily << std::endl;
	}
	*/
}

SocketAddress::SocketAddress(unsigned short int port, int addressFamily): valid(true) {
	std::stringstream ss;
	ss << port;

	useGetAddrInfo("", ss.str(), addressFamily);
	/*if (addressFamily == AF_INET) {
		struct sockaddr_in addr;

		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		memcpy(&(this->addressStorage), &addr, sizeof(addr));
	}
	else if (addressFamily == AF_INET6) {
		struct sockaddr_in6 addr;

		addr.sin6_family = AF_INET6;
		addr.sin6_port = htons(port);
		addr.sin6_addr = in6addr_any;

		memcpy(&(this->addressStorage), &addr, sizeof(addr));
	}
	else {
		std::cerr << "SocketAddress() - unknown family " << addressFamily << std::endl;
	}*/
}

void SocketAddress::useGetAddrInfo(const std::string& host, const std::string& port, int addressFamily) {
	bool bindLocal = host.empty();
	struct addrinfo hints;
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int result = 0;

	// Initialize hints structure
	memset(&hints, 0, sizeof hints);
	hints.ai_family = addressFamily;//AF_UNSPEC; // Use either IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP

	// Get linked list of addrinfo structures
	if (bindLocal) {
		hints.ai_flags = AI_PASSIVE; // use my IP
		result = ::getaddrinfo(
				NULL,
				port.c_str(),
				&hints,
				&servinfo);
	}
	else {
		result = ::getaddrinfo(
				host.c_str(),
				port.c_str(),
				&hints,
				&servinfo);
	}

	if (result != 0) {
		std::cerr << "TCPClientSocket::initialize() - failed to getaddrinfo" << std::endl;
		valid = false;
		return;
	}

    //loop through servinfo linked list
    for (p = servinfo; p != NULL; p = p->ai_next) {
    	memcpy(&(this->addressStorage), p->ai_addr, p->ai_addrlen);
    	valid = true;
        break;
    } //end for loop


    ::freeaddrinfo(servinfo); // Free memory for linked list
}

SocketAddress::SocketAddress(const SocketAddress& other): valid(true) {
	memcpy(&(this->addressStorage), &(other.addressStorage), sizeof(struct sockaddr_storage));
}

SocketAddress& SocketAddress::operator=(const SocketAddress& other) {
	if (this != &other) {
		memcpy(&(this->addressStorage), &(other.addressStorage), sizeof(struct sockaddr_storage));
		this->valid = other.valid;
	}

	return *this;
}

bool SocketAddress::equalIP(const SocketAddress& other) const {
	unsigned char* addrBytes = 0;
	unsigned char* otherAddrBytes = 0;
	unsigned short int addrBytesSize = 0;
	bool isEqual = false;
	unsigned short int family = this->getFamily();

	// First, check family (IPv4 or IPv6) equality 
	if (family == other.getFamily()) {
		
		// Check address bytes equality
		addrBytesSize = this->getIPAddressSize();
		addrBytes = new unsigned char[addrBytesSize];
		otherAddrBytes = new unsigned char[addrBytesSize];

		if (this->getIPAddressBytes(addrBytes, addrBytesSize) > 0 &&
			other.getIPAddressBytes(otherAddrBytes, addrBytesSize) > 0) {

			isEqual = memcmp(addrBytes, otherAddrBytes, addrBytesSize) == 0; // Compare
		}
		else {
			std::cerr << "SocketAddress::equalIP() - could not get addr bytes " << std::endl;
			isEqual = false;
		}

		// Clean-up
		delete[] addrBytes;
		delete[] otherAddrBytes;
		
	}

	return isEqual;
}

bool SocketAddress::equalPort(const SocketAddress& other) const {
	return this->getPort() == other.getPort();
}

////////////////////////////////////////////
////            GETTERS                 ////
////////////////////////////////////////////

unsigned short int SocketAddress::getPort(bool inHostByteOrder) const {
	unsigned short int port = 0;
	unsigned short int family = this->getFamily();

	if (family == AF_INET) {
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)&(this->addressStorage); 
		port = ipv4->sin_port;
	}
	else if (family == AF_INET6) {
		struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&(this->addressStorage); 
		port = ipv6->sin6_port;
	}
	else {
		// Not using IPv4 or IPv6... wtf?
		std::cerr << "SocketAddress::getIPAddressSize() - unknown family type: " << family << std::endl;
		port = 0;
	}

	if (inHostByteOrder) {
		port = ntohs(port);
	}

	return port;
}

void SocketAddress::setPort(unsigned short int port, bool portInHostByteOrder) {
	unsigned short int family = this->getFamily();

	if (family == AF_INET) {
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)&(this->addressStorage); 

		if (portInHostByteOrder) {
			ipv4->sin_port = htons(port);
		}
		else {
			ipv4->sin_port = port;
		}
	}
	else if (family == AF_INET6) {
		struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&(this->addressStorage); 

		if (portInHostByteOrder) {
			ipv6->sin6_port = htons(port);
		}
		else {
			ipv6->sin6_port = port;
		}
	}
	else {
		// Not using IPv4 or IPv6... wtf?
		std::cerr << "SocketAddress::setPort() - unknown family type: " << family << std::endl;
	}	
}


		
std::string SocketAddress::getIPAddress() const {
	std::string ipAddress = "";
	
	unsigned short int size = this->getIPAddressSize();
	unsigned short int family = this->getFamily();
	char strArray[INET6_ADDRSTRLEN];

	if (family == AF_INET) {
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)&(this->addressStorage); 
		inet_ntop(AF_INET, &(ipv4->sin_addr), strArray, INET6_ADDRSTRLEN);
	}
	else if (family == AF_INET6) {
		struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&(this->addressStorage);
		inet_ntop(AF_INET, &(ipv6->sin6_addr), strArray, INET6_ADDRSTRLEN);
	}
	else {
		// Not using IPv4 or IPv6... wtf?
		std::cerr << "SocketAddress::getIPAddressBytes() - unknown family type: " << family << std::endl;
	}
	
	ipAddress = strArray;

	return ipAddress;	
}
	
unsigned short int SocketAddress::getIPAddressSize() const {
	unsigned short int size = 0;
	unsigned short int family = this->getFamily();

	if (family == AF_INET) {
		struct sockaddr_in* ipv4 = (struct sockaddr_in*)&(this->addressStorage); 
		size = sizeof(ipv4->sin_addr.s_addr);
	}
	else if (family == AF_INET6) {
		struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&(this->addressStorage); 
		size = sizeof(ipv6->sin6_addr.s6_addr);
	}
	else {
		// Not using IPv4 or IPv6... wtf?
		std::cerr << "SocketAddress::getIPAddressSize() - unknown family type: " << family << std::endl;
		size = 0;
	}
	
	return size;
}
	
unsigned int SocketAddress::getIPAddressBytes(void* buffer, unsigned int bufferSize, bool inHostByteOrder) const {
	unsigned int bytesCopied = 0;
	unsigned short int size = this->getIPAddressSize();
	unsigned short int family = this->getFamily();

	if (bufferSize >= size) {
		if (family == AF_INET) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)&(this->addressStorage); 
			unsigned int ip = ipv4->sin_addr.s_addr;
						
			if (!inHostByteOrder) {
				ip = htonl(ip);
			}

			memcpy(buffer, &ip, sizeof(ip));
			bytesCopied = sizeof(ip);
		}
		else if (family == AF_INET6) {
			struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)&(this->addressStorage);
			const unsigned char* ip = ipv6->sin6_addr.s6_addr;

			memcpy(buffer, ip, sizeof(ip));
			bytesCopied = sizeof(ip);
		}
		else {
			// Not using IPv4 or IPv6... wtf?
			std::cerr << "SocketAddress::getIPAddressBytes() - unknown family type: " << family << std::endl;
			bytesCopied = 0;
		}
	}
	else {
		bytesCopied = 0;
	}

	return bytesCopied;
}


unsigned short int SocketAddress::getFamily() const {
	return this->addressStorage.ss_family;
}


struct sockaddr* SocketAddress::getCSockAddr() const {
	return (struct sockaddr*)&(this->addressStorage);
}

unsigned int SocketAddress::getCSockAddrSize() const {
	unsigned short int size = 0;
	unsigned short int family = this->getFamily();

	if (family == AF_INET) {
		size = sizeof(struct sockaddr_in);
	}
	else if (family == AF_INET6) {
		size = sizeof(struct sockaddr_in6);
	}
	else {
		// Not using IPv4 or IPv6... wtf?
		std::cerr << "SocketAddress::getCSockAddrSize() - unknown family type: " << family << std::endl;
		size = sizeof(struct sockaddr_storage);
	}
	
	return size;	
}


std::string SocketAddress::toString() const {
	std::stringstream ss;

	ss << this->getIPAddress() << ":" << this->getPort();
	return ss.str();
}

bool SocketAddress::isValid() const {
	return this->valid;
}

SocketAddress Mogi::Network::SocketAddress::getAddressInfo(
		const std::string& hostName, const std::string& service,
		int addressFamily) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo, *p;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = addressFamily;     // IPv4 or IPv6

	if (hostName.empty()) {
		hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
		status = getaddrinfo(NULL, service.c_str(), &hints, &servinfo);
	}
	else {
		status = getaddrinfo(hostName.c_str(), service.c_str(), &hints, &servinfo);
	}

	if (status == 0) {
		for(p = servinfo;p != NULL; p = p->ai_next) {
			return SocketAddress(*p->ai_addr);
		}
	}
	return SocketAddress();
}
