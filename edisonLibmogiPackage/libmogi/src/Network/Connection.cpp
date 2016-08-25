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
 * Connection.cpp
 *
 *  Created on: Apr 19, 2016
 *      Author: adrian
 */
#include <iostream>
#include "mogi/network/Connection.h"
using namespace Mogi::Network;

ConnectionManager::ConnectionManager(unsigned short int localPort, unsigned int maxConnections, SocketDescriptor::Domain domain): sock_fd(0), max_fd(0), previousFdIndex(0), domain(domain), maxConnections(maxConnections), port(localPort) {
}


void ConnectionManager::releaseMemory() {
	if (this->sock_fd) {
		delete this->sock_fd;
	}
	int len = activeFds.size();
	for (int i = 0; i < len; ++i) {
		if (activeFds[i]) {
			delete activeFds[i];
		}
	}
}

ConnectionManager::~ConnectionManager() {
	stop();
	this->releaseMemory();
}

int ConnectionManager::start() {
	this->max_fd = 0;

	// Create socket descriptor if necessary
	if (!this->sock_fd || !this->sock_fd->isValid()) {

		this->sock_fd = new TCPServerSocket( // Creates, Binds, and Listens
					this->port,
					this->maxConnections,
					ConnectionManager::defaultBlocking,
					this->domain
		);

	}

	// Check socket validity
	if (!this->sock_fd->isValid()) {
		return -1;
	}

	FD_ZERO(&fdset);

	return Thread::start() ? 0 : -1;
}

void ConnectionManager::doAction() {
	TCPSocket* client;
	if ((client = this->sock_fd->accept()) != 0) {
		client->setNonBlocking(true);
		int clientfd = client->getRaw();

		lock();
		FD_SET(clientfd, &fdset);
		activeFds.push_back(client);
		unlock();
		if (max_fd < clientfd) {
			max_fd = clientfd;
		}
		this->onClientAccepted(*client);
	}

	this->snooze(100);
}

void ConnectionManager::exitAction() {
	// close server socket
	if (this->sock_fd) {
		this->sock_fd->close();
	}
}

TCPSocket* ConnectionManager::selectClient() {
	fd_set localFdSet;
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	uint start;
	uint end;
	uint len = activeFds.size();
	int selectRet;

	lock();
	localFdSet = fdset;
	unlock();

	selectRet = select(max_fd+1,&localFdSet,0,0,&tv);

	if(selectRet <= 0) {
		return 0;
	}

	start = previousFdIndex + 1;
	if(start >= len)
		start = 0;

	if(start > 0)
		end = start - 1;
	else
		end = len - 1;

	for (uint i = start; ; i++){
		if(i >= len)
			i = 0;

		if(FD_ISSET(activeFds[i]->getRaw(),&localFdSet)){
			previousFdIndex = i;
			this->onClientSelected(*activeFds[i]);

			return activeFds[i];
		}

		if(i == end) {
			return 0;
		}
	}

	return 0;
}

void Mogi::Network::ConnectionManager::removeClient(
		TCPSocket& sock) {
	int sockIndex = 0;
	int sockRaw = sock.getRaw();

	lock();
	FD_CLR(sockRaw,&fdset);


	for (sockIndex = 0; sockIndex < activeFds.size(); sockIndex++) {
		if (activeFds[sockIndex]->getRaw() == sockRaw) {
			delete activeFds[sockIndex];
			activeFds.erase(activeFds.begin() + sockIndex);
			break;
		}
	}
	unlock();

	this->onClientRemoved(sock);

}

void Mogi::Network::ConnectionManager::snooze(uint msTime) {
	struct timespec time;

	time.tv_sec = msTime / 1000;
	time.tv_nsec = (msTime % 1000) * 1000000;
	nanosleep(&time, 0);
}


/////////////////////////////////////////////////////////////
/// Connection Handler
////////////////////////////////////////////////////////////

void ConnectionHandler::doAction() {
	//std::cout << "ConnectionHandler::doAction()" << std::endl;
	TCPSocket* sock = manager.selectClient();

	if (!sock || !sock->isValid()) {
		//std::cout << "no client yet ..." << sock.getSocketDescriptor() << std::endl;
	}
	else {
		SocketAddress sourceAddress;
		int readSize = 0;

		readSize = sock->receive(this->buff, buffMaxLen, false);
		sourceAddress = sock->getRemoteSocketAddress();

		if (readSize == 0) {
			std::cout << "client closed" << std::endl;
			manager.removeClient(*sock);
		}
		else if (readSize == -1) {
			std::cout << "recv error" << std::endl;
		}
		else {
			this->process(*sock, this->buff, readSize, sourceAddress);
		}
	}
}


