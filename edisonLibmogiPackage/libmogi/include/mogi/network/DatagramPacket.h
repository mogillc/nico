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

#ifndef INCLUDE_MOGI_NETWORK_DATAGRAMPACKET_H_
#define INCLUDE_MOGI_NETWORK_DATAGRAMPACKET_H_

#include "mogi/network/SocketAddress.h"

namespace Mogi {
	namespace Network {

		/*! @class DatagramPacket
		 * \brief Simple class that represents a datagram packet with a destination.
		 * This is not too useful after the most recent changes to the library. Will probably be removed.
		 */
		class DatagramPacket {
		public:

			/*! \brief Constructs a DatagramPacket whose payload corresponds to the specified buffer.
			 * @param buffer The pre-allocated buffer.
			 * @param length The buffer's length.
			 */
			DatagramPacket(void* buffer, unsigned int length): packetBuffer(buffer), bufferLength(length), packetAddress() {

			}

			/*! \brief Constructs a DatagramPacket whose payload corresponds to the specified buffer.
			 * @param buffer The pre-allocated buffer.
			 * @param length The buffer's length
			 * @param address The socket address (port + IP address) of a remote host to/from which data will be sent/received.
			 */
			DatagramPacket(void* buffer, unsigned int length, const SocketAddress& address): packetBuffer(buffer), bufferLength(length), packetAddress(address) {

			}

			/*!
			 * \brief Returns the socket address (reference) of the intended remote host.
			 * @return The remote host's address.
			 */
			SocketAddress& getSocketAddress() { return this->packetAddress; }

			/*!
			 * Returns the socket address of the intended remote host.
			 * @return The remote host's address.
			 */
			const SocketAddress& getSocketAddress() const {return this->packetAddress;}

			/*! \brief Sets the socket address of this packet's remote host.
			 * @param address The remote host's address.
			 */
			void setSocketAddress(const SocketAddress& address) {this->packetAddress = address;}

			/*! \brief Returns the packet's data buffer.
			 * @return A pointed to the buffer's first element.
			 */
			void* getPacketBuffer() const {return this->packetBuffer;}

			/*! \brief Sets this packet's buffer.
			 * @param buffer A pointer to the buffer's first element.
			 */
			void setPacketBuffer(void* buffer) {this->packetBuffer = buffer;}

			/*! \brief Returns the size of the packet's buffer.
			 * @return The size of the packet's buffer.
			 */
			unsigned int getBufferLength() const {return this->bufferLength;}

			/*! \brief Sets the size of this packet's buffer.
			 * @param length The length/size of this packet's buffer.
			 */
			void setBufferLength(unsigned int length) {this->bufferLength = length;}

		private:
			void* packetBuffer; // Pointer to buffer (not owned by this object)
			unsigned int bufferLength; // Buffer's length
			SocketAddress packetAddress; // Destination or source address
		};

	}
}
#endif /* INCLUDE_MOGI_NETWORK_DATAGRAMPACKET_H_ */
