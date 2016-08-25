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
 * SocketDescriptor.cpp
 *
 *  Created on: Mar 22, 2016
 *      Author: adrian
 */

#include "mogi/network/SocketDescriptor.h"
#include <iostream>

using namespace Mogi::Network;

SocketException::SocketException(const std::string& message) : exceptionMessage(message) {}

const char* SocketException::what() {
	return exceptionMessage.c_str();
}

bool SocketDescriptor::setReuseAddress(bool on) {
	bool success = false;
	int yes = on ? 1 : 0;

	if (this->isValid()) {
		success = ::setsockopt(this->socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 0;
	}

	return success;
}

bool SocketDescriptor::getReuseAddress() const {
	bool success = false;
	int optval = 0;
	unsigned int optlen = sizeof(optval);

	if (this->isValid()) {
		if (::getsockopt(this->socketDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen) != 0) {
			std::cerr << "SocketDescriptor::getReuseAddress() - error calling getsockopt" << std::endl;
		}
	}

	return optval != 0;
}

void SocketDescriptor::setNonBlocking(bool nonblocking) {
	int opts = ::fcntl(this->socketDescriptor, F_GETFL);

	if (opts < 0) {
		return;
	}

	if (nonblocking){
		opts = (opts | O_NONBLOCK);
	} else {
		opts = (opts & ~O_NONBLOCK);
	}

	::fcntl(this->socketDescriptor, F_SETFL, opts);

	this->setFlagOptions(SOCKET_NONBLOCKING, nonblocking);
}

bool SocketDescriptor::bind(const SocketAddress& bindpoint) {
	bool success = false;
	struct sockaddr* addr;

	if (this->isValid()) {
		addr = bindpoint.getCSockAddr();
		success = ::bind(this->socketDescriptor, addr, bindpoint.getCSockAddrSize()) == 0;
	}

	if (success) {
		this->localAddress = bindpoint;
		this->setFlagOptions(SOCKET_BOUND, true);
	}

	return success;
}





void SocketDescriptor::close() {
	::close(this->socketDescriptor);
	this->socketDescriptor = -1;

	this->setFlagOptions(SOCKET_CLOSED, true);
	this->setFlagOptions(SOCKET_BOUND | SOCKET_CONNECTED, false);
}


bool SocketDescriptor::isBound() const {
	return this->getFlagOption(SOCKET_BOUND);
}

bool SocketDescriptor::isConnected() const {
	return this->getFlagOption(SOCKET_CONNECTED);
}

bool SocketDescriptor::isClosed() const {
	return this->getFlagOption(SOCKET_CLOSED);
}

unsigned short int SocketDescriptor::getLocalPort() {
	if (!this->localAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getsockname(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->localAddress = addr;
		}
	}

	return this->localAddress.getPort();
}

std::string SocketDescriptor::getLocalIPAddress() {
	if (!this->localAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getsockname(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->localAddress = addr;
		}
	}

	return this->localAddress.getIPAddress();
}

SocketAddress SocketDescriptor::getLocalSocketAddress() {
	if (!this->localAddress.isValid()) {
		struct sockaddr_storage addr;
		unsigned int addrLen = sizeof(addr);

		if ((::getsockname(this->socketDescriptor, (struct sockaddr*) &addr, &addrLen)) == 0) {
			this->localAddress = addr;
		}
	}

	return this->localAddress;
}

