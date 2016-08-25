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

#include "mogi/network/UDPSocket.h"
#include <iostream>
#include <cstring>
#include <sstream>

using namespace Mogi::Network;

UDPSocket::UDPSocket(Domain domain) : ConnectedSocket(domain, DGRAM, UDP) {

}

UDPSocket::UDPSocket(const std::string& localHost, unsigned short int localPort, bool blocking, Domain domain): ConnectedSocket(domain, DGRAM, UDP) {
	SocketAddress lAddress(localHost, localPort, domain);
	if (!setReuseAddress(true)) {throw SocketException("UDPSocket(): Failed to setReuseAddress()");}
	if (!bind(lAddress)) {throw SocketException("UDPSocket(): Failed to bind()");}
	setNonBlocking(!blocking);
}

UDPSocket::UDPSocket(unsigned short int localPort, bool blocking, Domain domain): ConnectedSocket(domain, DGRAM, UDP) {
	SocketAddress lAddress(localPort, domain);
	if (!setReuseAddress(true)) {throw SocketException("UDPSocket(): Failed to setReuseAddress()");}
	if (!bind(lAddress)) {throw SocketException("UDPSocket(): Failed to bind()");}
	setNonBlocking(!blocking);
}


int UDPSocket::setMulticastTTL(unsigned char multicastTTL) {
	return setsockopt(
			this->socketDescriptor,
			IPPROTO_IP,
			IP_MULTICAST_TTL,
			&multicastTTL,
			sizeof (multicastTTL)
	);
}

int UDPSocket::joinMulticastGroup(const std::string& multicastGroup) {
	struct ip_mreq multicastReq;

	multicastReq.imr_multiaddr.s_addr = inet_addr(multicastGroup.c_str());
	multicastReq.imr_interface.s_addr = htonl(INADDR_ANY);

	return setsockopt(
			this->socketDescriptor,
			IPPROTO_IP,
			IP_ADD_MEMBERSHIP,
			&multicastReq,
			sizeof(multicastReq)
	);
}


int UDPSocket::sendTo(const void* bytes, int size, const SocketAddress& destination) const {
	const struct sockaddr* to = destination.getCSockAddr();
	unsigned int tolen = destination.getCSockAddrSize();

	return ::sendto(this->socketDescriptor, bytes, size, 0, to, tolen);
}

int UDPSocket::receiveFrom(void* bytes, int size, SocketAddress& source) const {
	struct sockaddr_storage from;
	unsigned int fromlen = sizeof(from);

	int recvd = ::recvfrom(this->socketDescriptor, bytes, size, 0, (struct sockaddr*)&from, &fromlen);

	source = from;
	return recvd;
}

int UDPSocket::sendTo(const DatagramPacket& packet) const {
	return UDPSocket::sendTo(packet.getPacketBuffer(), packet.getBufferLength(), packet.getSocketAddress());
}

int UDPSocket::receiveFrom(DatagramPacket& packet) const {
	return UDPSocket::receiveFrom(packet.getPacketBuffer(), packet.getBufferLength(), packet.getSocketAddress());
}





/*
bool UDPSocket::initialize(const std::string& host, const std::string& port) {
	bool success = false;
	bool bindLocal = host.empty();
	char ipstr[INET6_ADDRSTRLEN];
	struct addrinfo hints;
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int result = 0;

	// Initialize hints structure
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // Use either IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP

	if (bindLocal) {
		hints.ai_flags = AI_PASSIVE; // use my IP
	}

	// Get linked list of addrinfo structures
	if (bindLocal) {
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
		std::cerr << "UDPSocket::initialize() - failed to getaddrinfo" << std::endl;
	}

    //loop through servinfo linked list and connect using the first one that works
    for (p = servinfo; p != NULL; p = p->ai_next) {

		// Print info about this addrinfo struct
		void* _addr;
		const char* _ipver;

		if (p->ai_family == AF_INET) {
			struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
			_addr = &(ipv4->sin_addr);
			_ipver = "IPv4";
		}
		else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
			_addr = &(ipv6->sin6_addr);
			_ipver = "IPv6";
		}

		inet_ntop(p->ai_family, _addr, ipstr, sizeof ipstr);
		//std::cout << _ipver << ": " << ipstr << std::endl;

		// Try to create socket descriptor
	  	if ( (this->socketDescriptor = ::socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1 ) {
			std::cout << "UDPSocket::initialize() - Did not create socket descriptor" << std::endl;
		 	continue; //try next struct
	   	}
	
		if (bindLocal) {
			// Reusable port
			//if (::setsockopt(this->socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	  		if (!this->setReuseAddress(true)){
				std::cout << "UDPSocket::initialize() - Could not set SO_REUSEADDR" << std::endl;
				p = NULL;
				break;
			}

			// Try to bind
			//if ((::bind(this->socketDescriptor,p->ai_addr,p->ai_addrlen)) == -1) {
			if (!this->bind(*p->ai_addr)) {
				std::cout << "UDPSocket::initialize() - Did not bind" << std::endl;
				this->close();//::close(this->socketDescriptor);
				continue;
			}
		}
		else {
			// Try to connect
		  	//if ((::connect(this->socketDescriptor,p->ai_addr,p->ai_addrlen)) == -1){
			if (!this->connect(*p->ai_addr)) {
				std::cout << "UDPSocket::initialize() - Did not connect." << std::endl;
			  	this->close();//::close(this->socketDescriptor);
			   	continue; // Try next struct
		  	}
		}

        
        break;
    } //end for loop
    
    if (p == NULL) {
        std::cerr << "UDPSocket::initialize() - failed to get socket descriptor and/or connect/bind" << std::endl;
        success = false; // failed to create/connect!!!!
    }
	else {
		success = true;
	}

    ::freeaddrinfo(servinfo); // Free memory for linked list
    return success;
	
}
*/

















