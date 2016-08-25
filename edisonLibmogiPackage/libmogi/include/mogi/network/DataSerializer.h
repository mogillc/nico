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

#ifndef INCLUDE_MOGI_NETWORK_DATASERIALIZER_H_
#define INCLUDE_MOGI_NETWORK_DATASERIALIZER_H_

#include <iostream>
//#include <sys/types.h>

#define pack754_16(i, f) (packFloat754((i), (f), 16, 5))
#define pack754_32(i, f) (packFloat754((i), (f), 32, 8))
#define pack754_64(i, f) (packFloat754((i), (f), 64, 11))
#define unpack754_16(i) (unpackFloat754((i), 16, 5))
#define unpack754_32(i) (unpackFloat754((i), 32, 8))
#define unpack754_64(i) (unpackFloat754((i), 64, 11))

namespace Mogi {
	namespace Network {

		////////////////////////////////////////////////////////////////////////////////////
		// Some of these functions where adapted and modified from Beej's guide to networking
		// http://beej.us/guide/bgnet/examples/pack2.c
		////////////////////////////////////////////////////////////////////////////////////

		/*!
		 * \brief Packs the data specified by the format string and variadic arguments into the specified buffer.
		 *
		 * Example usage: int bytesPacked = Mogi::Network::pack(myBuffer, "llds", myInt, 32, 3.14, "Some C String");
		 *
		 * @param buf The pre-allocated buffer.
		 * @param format A string indicating the data types of subsequent arguments
		 * @param ... A variadic list of data values to be packed into the provided buffer.
		 * Note that a 16-bit unsigned length is prepended to strings to facilitate unpacking.
		 *
		 *   bits |signed   unsigned   float   string
		 *   -----+----------------------------------
		 *      8 |   c        C
		 *     16 |   h        H         f
		 *     32 |   l        L         d
		 *     64 |   q        Q         g
		 *      - |                               s
		 */
		unsigned int pack(unsigned char *buf, char *format, ...);

		/*!
		 * \brief Unpacks the data specified by the format string into the variadic arguments.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param format A string indicating the data types of subsequent arguments.
		 * @param ... A variadic list of data values to be initialized by data unpacked from the specified buffer.
		 * Note that a 16-bit unsigned length is assumed to be prepended to strings to facilitate unpacking.
		 *
		 *   bits |signed   unsigned   float   string
		 *   -----+----------------------------------
		 *      8 |   c        C
		 *     16 |   h        H         f
		 *     32 |   l        L         d
		 *     64 |   q        Q         g
		 *      - |                               s
		 */
		void unpack(const unsigned char *buf, char *format, ...);

		/*! \brief Packs an integer into the specified buffer.
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param data A pointer to the integer to pack into the buffer.
		 * @param size The size (in bytes) of the integer to pack.
		 */
		void packInteger(unsigned char* buf, void* data, int size);

		/*! \brief Packs a floating-point value into the specified buffer.
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param data A pointer to the float to pack into the buffer.
		 * @param size The size (in bytes) of the float to pack.
		 */
		void packFloat(unsigned char* buf, void* data, int size);

		/*! \brief Packs a C string into the specified buffer.
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param data The C string to pack into the buffer.
		 * @param size The size (in bytes) of the C string to pack.
		 */
		void packCString(unsigned char* buf, const char* data, int size);


		/*! \brief Unpacks an integer from the specified buffer.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param size The size of the buffer.
		 * @return The unpacked integer.
		 */
		long long int unpackInteger(const unsigned char* buf, int size);

		/*! \brief Unpacks an unsigned integer from the specified buffer.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param size The size of the buffer.
		 * @return The unpacked unsigned integer.
		 */
		unsigned long long int unpackUnsignedInteger(const unsigned char* buf, int size);

		/*! \brief Unpacks a floating-point value (IEEE-754 format) from the specified buffer.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param size The size of the buffer.
		 * @return The unpacked floating-point value.
		 */
		double unpackFloat(const unsigned char* buf, int size);

		/*! \brief Unpacks a C-string from the specified buffer.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param data The C-string's char buffer.
		 * @param stringLength The size of the C-string's buffer.
		 */
		void unpackCString(const unsigned char* buf, char* data, int stringLength);

		/*! \brief Packs an 8-bit integer into the specified buffer (like htons()).
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param i The 8-bit integer to pack into the buffer.
		 */
		void packInteger8bit(unsigned char *buf, unsigned char i);

		/*! \brief Packs a 16-bit integer into the specified buffer (like htons()).
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param i The 16-bit integer to pack into the buffer.
		 */
		void packInteger16bit(unsigned char *buf, unsigned int i);

		/*! \brief Packs a 32-bit integer into the specified buffer (like htonl()).
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param i The 32-bit integer to pack into the buffer.
		 */
		void packInteger32bit(unsigned char *buf, unsigned long int i);

		/*! \brief Packs a 64-bit integer into the specified buffer (like htonl()).
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param i The 64-bit integer to pack into the buffer.
		 */
		void packInteger64bit(unsigned char *buf, unsigned long long int i);

		/*! \brief Unpacks a 8-bit integer from the specified buffer (like ntohs())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 8-bit integer unpacked from the buffer.
		 */
		char unpackInteger8bit(const unsigned char *buf);

		/*! \brief Unpacks a 8-bit unsigned integer from the specified buffer (like ntohs())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 8-bit unsigned integer unpacked from the buffer.
		 */
		unsigned char unpackUnsignedInteger8bit(const unsigned char *buf);

		/*! \brief Unpacks a 16-bit integer from the specified buffer (like ntohs())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 16-bit integer unpacked from the buffer.
		 */
		int unpackInteger16bit(const unsigned char *buf);

		/*! \brief Unpacks a 16-bit unsigned integer from the specified buffer (like ntohs())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 16-bit integer unpacked from the buffer.
		 */
		unsigned int unpackUnsignedInteger16bit(const unsigned char *buf);

		/*! \brief Unpacks a 32-bit integer from the specified buffer (like ntohl())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 32-bit integer unpacked from the buffer.
		 */
		long int unpackInteger32bit(const unsigned char *buf);

		/*! \brief Unpacks a 32-bit unsigned integer from the specified buffer (like ntohl())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 32-bit unsigned integer unpacked from the buffer.
		 */
		unsigned long int unpackUnsignedInteger32bit(const unsigned char *buf);


		/*! \brief Unpacks a 64-bit integer from the specified buffer (like ntohl())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 64-bit integer unpacked from the buffer.
		 */
		long long int unpackInteger64bit(const unsigned char *buf);

		/*! \brief Unpacks a 64-bit unsigned integer from the specified buffer (like ntohl())
		 * @param buf The pre-allocated buffer containing packed data.
		 * @return The 64-bit unsigned integer unpacked from the buffer.
		 */
		unsigned long long int unpackUnsignedInteger64bit(const unsigned char *buf);

		/*! \brief Packs a floating-point value (IEEE-754 format) into the specified buffer.
		 * @param buf The pre-allocated buffer into which data is packed.
		 * @param f The floating-point value to pack into the buffer.
		 * @param bits The size (in bits) of the floating-point value. Ex: 32 for most implementations of float
		 * @param expbits The number of bits used to represent the values exponent.Ex: 8 bits for 32 bit value
		 */
		void packFloat754(unsigned char *buf, long double f, unsigned bits, unsigned expbits);

		/*! \brief Unpacks a floating-point value (IEEE-754 format) from the specified buffer.
		 * @param buf The pre-allocated buffer containing packed data.
		 * @param bits The size (in bits) of the floating-point value to unpack. Ex: 32 for most implementations of float.
		 * @return The floating-point value unpacked from the buffer.
		 */
		long double unpackFloat754(const unsigned char *buf, unsigned bits, unsigned expbits);

	}
}



#endif /* INCLUDE_MOGI_NETWORK_DATASERIALIZER_H_ */
