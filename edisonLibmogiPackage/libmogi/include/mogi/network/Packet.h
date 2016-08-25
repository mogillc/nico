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

#ifndef INCLUDE_MOGI_NETWORK_PACKET_H_
#define INCLUDE_MOGI_NETWORK_PACKET_H_

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "mogi/network/DataSerializer.h"

namespace Mogi {
	namespace Network {

		/*! \class ErrorDetection
		 * \brief Purely virtual error detection policy class for Packets.
		 */
		class ErrorDetection {
		public:
			virtual ~ErrorDetection() {}

		protected:
			/*! \brief Returns the size (in bytes) of the "check value" field
			 *
			 */
			virtual unsigned int getCheckValueSize() = 0;

			/*! \brief Calculates a check value for an array of data.
			 *
			 * @param data The data array for which to calculate the check value.
			 * @param size The size of the data array.
			 * @return A pointer to the calculated check value
			 */
			virtual const unsigned char* calculateCheckValue(const unsigned char* data, unsigned int size) = 0;
		};

		/*! \class Payload
		 * \brief Purely virtual packet payload class for Packets.
		 */
		class Payload {
		public:
			/*!
			 * \brief The virtual destructor (RIP)
			 */
			virtual ~Payload() {}
		protected:
			/*!
			 * \brief Implementations of this pure-virtual function should return the size (in bytes)
			 * of the payload data.
			 * @return The payload's size in bytes.
			 */
			virtual unsigned int getDataSize() = 0;

			/*!
			 * \brief Implementations of this pure-virtual function should pack the payload into the provided
			 * buffer and return a boolean flag indicating success.
			 * @param buffer The buffer into which data should be packed.
			 * @param size The buffer's size (in bytes), which should be as large as the value returned by getDataSize().
			 * @return True for success or false for failure.
			 */
			virtual bool packBuffer(unsigned char* buffer, unsigned int size) = 0;

			/*!
			 * \brief Implementations of this pure-virtual function should unpack the data from the buffer and
			 * initialize the payload.
			 * @param buffer The buffer containing the packed payload data.
			 * @param size The buffer's size in bytes.
			 * @return True for success or false for failure.
			 */
			virtual bool unpackBuffer(const unsigned char* buffer, unsigned int size) = 0;
		};

		/*! \class ChecksumErrorDetection
		 * \brief Error detection policy class for Packets. Uses a simple checksum value.
		 */
		class ChecksumErrorDetection : public ErrorDetection{
		public:
			/*! \brief Returns the size (in bytes) of the checksum field
			 *
			 */
			unsigned int getCheckValueSize();

			/*! \brief Calculates a checksum value for an array of data.
			 *
			 * @param data The data array for which to calculate the checksum.
			 * @param size The size of the data array.
			 * @return A pointer to the calculated checksum
			 */
			const unsigned char* calculateCheckValue(const unsigned char* data, unsigned int size);

		private:
			static const int numBytes = 4; // Size of checksum in bytes
			unsigned char checksum[numBytes]; // Buffer to store last calculated checksum
		};


		/*! \class Packet
		 * \brief A Packet is a wrapper for a generic payload (i.e., PacketData) that supports
		 * data serialization and de-serialization for network transmission and reception. Packet supports
		 * partial data reception, and ensures data integrity via length and checksum processing. Importantly,
		 * data should be sent and received using the same generic Packet type.
		 *
		 * @tparam PacketPayload A generic type that represents a packet payload. Although not strictly necessary, PacketPayload
		 * could be a class derived from the Payload class to ensure the appropriate functions are implemented.
		 * @tparam ErrorDetectionPolicy A generic type that calculates a checksum or some other "check value" to detect transmission errors.
		 */
		template <class PacketPayload, class ErrorDetectionPolicy = ChecksumErrorDetection>
		class Packet : public PacketPayload, public ErrorDetectionPolicy {
		public:
			/*! @enum RXState
			 * \brief Type represents the possible states occupied by a Packet during reception.
			 */
			enum RXState {
				READY, // Ready to receive new data
				UNKNOWN_LENGTH, // Received some data but not enough (2 bytes) to determine the packet's length
				INSUFFICIENT_DATA, // Have not received all data, but know the packet's length
				RX_SUCCESSFUL, // Successfully received all expected data. Unpacking is now possible.
				RX_ERROR // Received data that failed error detection (e.g., checksum failure).
			};

			/*!
			 * \brief Constructs a Packet
			 */
			Packet(): bufferPackedSize(0), receivePacketLength(0), receiveState(READY) {

			}

			/*! \brief Packs (i.e., serializes) payload data into a buffer, which can be used for transmission.
			 * @return A boolean value indicating success of packing operation.
			 */
			bool packBuffer();

			/*! \brief Unpacks payload data from buffer, assigning new values to payload fields. Typically used after packet reception.
			 * @return A boolean value indicating success of unpacking operation.
			 */
			bool unpackBuffer();

			/*! \brief Returns transmission buffer.
			 * @return The transmission buffer.
			 */
			unsigned char* getTransmitBuffer();

			/*! \brief Returns the size of the transmission buffer.
			 * @return The size of the transmission buffer.
			 */
			int getTransmitBufferSize();

			/*! \brief Returns the current status of packet reception.
			 * @return The status (e.g., READY, UNKNOWN_LENGTH, RX_SUCCESSFUL) of packet reception.
			 */
			RXState getReceiveStatus();

			/*! \brief Attempts to consume received packet data for later unpacking. Supports partial packet reception and checks received packet length and checksum.
			 * @param data The received data to consume.
			 * @param size The size of the data.
			 * @return The amount of data consumed. If not all data is consumed, the data array contains extra data, which the user should process separately.
			 */
			int receiveData(const unsigned char* data, int size);



		private:
			static const int packetLenBytes = 2; // The size of a packet's length field.
			std::vector<unsigned char> buffer; // The buffer containing all raw (i.e., packed) payload and header data.
			int bufferPackedSize; // Current amount (in bytes) of valid data that's packed into the buffer.
								  // Note that this size is likely to differ from buffer.size().
			unsigned int receivePacketLength; // Tracks the current amount of valid data received for unpacking.

			RXState receiveState; // Receive state
		};


		/////////////////////////////////
		// Packet class implementation
		////////////////////////////////

		template <class PacketPayload, class ErrorDetectionPolicy>
		bool Packet<PacketPayload, ErrorDetectionPolicy>::packBuffer() {
			bool success;
			int checkValueBytes = ErrorDetectionPolicy::getCheckValueSize();
			int dataBytes = PacketPayload::getDataSize();

			unsigned int packetLength = packetLenBytes + checkValueBytes + dataBytes;
			int dataOffset = packetLenBytes;
			int checkValueOffset = dataOffset + dataBytes;

			if (this->buffer.size() < packetLength){
				this->buffer.resize(packetLength);
			}

			// Pack data into buffer
			success = PacketPayload::packBuffer(&this->buffer[dataOffset], dataBytes);


			// Calculate and pack check value into buffer
			const unsigned char* checkValue = ErrorDetectionPolicy::calculateCheckValue(&this->buffer[dataOffset], dataBytes);
			memcpy(&buffer[checkValueOffset], checkValue, checkValueBytes);

			// Pack total packet length into buffer
			packInteger16bit(&(buffer[0]), packetLength);

			this->bufferPackedSize = packetLength;
			this->receiveState = READY;

			return success;
		}

		template <class PacketPayload, class ErrorDetectionPolicy>
		bool Packet<PacketPayload, ErrorDetectionPolicy>::unpackBuffer() {
			int checkValueBytes = ErrorDetectionPolicy::getCheckValueSize();
			int dataOffset = packetLenBytes;
			int dataBytes = bufferPackedSize - packetLenBytes - checkValueBytes;

			if (dataBytes <= 0) {
				return false;
			}

			return PacketPayload::unpackBuffer(&buffer[dataOffset], dataBytes);
		}

		template <class PacketPayload, class ErrorDetectionPolicy>
		unsigned char* Packet<PacketPayload, ErrorDetectionPolicy>::getTransmitBuffer() {
			return &this->buffer[0];
		}

		template <class PacketPayload, class ErrorDetectionPolicy>
		int Packet<PacketPayload, ErrorDetectionPolicy>::getTransmitBufferSize() {
			return this->bufferPackedSize;
		}

		template <class PacketPayload, class ErrorDetectionPolicy>
		typename Packet<PacketPayload, ErrorDetectionPolicy>::RXState Packet<PacketPayload, ErrorDetectionPolicy>::getReceiveStatus() {
			return this->receiveState;
		}

		template <class PacketPayload, class ErrorDetectionPolicy>
		int Packet<PacketPayload, ErrorDetectionPolicy>::receiveData(const unsigned char* data, int size) {
			int dataConsumed = 0;

			if (size == 0) {
				return 0;
			}

			if (receiveState == RX_SUCCESSFUL || receiveState == RX_ERROR) {
				receiveState = READY;
			}

			if (receiveState == READY) {
				bufferPackedSize = 0;

				if (size < packetLenBytes) { // Don't even have enough data to get total packet length
					// Copy data
					buffer.assign(data, data + size);
					bufferPackedSize = size;
					dataConsumed = size;

					receiveState = UNKNOWN_LENGTH;
				}
				else {
					// Extract total packet length
					receivePacketLength = unpackInteger16bit(data);

					if (size >= receivePacketLength) { // Have all the data now
						// Copy the rest of the packet
						buffer.assign(data, data + receivePacketLength);
						bufferPackedSize = receivePacketLength;
						dataConsumed = receivePacketLength;

						receiveState = RX_SUCCESSFUL;
					}
					else { // Don't have all data yet
						// Copy what we have
						buffer.assign(data, data + size);
						bufferPackedSize = size;
						dataConsumed = size;

						receiveState = INSUFFICIENT_DATA;
					}
				}
			}
			else if (receiveState == UNKNOWN_LENGTH) {
				int combinedSize = size + bufferPackedSize;
				if (combinedSize < packetLenBytes) { // Still don't know packet's length
					// Append new data to buffer
					buffer.insert(buffer.end(), data, data + size);
					bufferPackedSize += size;
					dataConsumed = size;

					receiveState = UNKNOWN_LENGTH;
				}
				else {
					// Copy enough to get packet length
					buffer.insert(buffer.begin() + bufferPackedSize, data, data + (packetLenBytes - bufferPackedSize));
					dataConsumed = (packetLenBytes - bufferPackedSize);

					// Extract total packet length
					receivePacketLength = unpackInteger16bit(&buffer[0]);
					std::cout << receivePacketLength << std::endl;

					if (combinedSize >= receivePacketLength) { // Have all data now
						// Copy the rest
						buffer.insert(buffer.begin() + packetLenBytes, data + dataConsumed, data + (receivePacketLength - packetLenBytes + dataConsumed));
						bufferPackedSize = receivePacketLength;
						dataConsumed += receivePacketLength - packetLenBytes;

						receiveState = RX_SUCCESSFUL;
					}
					else { // Still need more data
						// Copy what we have
						buffer.insert(buffer.begin() + packetLenBytes, data + dataConsumed, data + size);
						bufferPackedSize += size;
						dataConsumed = size;

						receiveState = INSUFFICIENT_DATA;
					}
				}
			}
			else if (receiveState == INSUFFICIENT_DATA) {
				int combinedSize = size + bufferPackedSize;

				if (combinedSize >= receivePacketLength) { // Have all data now
					// Copy the rest
					buffer.insert(buffer.begin() + bufferPackedSize, data, data + (receivePacketLength - bufferPackedSize));
					dataConsumed = receivePacketLength - bufferPackedSize;
					bufferPackedSize = receivePacketLength;

					receiveState = RX_SUCCESSFUL;
				}
				else { // Still need more data
					// Copy what we have
					buffer.insert(buffer.begin() + bufferPackedSize, data, data + size);
					bufferPackedSize += size;
					dataConsumed = size;

					receiveState = INSUFFICIENT_DATA;
				}
			}

			// Check "checksum"
			if (receiveState == RX_SUCCESSFUL) {
				int checkValueBytes = ErrorDetectionPolicy::getCheckValueSize();
				int dataOffset = packetLenBytes;
				int dataBytes = receivePacketLength - packetLenBytes - checkValueBytes;
				int checkValueOffset = dataOffset + dataBytes;
				const unsigned char* checkValue = ErrorDetectionPolicy::calculateCheckValue(&this->buffer[dataOffset], dataBytes);
				const unsigned char* rxCheckValue = &buffer[checkValueOffset];

				bool equal = true;

				for (int i = 0; i < checkValueBytes; ++i) {
					if (checkValue[i] != rxCheckValue[i]) {
						equal = false;
						break;
					}
				}

				if (!equal) {
					receiveState = RX_ERROR;
				}
			}

			return dataConsumed;
		}



	}
}



#endif /* INCLUDE_MOGI_NETWORK_PACKET_H_ */
