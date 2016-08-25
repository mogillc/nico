/*
 * ApplicationInterface.h
 *
 *  Created on: Apr 25, 2016
 *      Author: adrian
 */

#ifndef INCLUDE_MOGI_REMOTEAPP_APPLICATIONSERVER_H_
#define INCLUDE_MOGI_REMOTEAPP_APPLICATIONSERVER_H_

#include <string>
#include "mogi/network/Connection.h"
#include "mogi/network/ServerMulticast.h"

namespace Mogi {
	namespace RemoteApp {

		/*! @class ApplicationServer
		 * \brief Thread that continuously accepts TCP client connections. Data from
		 * these clients is processed in a separate thread by ApplicationCommunicator.
		 */
		class ApplicationServer: public Network::ConnectionManager {
		public:
			ApplicationServer();
			ApplicationServer(unsigned short int port, int maxConnections);
			~ApplicationServer();

			void setPort(unsigned int port);
			void onClientAccepted(Network::TCPSocket& client);
			void onClientSelected(Network::TCPSocket& client);
			void onClientRemoved(Network::TCPSocket& client);



		private:
			static const unsigned short int DEFAULT_SERVER_PORT = 8888;
			static const int DEFAULT_MAX_CONNECTIONS = 10;
			static const unsigned short int DEFAULT_MULTICAST_PORT = 2353;
			static const std::string DEFAULT_MULTICAST_GROUP;
			Network::ServerMulticast multicastThread;

			void entryAction();
			void exitAction();
		};
	}
}



#endif /* INCLUDE_MOGI_REMOTEAPP_APPLICATIONSERVER_H_ */
