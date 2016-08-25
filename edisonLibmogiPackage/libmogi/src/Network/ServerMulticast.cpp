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
 * ServerMulticast.cpp
 *
 *  Created on: May 3, 2016
 *      Author: adrian
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "mogi.h"
#include "mogi/network/ServerMulticast.h"

using namespace Mogi;
using namespace Network;

ServerMulticast::ServerMulticast(unsigned short int serverPort,
		unsigned short int multicastPort, const std::string& multicastGroupAddress):
		serverPort(serverPort), multicastPort(multicastPort), multicastGroupAddress(multicastGroupAddress), udpSocket(0),
		multicastRateSec(DEFAULT_MULTICAST_RATE_SEC) {

}

ServerMulticast::~ServerMulticast() {
	stop();
	if (udpSocket) {
		delete udpSocket;
	}
}

void ServerMulticast::setMulticastRate(int rateInSeconds) {
	lock();
	multicastRateSec = rateInSeconds;
	unlock();
}
unsigned int ServerMulticast::getMulticastRate() const {
	return multicastRateSec;
}

void ServerMulticast::entryAction() {
	// Initialize UDP socket
	udpSocket = new Network::UDPSocket(Network::SocketDescriptor::INET);
	udpSocket->setNonBlocking(false);

	// Initialize multicast address/port
	multicastDestination = Network::SocketAddress(multicastGroupAddress, multicastPort, Network::SocketDescriptor::INET);

	// Initialize multicast message
	if (gethostname(hostname, 64)) {
		sprintf(hostname, "(null)");
	}

	sprintf(message,
			"{\n\t\"hostname\":\"%s\",\n\t\"port\":%d,\n\t\"version\":\"%d.%d.%"
					"d\"\n}", hostname, serverPort,
			getLibraryVersion().major, getLibraryVersion().minor,
			getLibraryVersion().micro);

	std::cout << "Starting multicast at " << multicastRateSec << " second intervals on "
					<< multicastGroupAddress << ":" << multicastPort << std::endl;
}

void ServerMulticast::doAction() {
	int sentCount = udpSocket->sendTo(message, sizeof(message), multicastDestination);

	if (sentCount < 0) {
		perror("sendto");
		stop();
	}

	lock();
	sleep(multicastRateSec);
	unlock();
}

void ServerMulticast::exitAction() {
	delete udpSocket; // closes in destructor
	udpSocket = 0;
}
