/*
 * ApplicationInterface.cpp
 *
 *  Created on: Apr 26, 2016
 *      Author: adrian
 */
#include <mogi/remoteapp/ApplicationServer.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mogi.h"

//#define EXAMPLE_PORT 2353
//#define EXAMPLE_GROUP "239.255.0.1"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi::RemoteApp;

const std::string ApplicationServer::DEFAULT_MULTICAST_GROUP = "239.255.0.1";

ApplicationServer::ApplicationServer():
		Network::ConnectionManager(DEFAULT_SERVER_PORT, DEFAULT_MAX_CONNECTIONS, Network::SocketDescriptor::INET),
		multicastThread(DEFAULT_SERVER_PORT, DEFAULT_MULTICAST_PORT, DEFAULT_MULTICAST_GROUP){

}


ApplicationServer::ApplicationServer(unsigned short int port, int maxConnections):
		Network::ConnectionManager(port, maxConnections, Network::SocketDescriptor::INET),
		multicastThread(port, DEFAULT_MULTICAST_PORT, DEFAULT_MULTICAST_GROUP){

}

ApplicationServer::~ApplicationServer() {
}

void ApplicationServer::setPort(unsigned int port) {
	this->port = port;
}

void ApplicationServer::entryAction() {
	lock();
	if (!multicastThread.start()) {
		std::cerr << "Error starting multicast thread :(" << std::endl;
	}
	unlock();
}

void ApplicationServer::exitAction() {

	lock();
	multicastThread.stop();
	multicastThread.WaitForInternalThreadToExit();
	unlock();

	ConnectionManager::exitAction();
}

void ApplicationServer::onClientAccepted(Network::TCPSocket& client) {
	//Network::SocketAddress remoteAddress = client.getRemoteSocketAddress();
}

void ApplicationServer::onClientSelected(Network::TCPSocket& client) {
	lock();
	multicastThread.pause();
	unlock();
}

void ApplicationServer::onClientRemoved(Network::TCPSocket& client) {
	lock();
	multicastThread.resume();
	unlock();
}


#ifdef _cplusplus
}
#endif
