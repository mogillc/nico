/*
 * HexPacket.h
 *
 *  Created on: Apr 29, 2016
 *      Author: adrian
 */

#ifndef INCLUDE_MOGI_REMOTEAPP_HEXAPODPAYLOAD_H_
#define INCLUDE_MOGI_REMOTEAPP_HEXAPODPAYLOAD_H_

#include "mogi/network/DataSerializer.h"
#include "mogi/network/Packet.h"
#include "string.h"
#include <string>

namespace Mogi {
	namespace RemoteApp {

		/*! @class HexapodPayload
		 * A packet payload consisting of a JSON string.
		 */
		class HexapodPayload: public Network::Payload {
		public:
			std::string jsonString; // The payload

		protected:
			unsigned int getDataSize();
			bool packBuffer(unsigned char* buffer, unsigned int size);
			bool unpackBuffer(const unsigned char* buffer, unsigned int size);
		private:
			static const unsigned int strLenBytes = 2;
			static const unsigned int maxStrLen = 400;
		};

		typedef Network::Packet<HexapodPayload> HexapodPacket;

	}
}



#endif /* INCLUDE_MOGI_REMOTEAPP_HEXAPODPAYLOAD_H_ */
