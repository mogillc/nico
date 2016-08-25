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
 * MulticastClient.cpp
 *
 *  Created on: Apr 30, 2016
 *      Author: adrian
 */

#include <iostream>
#include <mogi/network/ClientMulticastReceiver.h>
#include <string>

using namespace Mogi;
using namespace App;
using namespace Network;

class JsonUShortIntObserver: public JsonValueObserver {
private:
	unsigned short int& refToValue;
	void update( JsonValueInterface& newValue) {
		if (newValue.isInt()) {
			refToValue = (unsigned short)newValue.asInt();
		}
	}

public:
	JsonUShortIntObserver(unsigned short int& valueToChange): refToValue(valueToChange) {
	}
};

class JsonStringObserver: public JsonValueObserver {
private:
	std::string& refToValue;
	void update( JsonValueInterface& newValue) {
		if (newValue.isString()) {
			refToValue = newValue.asString();
		}
	}

public:
	JsonStringObserver(std::string& valueToChange) : refToValue(valueToChange){
	}
};

const std::string ClientMulticastReceiver::SERVER_PORT_KEY = "port";
const std::string ClientMulticastReceiver::SERVER_HOSTNAME_KEY = "hostname";
const std::string ClientMulticastReceiver::SERVER_VERSION_KEY = "version";

ClientMulticastReceiver::ClientMulticastReceiver(unsigned short int listenPort, const std::string& multicastGroupAddress) :
		listenPort(listenPort), multicastGroupAddress(multicastGroupAddress), callback(0), udpSocket(0){

	// Json observer for server's port
	JsonValueObserver* observer = new JsonUShortIntObserver(serverInfo.port);
	jsonSubject.addValueObserver(SERVER_PORT_KEY, observer);
	valueObservers.push_back(observer);

	// Json observer for server's hostname
	observer = new JsonStringObserver(serverInfo.hostname);
	jsonSubject.addValueObserver(SERVER_HOSTNAME_KEY, observer);
	valueObservers.push_back(observer);

	// Json observer for server's libmogi version
	observer = new JsonStringObserver(serverInfo.version);
	jsonSubject.addValueObserver(SERVER_VERSION_KEY, observer);
	valueObservers.push_back(observer);

}

ClientMulticastReceiver::~ClientMulticastReceiver() {
	stop();
	releaseMemory();
}

void ClientMulticastReceiver::releaseMemory() {
	for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			it != valueObservers.end(); it++) {
		delete *it;
	}

	if (udpSocket) {
		delete udpSocket;
	}
}

void ClientMulticastReceiver::registerCallback(ServerInfoCallback callback) {
	this->callback = callback;
}

ClientMulticastReceiver::ServerInfo ClientMulticastReceiver::getServerInfo() {
	lock();
	ServerInfo info = this->serverInfo;
	unlock();

	return info;
}

void ClientMulticastReceiver::entryAction() {
	// Setup socket
	udpSocket = new Network::UDPSocket(listenPort, false);
	udpSocket->joinMulticastGroup(multicastGroupAddress);
}

void ClientMulticastReceiver::doAction() {
	Network::SocketAddress source;
	int recvSize = udpSocket->receiveFrom(rxBuffer, RX_BUFFER_SIZE, source);

	std::string jsonString;

	if (recvSize > 0) {
		jsonString.assign(rxBuffer, rxBuffer + recvSize);

		// Parse json string (using jsonsubject) and remember to lock()
		lock();
		bool parsingSuccessful = jsonSubject.parseJson(jsonString.begin(), jsonString.end());
		unlock();

		this->serverInfo.ipAddress = source.getIPAddress();

		if (parsingSuccessful) {
			// Notify listeners/handlers
			if (callback) {
				callback(this->serverInfo);
			}
		}
		else {
			std::cerr << "MulticastClient::doAction() - Json parsing error for string: " << jsonString << std::endl;
		}
	}

	snooze(SLEEP_TIME_MS);
}

void ClientMulticastReceiver::exitAction() {
	udpSocket->close();
	delete udpSocket;
	udpSocket = 0;
}

void ClientMulticastReceiver::snooze(uint msTime) {
	struct timespec time;

	time.tv_sec = msTime / 1000;
	time.tv_nsec = (msTime % 1000) * 1000000;
	nanosleep(&time, 0);
}
