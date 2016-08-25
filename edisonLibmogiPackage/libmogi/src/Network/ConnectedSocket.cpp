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

/*
 * ConnectedSocket.cpp
 *
 *  Created on: May 2, 2016
 *      Author: adrian
 */

#include "mogi/network/ConnectedSocket.h"

using namespace Mogi::Network;

ConnectedSocket::ConnectedSocket(Domain domain, Type type, Protocol protocol): SocketDescriptor(domain, type, protocol) {

}

ConnectedSocket::ConnectedSocket(int sockfd, Domain domain, Type type, Protocol protocol, unsigned char options) : SocketDescriptor(sockfd, domain, type, protocol, options) {

}

ConnectedSocket::~ConnectedSocket() {

}

void ConnectedSocket::connect(const SocketAddress& endpoint) {
	if ((::connect(this->socketDescriptor, endpoint.getCSockAddr(), endpoint.getCSockAddrSize())) < 0) {
		throw SocketException("Failed to connect to endpoint");
	}

	this->remoteAddress = endpoint;
	this->setFlagOptions(SOCKET_CONNECTED, true);
}

int ConnectedSocket::send(const void* bytes, int size, bool sendAll) const {
	const char* ptr = (const char*)bytes;
	int bytesRemaining = size;
	int sent = 0;

	while (bytesRemaining > 0) {
		sent = ::send(this->socketDescriptor, ptr, bytesRemaining, 0);

		if (sent <= 0 || !sendAll) {
			return sent;
		}

		bytesRemaining -= sent;
		ptr += sent;
	}

	return size;
}

int ConnectedSocket::receive(void* bytes, int size,
		bool receiveAll) const {

	char* ptr = (char*)bytes;
	int bytesRemaining = size;

	while (bytesRemaining > 0) {
		int recvd = ::recv(this->socketDescriptor, ptr, bytesRemaining, 0);

		if (recvd <= 0 || !receiveAll) {
			return recvd;
		}

		bytesRemaining -= recvd;
		ptr += recvd;
	}

	return size;
}


unsigned short int ConnectedSocket::getRemotePort() {
	if (!this->remoteAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getpeername(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->remoteAddress = addr;
		}
	}

	return this->remoteAddress.getPort();
}

std::string ConnectedSocket::getRemoteIPAddress() {
	if (!this->remoteAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getpeername(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->remoteAddress = addr;
		}
	}

	return this->remoteAddress.getIPAddress();
}

Mogi::Network::SocketAddress ConnectedSocket::getRemoteSocketAddress() {
	if (!this->remoteAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getpeername(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->remoteAddress = addr;
		}
	}

	return this->remoteAddress;
}
