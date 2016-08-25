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
 * TCPServerSocket.cpp
 *
 *  Created on: May 3, 2016
 *      Author: adrian
 */

#include "mogi/network/TCPServerSocket.h"

using namespace Mogi::Network;

TCPServerSocket::TCPServerSocket(unsigned short int localPort, unsigned int maxConnections, bool blocking, Domain domain) : SocketDescriptor(domain, STREAM, TCP) {
	SocketAddress lAddress(localPort, domain);
	setReuseAddress(true);
	bind(lAddress);
	listen(maxConnections);
	setNonBlocking(!blocking);
}

TCPServerSocket::TCPServerSocket(const std::string& localAddress, unsigned short int localPort, unsigned int maxConnections, bool blocking, Domain domain): SocketDescriptor(domain, STREAM, TCP) {
	SocketAddress lAddress(localAddress, localPort, domain);
	setReuseAddress(true);
	bind(lAddress);
	listen(maxConnections);
	setNonBlocking(!blocking);
}

void TCPServerSocket::listen(unsigned int maxConnections) const {
	if (::listen(this->socketDescriptor, maxConnections) < 0){
		throw SocketException("Failed to listen");
	}
}

Mogi::Network::TCPSocket* TCPServerSocket::accept() const {
	socklen_t addrSize;
	struct sockaddr_storage theirAddr;
	int sockfd = -1;

	addrSize = sizeof (struct sockaddr_storage);
	sockfd = ::accept(
						this->socketDescriptor,
						(struct sockaddr *)&theirAddr,
						&addrSize);

	if (sockfd < 0) {
		//throw SocketException("Failed to accept a tcp client");
		return 0;
	}

	//socket.setSocketDescriptor(sockfd);
	//socket.setSocketType(TCP_SOCKET);
	//socket.remoteAddress = theirAddr;

	//socket.socketDescriptor = sockfd;
	//socket.socketType = TCP_SOCKET;

	TCPSocket* socket = new TCPSocket(sockfd, getSocketDomain(), SOCKET_CONNECTED);
	socket->remoteAddress = theirAddr;

	return socket;
}
