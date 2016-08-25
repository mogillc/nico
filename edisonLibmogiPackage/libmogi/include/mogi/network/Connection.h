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

#ifndef INCLUDE_MOGI_NETWORK_CONNECTION_H_
#define INCLUDE_MOGI_NETWORK_CONNECTION_H_

#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include "mogi/thread.h"
#include "mogi/network/TCPServerSocket.h"
#include "mogi/network/UDPSocket.h"

namespace Mogi {
	namespace Network {

		/*! @class ConnectionManager
		 *
		 * \brief A socket server that manages all incoming connections.
		 * The socket descriptors for pending connections can be retrieved via selectClient().
		 *
		 * Based on the great and powerful Dr. Xiao's mogid implementation. Like a boss
		 */
		class ConnectionManager: public Thread {
		public:
			/*! \brief Constructs a ConnectionManager thread that listens for incoming TCP connections at the specified port.
			 * @param localPort The local port on which to listen.
			 * @param maxConnections The maximum number of pending connections to queue.
			 * @param domain The domain (IPv4 or IPv6) of the TCP connection.
			 */
			ConnectionManager(unsigned short int localPort, unsigned int maxConnections, SocketDescriptor::Domain domain = SocketDescriptor::INET);

			/*! \brief This little fella here is called before the ConnectionManager is destroyed into to a billion pieces. RIP and praise the sun.
			 *
			 */
			virtual ~ConnectionManager();

			/*! \brief Starts the thread.
			 * @return An integer less than zero if an error occurred while starting the thread.
			 */
			int start();

			/*! \brief Called to notify subclasses whenever the ConnectionManager accepts a new client.
			 * This function is meant to be overridden by ConnectionManger subclasses interested in this event.
			 * @param client The accepted client's socket.
			 */
			virtual void onClientAccepted(TCPSocket& client) {}

			/*! \brief Called just before selectClient() returns with a valid client.
			 * This function is meant to be overridden by ConnectionManger subclasses interested in this event.
			 * @param client The selected client's socket.
			 */
			virtual void onClientSelected(TCPSocket& client) {}

			/*! \brief Called to notify subclasses whenever the ConnectionManager removes a client from its queue.
			 * This function is meant to be overridden by ConnectionManger subclasses interested in this event.
			 * @param client The removed client's socket.
			 */
			virtual void onClientRemoved(TCPSocket& client) {}

			/*!
			 * \brief get the fd of next available connected client
			 */
			TCPSocket* selectClient();

			/*!
			 * \brief remove client fd from the fd_set
			 * usually called when client is closed by themselves
			 */
			void removeClient(TCPSocket& sock);

		protected:

			/*!
			 * \brief The local port.
			 */
			unsigned int port;

			/*!
			 * \brief Overrides Mogi::Thread::doAction(). This is the thread's "loop" task
			 * responsible for accepting new TCP clients.
			 */
			void doAction();

			/*!
			 * \brief Overrides Mogi::Thread::exitAction(). Cleans up.
			 */
			virtual void exitAction();

		private:
			/*!
			 * \brief TCP server socket.
			 */
			TCPServerSocket* sock_fd;

			/*!
			 * \brief TCP server socket's domain (IPv4 or IPv6)
			 */
			SocketDescriptor::Domain domain;

			/*!
			 * \brief Set of pending TCP clients
			 */
			fd_set fdset;

			/*!
			 * \brief Tracks the largest socket descriptor value for pending TCP clients.
			 */
			int max_fd;

			/*!
			 * \brief Tracks the last selected TCP client. Used for round-robin scheduling.
			 */
			int previousFdIndex;

			/*!
			 * \brief Vector of active TCP client sockets.
			 */
			std::vector<TCPSocket*> activeFds;

			/*!
			 * \brief Maximum number of pending TCP clients (used in server socket's listen() call).
			 */
			int maxConnections;

			/*!
			 * \brief Default blocking policy for socket reading/writing/accepting ...
			 */
			static const bool defaultBlocking = false;

			/*! \brief Used to make the thread sleep for a specified amount of time.
			 * @param msTime The time duration in milliseconds.
			 */
			void snooze(uint msTime);

			/*!
			 * \brief Cleans up heap-allocated memory.
			 */
			void releaseMemory();

			/*!
			 * \brief Copy constructor is disabled.
			 */
			ConnectionManager(const ConnectionManager& other);

			/*!
			 * \brief Assignment operator is disabled.
			 */
			ConnectionManager& operator=(const ConnectionManager& other);
		};

		/*!
		 @class ConnectionHandler
		 \brief A thread that process/consume message from conneted fds from ConnectionManager.
		*
		* Adapted from Xiao's mogid implementation
		*/
		class ConnectionHandler : public Thread
		{
		public:

			/*! \brief Constructs a ConnectionHandler thread that can process incoming data from the specified ConnectionManager.
			 * @param manager The ConnectionManager supplying this thread with incoming data from connected clients.
			 */
			ConnectionHandler(ConnectionManager& manager):manager(manager){}

			/*!
			 * Destructor. RIP
			 */
			virtual ~ConnectionHandler(){stop();}

		protected:
			/*!
			 * \brief process connection from a new client
			 * @param fd The client socket descriptor.
			 * @param data The data received from the client.
			 * @param size The size of the data received from the client.
			 * @param sourceAddress The client's network address.
			 */
			virtual void process(TCPSocket& fd, const unsigned char* data, unsigned int size, const SocketAddress& sourceAddress) = 0;

			/*!
			 * \brief Overrides Mogi::Thread::exitAction() for cleanup purposes.
			 */
			virtual void exitAction() {}

		private:
			ConnectionManager& manager; // The manager thread accepting TCP clients
			static const int buffMaxLen = 2000; // The receive buffer's size
			unsigned char buff[buffMaxLen]; // The receive buffer

			/*!
			 * \brief Overrides Mogi::Thread::doAction() to select pending TCP clients,
			 * receive their data, and pass the received data to the process() function.
			 */
			void doAction();
		};

	}
}



#endif /* INCLUDE_MOGI_NETWORK_CONNECTION_H_ */
