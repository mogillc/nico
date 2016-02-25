/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#ifdef IDENT_C
static const char *const SOCKETCONTROL_C_Id = "$Id$";
#endif

#include "mogi.h"
#include "appcomm.h"
#include "errno.h"
#include "hexapod.h"

#include <arpa/inet.h>
#include <assert.h>
#include <math.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>  // exit()

//#include <regex>

#define EXAMPLE_PORT 2353
#define EXAMPLE_GROUP "239.255.0.1"

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace App;

AppInterface::AppInterface() {
}

AppInterface::~AppInterface() {
	//        close(fd);
	if (terminate == false)
		stop();

	Thread::stop();
}

void AppInterface::setPort(int port) {
	portNumber = port;
}

void set_nonblock(int socket) {
	int flags;
	flags = fcntl(socket, F_GETFL, 0);
	assert(flags != -1);
	fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

int AppInterface::start() {
	//		bufferIndex = 0;
	//		haveBeginningOfProtocol = false;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		// perror("socket error");
		return -1;
	}

	// set_nonblock(fd);

	// int flags = fcntl(fd, F_GETFL, 0);
	// fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	struct sockaddr_in addr;

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(portNumber);

	int yes = 1;
	// char yes='1'; // use this under Solaris

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		// perror("setsockopt");
		::exit(1);
	}

	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
		// perror("bind error");
		return -2;
	}

	if (listen(fd, 5) == -1) {
		// perror("listen error");
		return -3;
	}

	terminate = false;

	Thread::start();

	return 0;
}

int AppInterface::stop() {
	terminate = true;
	close(fd);
	//				pthread_join(hexapodCommunication_thread, NULL);

	// Thread::kill();
	// std::cout << "Socket thread joined!\n";

	return 0;
}

//	int AppInterface::getClient()
//	{
//		return fd;
//	}

void AppInterface::multicastThreadEntry() {
	while (!currentlyOpen) {
		struct sockaddr_in addr;
		size_t addrlen;
		int sock, cnt;
		// struct ip_mreq mreq;
		char message[200];
		char hostname[64];
		if (gethostname(hostname, 64)) {
			sprintf(hostname, "(null)");
		}

		/* set up socket */
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) {
			perror("socket");
			sleep(2);
			continue;
		}
		bzero((char *) &addr, sizeof(addr));
		addr.sin_family = AF_INET;
		// addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
		// inet_aton("192.168.42.1", &addr.sin_addr);
		addr.sin_port = htons(EXAMPLE_PORT);
		addr.sin_addr.s_addr = inet_addr(EXAMPLE_GROUP);
		addrlen = sizeof(addr);
		sprintf(message,
				"{\n\t\"hostname\":\"%s\",\n\t\"port\":%d,\n\t\"version\":\"%d.%d.%"
						"d\"\n}", hostname, portNumber,
				getLibraryVersion().major, getLibraryVersion().minor,
				getLibraryVersion().micro);

		// addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
		std::cout << "Starting multicast at 2 second intervals on "
				<< EXAMPLE_GROUP << ":" << EXAMPLE_PORT << std::endl;
		while (!currentlyOpen) {
			cnt = sendto(sock, message, sizeof(message), 0,
					(struct sockaddr *) &addr, addrlen);
			if (cnt < 0) {
				perror("sendto");
				sleep(2);
				break;
			}
			sleep(2);
		}
	}
}
void *AppInterface::multicastStart(void *This) {
	AppInterface *server = (AppInterface *) This;
	server->multicastThreadEntry();
	return NULL;
}

void AppInterface::exitAction() {
	// std::cerr << "AppInterface::exitAction()" << std::endl;
}

void AppInterface::entryAction() {
	pthread_t _thread;
#define BUFFER_SIZE 1000
	unsigned char buf[BUFFER_SIZE];
	int rc;
	// int bytes;
	// int numberOfTransacitons = 0;
	int client;

	// Wait for a client to connect:
	currentlyOpen = false;
	if (pthread_create(&_thread, NULL, multicastStart, this) != 0) {
		std::cerr << "error starting multicast thread :(" << std::endl;
	}
	while (!terminate) {
		int iResult;
		struct timeval tv;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		tv.tv_sec = (long) 1;
		tv.tv_usec = 0;

		iResult = select(fd + 1, &rfds, (fd_set *) 0, (fd_set *) 0, &tv);
		if (iResult > 0) {
			client = accept(fd, NULL, NULL);
		} else {
			// always here, even if i connect from another application
			client = -1;
		}

		usleep(100000);
		if (client == -1) {
			currentlyOpen = false;

			// perror("accept error");
		} else {
			didConnect();
			currentlyOpen = true;
			std::cout << "Accepted a client!" << std::endl;
			// setClient(client);
			this->client = client;
			// fd = client;
			// pthread_join(_thread, NULL);

			//			set_nonblock(client);

			/*
			 // All this is for making the socket non-blocking:
			 fd_set input;
			 FD_ZERO(&input);
			 FD_SET(client, &input);
			 struct timeval timeout;
			 timeout.tv_sec = 5;
			 timeout.tv_usec = 1000; // timeout is 1ms
			 int n = select(client+1, &input, NULL, NULL, &timeout);

			 if (n == -1) {
			 printf("Something went wrong!\n");
			 //something wrong
			 }
			 if (!FD_ISSET(client, &input))
			 printf("Something went wrong 2!\n");
			 */

			// int flags = fcntl(client, F_GETFL, NULL);
			// fcntl(client, F_SETFL, flags | O_NONBLOCK);
			// rc = recv
			// setsockopt();
			// while ( (rc=read(client,buf,sizeof(buf))) > -1)
			bool stopReading = false;
			while (((rc = recv(client, buf, BUFFER_SIZE - 1, MSG_DONTWAIT)) > -2)
					&& (!terminate) && (!stopReading)) {
				// std::cout << "Got a packet rc = " << rc << std::endl;

				if (rc > 0) {
					// printf("\n");
					buf[rc] = '\0';
					// buffer += (char*)buf;

					// while (buffer.length() > 0) {
					didReceiveData((char *) buf);
					//}

				} else if (rc == 0) {
					stopReading = true;
				} else if ((errno != EWOULDBLOCK) && (errno != ETIMEDOUT)) {
					std::cout << " - errno = " << errno << std::endl;
					stopReading = true;
				} else {
					usleep(1000);
				}
				// int buffSize = sprintf(buf, "Server: Thanks! Total transactions: %d",
				// ++numberOfTransacitons);
				// if( (bytes = write(client, buf, buffSize)) != buffSize)
				//	printf("Could not send a response message...\n");
				// else
				//	printf("Sent back a response of %d bytes\n", bytes);
			}

			close(client);
			pthread_join(_thread, NULL);
			currentlyOpen = false;
			if (pthread_create(&_thread, NULL, multicastStart, this) != 0) {
				std::cerr << "error starting multicast thread :(" << std::endl;
			}

			std::cout << "Exiting the connection loop, rc = " << rc << std::endl;
		}
	}
	//		std::cout << "Socket Thread complete." << std::endl;
}

int AppInterface::sendState(std::string jsonData) {
	int ret = 0;
	// std::string xmitBuffer = generateTransmitBuffer();

	// ret = ftdi_write_data( &ftdic, buffer, numberOfBytesToTransmit);
	ret = write(client, jsonData.c_str(), jsonData.length()); // numberOfBytesToTransmit);
	if (ret != jsonData.length()) {
		std::cerr
				<< "AppInterface: Failed to write() state to iOS device, sent bytes: "
				<< ret << std::endl;
	}

	return ret;
}

#ifdef _cplusplus
}
#endif
