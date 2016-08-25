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
 * testNetworkSockets.cpp
 *
 *  Created on: May 13, 2016
 *      Author: adrian
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include "mogi/network/UDPSocket.h"
#include "mogi/thread.h"

using namespace Mogi::Network;

bool testConstruction(bool verbose);
bool testBind(bool verbose);
bool testSendReceive(bool verbose);

void printt(const char* testString) {
	std::cout << "Testing " << testString << " .........." << std::endl;
}

void printc(const char* checkString) {
	std::cout << "\tChecking " << checkString << " .........." << std::endl;
}

class TXThread: public Mogi::Thread {
public:
	TXThread() {
		this->done = false;
		this->udp = 0;
		this->message = 0;
		this->msgSize = 0;
		this->bytesSent = 0;
	}

	void setDestination(const SocketAddress& destination) {
		lock();
		this->destination = destination;
		unlock();
	}

	void setMessage(const char* msg, unsigned int size) {
		lock();
		this->message = msg;
		this->msgSize = size;
		unlock();
	}

	unsigned int getBytesSent() {
		unsigned int sent = 0;

		lock();
		sent = bytesSent;
		unlock();

		return sent;
	}

	bool isDone() {
		bool d;

		lock();
		d = done;
		unlock();

		return d;
	}

protected:
	UDPSocket* udp;
	SocketAddress destination;
	const char* message;
	unsigned int msgSize;
	unsigned int bytesSent;
	bool done;


	void entryAction() {
		udp = new UDPSocket();
		udp->setNonBlocking(false); // blocking is fine
	}

	void doAction() {
		lock();
		if (udp && message && msgSize > 0) {
			bytesSent = udp->sendTo(message, msgSize, destination);
		}
		unlock();

		if (bytesSent > 0) {
			lock();
			done = true;
			unlock();
			stop();
		}
		else {
			sleep(1);
		}
	}

	void exitAction() {
		if (udp) {
			delete udp;
		}
	}
};

class RXThread: public Mogi::Thread {
public:
	RXThread(unsigned short int localPort) {
		this->done = false;
		this->udp = 0;
		this->localPort = localPort;
		this->buffer = 0;
		this->bufferSize = 0;
		this->bytesReceived = 0;
	}

	SocketAddress getSource() {
		SocketAddress s;

		lock();
		s = this->source;
		unlock();

		return s;
	}

	void setMessageBuffer(char* buf, unsigned int size) {
		lock();
		this->buffer = buf;
		this->bufferSize = size;
		unlock();
	}

	unsigned int getBytesReceived() {
		unsigned int recv = 0;

		lock();
		recv = bytesReceived;
		unlock();

		return recv;
	}

	bool isDone() {
		bool d;

		lock();
		d = done;
		unlock();

		return d;
	}

protected:
	UDPSocket* udp;
	unsigned short int localPort;
	SocketAddress source;
	char* buffer;
	unsigned int bufferSize;
	unsigned int bytesReceived;
	bool done;


	void entryAction() {
		try {
			udp = new UDPSocket(localPort, true);
		} catch(SocketException& e) {
			udp = 0;
			std::cout << e.what() << std::endl;
		}
	}

	void doAction() {
		lock();
		if (udp && buffer && bufferSize > 0) {
			bytesReceived = udp->receiveFrom(buffer, bufferSize, source);
		}
		unlock();

		if (bytesReceived > 0) {
			lock();
			done = true;
			unlock();
			stop();
		}
		else {
			sleep(1);
		}
	}

	void exitAction() {
		if (udp) {
			delete udp;
		}
	}
};

int main (int argc, char** argv) {
	bool allTestsPass = true;

	allTestsPass = testConstruction(true) ? allTestsPass : false;
	allTestsPass = testBind(true) ? allTestsPass : false;
	allTestsPass = testSendReceive(true) ? allTestsPass : false;


	if (!allTestsPass) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


bool testConstruction(bool verbose) {
	bool allTestsPass = true;

	printt("construction");
	printc("raw socket creation");

	UDPSocket* udp = 0;

	try {
		udp = new UDPSocket();
	} catch (SocketException& e) {
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		allTestsPass = false;
	}

	if (udp) {
		if (verbose) std::cout << "\t\tPASSED" << std::endl;
		delete udp;
	}

	return allTestsPass;
}

bool testBind(bool verbose) {
	bool allTestsPass = true;

	if (verbose) printt("bind");

	// Create raw UDP socket
	UDPSocket* udp = 0;
	try {
		udp = new UDPSocket();
	} catch (SocketException& e) {
		allTestsPass = false;
		if (verbose) std::cout << "\t\tfailed to create UDPSocket()" << std::endl;
	}

	// Bind to local port
	if (udp) {
		unsigned short localPort = 8888;

		if (verbose) printc("bind to local port");
		if (!udp->bind(SocketAddress(localPort))) {
			if (verbose) std::cout << "\t\tFAILED" << std::endl;
			allTestsPass = false;
		}
		else {
			if (verbose) std::cout << "\t\tPASSED" << std::endl;
		}

		delete udp;
	}

	return allTestsPass;
}

bool testSendReceive(bool verbose) {
	bool allTestsPass = true;
	if (verbose) printt("send & receive");

	unsigned short int port = 8888;
	std::string hostname = "127.0.0.1";//"localhost";
	std::string loopbackAddress = "127.0.0.1";
	static unsigned int RX_BUFFER_SIZE = 30;
	char rxBuffer[RX_BUFFER_SIZE];
	std::string txMessage = "Praise the sun!"; // Everyone should play dark souls :)
	unsigned int bytesSent = 0, bytesReceived = 0;

	RXThread rxThread(port);
	TXThread txThread;

	// Initialize transmission thread
	txThread.setDestination(SocketAddress(hostname, port));
	txThread.setMessage(txMessage.c_str(), txMessage.size()); // Don't need to send null char

	// Initialize reception thread
	rxThread.setMessageBuffer(rxBuffer, RX_BUFFER_SIZE);

	// Start rxThread
	rxThread.start();

	// Start txThread
	txThread.start();

	int limit = 8;
	int i = 0;
	while ((!txThread.isDone() || !rxThread.isDone()) && i < limit) {
		sleep(1);
		i++;
	}

	txThread.WaitForInternalThreadToExit();
	rxThread.WaitForInternalThreadToExit();

	if (i == limit) {
		if (verbose) std::cout << "\tThreads are taking too long to send/receive!" << std::endl;
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		return false;
	}

	//
	// Check conditions
	//

	// Check tx size
	if (verbose) printc("TX size");
	bytesSent = txThread.getBytesSent();
	if (bytesSent != txMessage.size()) {
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		allTestsPass = false;
	}
	else {
		if (verbose) std::cout << "\t\tPASSED" << std::endl;
	}

	// Check rx size
	if (verbose) printc("RX size");
	bytesReceived = rxThread.getBytesReceived();
	if (bytesReceived != txMessage.size()) {
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		allTestsPass = false;
	}
	else {
		if (verbose) std::cout << "\t\tPASSED" << std::endl;
	}

	// Check rx contents
	if (verbose) printc("RX contents");
	std::string rxString(rxBuffer, rxBuffer + txMessage.size());
	if (rxString.compare(txMessage) != 0) {
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		allTestsPass = false;
	}
	else {
		if (verbose) std::cout << "\t\tPASSED" << std::endl;
	}

	// Check rx source
	if (verbose) printc("RX source");
	std::string source = rxThread.getSource().getIPAddress();
	if (source.compare(loopbackAddress) != 0 && source.compare(hostname) != 0) {
		if (verbose) std::cout << "\t\tFAILED" << std::endl;
		allTestsPass = false;
	}
	else {
		if (verbose) std::cout << "\t\tPASSED" << std::endl;
	}

	return allTestsPass;
}
