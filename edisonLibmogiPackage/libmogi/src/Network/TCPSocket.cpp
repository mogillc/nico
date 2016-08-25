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

#include "mogi/network/TCPSocket.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>

using namespace Mogi::Network;

TCPSocket::TCPSocket(Domain domain): ConnectedSocket(domain, STREAM, TCP) {

}


TCPSocket::TCPSocket(const std::string& remoteHost, unsigned short int remotePort, bool blocking, Domain domain) : ConnectedSocket(domain, STREAM, TCP) {
	SocketAddress rAddress(remoteHost, remotePort, domain);
	connect(rAddress);
	setNonBlocking(!blocking);
}

TCPSocket::TCPSocket(int sockfd, Domain domain, unsigned char options) : ConnectedSocket(sockfd, domain, DGRAM, TCP, options) {

}

/*
bool TCPSocket::initialize(const std::string& host, const std::string& port) {
	bool bindLocal = host.empty();
	bool success = false;
	char ipstr[INET6_ADDRSTRLEN];
	struct addrinfo hints;
	struct addrinfo* servinfo;
	struct addrinfo* p;
	int result = 0;
	int yes = 1;

	// Initialize hints structure
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // Use either IPv4 or IPv6
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
			std::cout << "TCPClientSocket::initialize() - Did not create socket descriptor" << std::endl;
		 	continue; //try next struct
	   	}
	
	  	if (bindLocal) {
			// Reusable port
			//if (::setsockopt(this->socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	  		if (!this->setReuseAddress(true)){
				std::cout << "TCPServerSocket::initialize() - Could not set SO_REUSEADDR" << std::endl;
				p = NULL;
				break;
			}
		
			// Try to bind
		  	//if ((::bind(this->socketDescriptor,p->ai_addr,p->ai_addrlen)) == -1){
	  		if (!this->bind(*p->ai_addr)) {
				std::cout << "TCPServerSocket::initialize() - failed to bind."  << std::endl;
				this->close();
			   	continue; // Try next struct
		  	}
	  	}
	  	else {
			// Try to connect
			//if ((::connect(this->socketDescriptor,p->ai_addr,p->ai_addrlen)) == -1){
	  		if (!this->connect(*p->ai_addr)) {
				std::cout << "TCPClientSocket::initialize() - Did not connect." << std::endl;
				this->close();
				continue; // Try next struct
			}
	  	}
        break;
    } //end for loop
    
    if (p == NULL) {
        std::cerr << "TCPClientSocket::initialize() - failed to get socket descriptor and/or connect" << std::endl;
        success = false; // failed to create/connect!!!!
    }
	else {
		success = true;
	}

    ::freeaddrinfo(servinfo); // Free memory for linked list
    return success;
	

}
*/




