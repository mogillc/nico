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
 * DataSerializer.cpp
 *
 *  Created on: Mar 21, 2016
 *      Author: adrian
 */

#include "mogi/network/DataSerializer.h"
#include <stdarg.h>
#include <cstring>
#include <iostream>

namespace Mogi {
	namespace Network {

		void packInteger(unsigned char* buf, void* data, int size) {
			switch (size) {
			case 1:
				packInteger8bit(buf,*static_cast<unsigned char*>(data));
				break;
			case 2:
				packInteger16bit(buf,*static_cast<unsigned int*>(data));
				break;
			case 4:
				packInteger32bit(buf,*static_cast<unsigned long int*>(data));
				break;
			case 8:
				packInteger64bit(buf,*static_cast<unsigned long long int*>(data));
				break;
			default:
				std::cout << "DataTypeSerializer::packInteger - Data type of size " << size << " bytes is not supported"<< std::endl;
			}
		}

		void packFloat(unsigned char* buf, void* data, int size) {
			switch (size) {
			case 4:
				pack754_32(buf, *static_cast<double*>(data));
				break;
			case 8:
				pack754_64(buf, *static_cast<double*>(data));
				break;
			default:
				std::cout << "DataTypeSerializer::packFloat - Data type of size " << size << " bytes is not supported"<< std::endl;
			}
		}

		void packCString(unsigned char* buf, const char* data, int size) {
			memcpy(buf, data, size);
		}

		long long int unpackInteger(const unsigned char* buf, int size) {
			switch (size)
			{
			case 1:
				return unpackInteger8bit(buf);
				break;
			case 2:
				return unpackInteger16bit(buf);
				break;
			case 4:
				return unpackInteger32bit(buf);
				break;
			case 8:
				return unpackInteger64bit(buf);
				break;
			default:
				std::cout<<"DataTypeSerializer::unpacki - Data type of size "<< size <<" bytes is not supported"<<std::endl;
				return 0;
			}
		}

		unsigned long long int unpackUnsignedInteger(const unsigned char* buf, int size) {
			switch (size)
			{
			case 1:
				return unpackUnsignedInteger8bit(buf);
				break;
			case 2:
				return unpackUnsignedInteger16bit(buf);
				break;
			case 4:
				return unpackUnsignedInteger32bit(buf);
				break;
			case 8:
				return unpackUnsignedInteger64bit(buf);
				break;
			default:
				std::cout<<"DataTypeSerializer::unpacki - Data type of size "<< size <<" bytes is not supported"<<std::endl;
				return 0;
			}
		}

		double unpackFloat(const unsigned char* buf, int size) {
			double result = 0;
			switch (size) {
			case 4:
				result = unpack754_32(buf);
				break;
			case 8:
				result = unpack754_64(buf);
				break;
			default:
				result = 0;
			}

			return result;
		}

		void unpackCString(const unsigned char* buf, char* data, int stringLength) {
			memcpy(data, buf, stringLength);
			data[stringLength] = '\0';
		}

		/*
		** pack() -- store data dictated by the format string in the buffer
		**
		**   bits |signed   unsigned   float   string
		**   -----+----------------------------------
		**      8 |   c        C
		**     16 |   h        H
		**     32 |   l        L         f
		**     64 |   q        Q         d
		**      - |                               s
		**
		**  (16-bit unsigned length is automatically prepended to strings)
		*/

		unsigned int pack(unsigned char *buf, char *format, ...) {
		    va_list ap;

		    signed char c;              // 8-bit
		    unsigned char C;

		    int h;                      // 16-bit
		    unsigned int H;

		    long int l;                 // 32-bit
		    unsigned long int L;

		    long long int q;            // 64-bit
		    unsigned long long int Q;

		    float f;					// floats
		    double d;
		    unsigned long long int fhold;

		    char *s;                    // strings
		    unsigned int len;

		    unsigned int size = 0;

		    va_start(ap, format);

		    for(; *format != '\0'; format++) {
		        switch(*format) {
		        case 'c': // 8-bit
		            size += 1;
		            c = (signed char)va_arg(ap, int); // promoted
		            packInteger8bit(buf, c);
		            buf++;
		            break;

		        case 'C': // 8-bit unsigned
		            size += 1;
		            C = (unsigned char)va_arg(ap, unsigned int); // promoted
		            packInteger8bit(buf, C);
		            buf++;
		            break;

		        case 'h': // 16-bit
		            size += 2;
		            h = va_arg(ap, int);
		            packInteger16bit(buf, h);
		            buf += 2;
		            break;

		        case 'H': // 16-bit unsigned
		            size += 2;
		            H = va_arg(ap, unsigned int);
		            packInteger16bit(buf, H);
		            buf += 2;
		            break;

		        case 'l': // 32-bit
		            size += 4;
		            l = va_arg(ap, long int);
		            packInteger32bit(buf, l);
		            buf += 4;
		            break;

		        case 'L': // 32-bit unsigned
		            size += 4;
		            L = va_arg(ap, unsigned long int);
		            packInteger32bit(buf, L);
		            buf += 4;
		            break;

		        case 'q': // 64-bit
		            size += 8;
		            q = va_arg(ap, long long int);
		            packInteger64bit(buf, q);
		            buf += 8;
		            break;

		        case 'Q': // 64-bit unsigned
		            size += 8;
		            Q = va_arg(ap, unsigned long long int);
		            packInteger64bit(buf, Q);
		            buf += 8;
		            break;

		        case 'f': // float-32
		            size += 4;
		            f = va_arg(ap, double);
		            pack754_32(buf, f);
		            buf += 4;
		            break;

		        case 'd': // float-64
		            size += 8;
		            d = va_arg(ap, double);
		            pack754_64(buf, d);
		            buf += 8;
		            break;

		        case 's': // string
		            s = va_arg(ap, char*);
		            len = strlen(s);
		            size += len + 2;
		            packInteger16bit(buf, len);
		            buf += 2;
		            memcpy(buf, s, len);
		            buf += len;
		            break;
		        }
		    }

		    va_end(ap);

		    return size;
		}

		/*
		** unpack() -- unpack data dictated by the format string into the buffer
		**
		**   bits |signed   unsigned   float   string
		**   -----+----------------------------------
		**      8 |   c        C
		**     16 |   h        H
		**     32 |   l        L         f
		**     64 |   q        Q         d
		**      - |                               s
		**
		**  (string is extracted based on its stored length, but 's' can be
		**  prepended with a max length)
		*/
		void unpack(const unsigned char *buf, char *format, ...)
		{
		    va_list ap;

		    signed char *c;              // 8-bit
		    unsigned char *C;

		    int *h;                      // 16-bit
		    unsigned int *H;

		    long int *l;                 // 32-bit
		    unsigned long int *L;

		    long long int *q;            // 64-bit
		    unsigned long long int *Q;

		    float *f;					// floats
		    double *d;

		    char *s;
		    unsigned int len, maxstrlen=0, count;

		    va_start(ap, format);

		    for(; *format != '\0'; format++) {
		        switch(*format) {
		        case 'c': // 8-bit
		            c = va_arg(ap, signed char*);
		            *c = unpackInteger8bit(buf);
		            buf++;
		            break;

		        case 'C': // 8-bit unsigned
		            C = va_arg(ap, unsigned char*);
		            *C = unpackUnsignedInteger8bit(buf);
		            buf++;
		            break;

		        case 'h': // 16-bit
		            h = va_arg(ap, int*);
		            *h = unpackInteger16bit(buf);
		            buf += 2;
		            break;

		        case 'H': // 16-bit unsigned
		            H = va_arg(ap, unsigned int*);
		            *H = unpackUnsignedInteger16bit(buf);
		            buf += 2;
		            break;

		        case 'l': // 32-bit
		            l = va_arg(ap, long int*);
		            *l = unpackInteger32bit(buf);
		            buf += 4;
		            break;

		        case 'L': // 32-bit unsigned
		            L = va_arg(ap, unsigned long int*);
		            *L = unpackUnsignedInteger32bit(buf);
		            buf += 4;
		            break;

		        case 'q': // 64-bit
		            q = va_arg(ap, long long int*);
		            *q = unpackInteger64bit(buf);
		            buf += 8;
		            break;

		        case 'Q': // 64-bit unsigned
		            Q = va_arg(ap, unsigned long long int*);
		            *Q = unpackUnsignedInteger64bit(buf);
		            buf += 8;
		            break;

		        case 'f': // float-32
		            f = va_arg(ap, float*);
		            *f = unpack754_32(buf);
		            buf += 4;
		            break;

		        case 'd': // float-64
		            d = va_arg(ap, double*);
		            *d = unpack754_64(buf);
		            buf += 8;
		            break;

		        case 's': // string
		            s = va_arg(ap, char*);
		            len = unpackUnsignedInteger16bit(buf);
		            buf += 2;
		            if (maxstrlen > 0 && len > maxstrlen) count = maxstrlen - 1;
		            else count = len;
		            memcpy(s, buf, count);
		            s[count] = '\0';
		            buf += len;
		            break;

		        default:
		            if (isdigit(*format)) { // track max str len
		                maxstrlen = maxstrlen * 10 + (*format-'0');
		                //std::cout << "track maxstrlen = " << maxstrlen << std::endl;
		            }
		        }

		        if (!isdigit(*format)) {
		        	maxstrlen = 0;
		        	//std::cout << "reset maxstrlen = " << maxstrlen << std::endl;
		        }
		    }

		    va_end(ap);
		}

		/*
		** packi8() -- store a 8-bit int into a char buffer (like htons())
		*/
		void packInteger8bit(unsigned char *buf, unsigned char i) {
			buf[0] = i;
		}

		/*
		** packi16() -- store a 16-bit int into a char buffer (like htons())
		*/
		void packInteger16bit(unsigned char *buf, unsigned int i) {
		    buf[0] = i>>8; buf[1] = i;
		}


		/*
		** packi32() -- store a 32-bit int into a char buffer (like htonl())
		*/
		void packInteger32bit(unsigned char *buf, unsigned long int i) {
			/* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
			buf[0] = i >> 24;
			buf[1] = i >> 16;
			buf[2] = i >> 8;
			buf[3] = i;
		}

		/*
		** packi64() -- store a 64-bit int into a char buffer (like htonl())
		*/
		void packInteger64bit(unsigned char *buf, unsigned long long int i) {
			/* Write big-endian long int value into buffer */
			buf[0] = i >> 56;
			buf[1] = i >> 48;
			buf[2] = i >> 40;
			buf[3] = i >> 32;
			buf[4] = i >> 24;
			buf[5] = i >> 16;
			buf[6] = i >> 8;
			buf[7] = i;
		}

		/*
		** unpacki8() -- unpack a 8-bit int from a char buffer (like ntohs())
		*/
		char unpackInteger8bit(const unsigned char *buf) {
			char i;
		    unsigned char temp = (buf[0]);

		    // Check sign
		    if (temp <= 0x7fu) {
		    	i = temp;
		    }
		    else {
		    	i = -1 - (unsigned char)(0xff - temp);
		    }

		    return i;
		}

		unsigned char unpackUnsignedInteger8bit(const unsigned char *buf) {
			return buf[0];
		}

		/*
		** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
		*/
		int unpackInteger16bit(const unsigned char *buf) {
			int i;
		    unsigned int temp = ((unsigned int)buf[0]<<8) | buf[1];


		    // Check sign
		    if (temp <= 0x7fffu) {
		    	i = temp;
		    }
		    else {
		    	i = -1 - (unsigned char)(0xffffu - temp);
		    }

		    return i;
		}

		/*
		** unpacki16() -- unpack a 16-bit int from a char buffer (like ntohs())
		*/
		unsigned int unpackUnsignedInteger16bit(const unsigned char *buf) {
		    return ((unsigned int)buf[0]<<8) | buf[1];
		}

		/*
		** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
		*/
		long int unpackInteger32bit(const unsigned char *buf) {
			long int i;
		    unsigned long int temp = ((unsigned long int)buf[0]<<24) | ((unsigned long int)buf[1]<<16) | ((unsigned long int)buf[2]<<8) | buf[3];

		    // Check sign
		    if (temp <= 0x7fffffffu) {
		    	i = temp;
		    }
		    else {
		    	i = -1 - (unsigned char)(0xffffffffu - temp);
		    }

		    return i;
		}

		/*
		** unpacki32() -- unpack a 32-bit int from a char buffer (like ntohl())
		*/
		unsigned long int unpackUnsignedInteger32bit(const unsigned char *buf) {
		    return ((unsigned long int)buf[0]<<24) | ((unsigned long int)buf[1]<<16) | ((unsigned long int)buf[2]<<8) | buf[3];
		}

		/*
		** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
		*/
		long long int unpackInteger64bit(const unsigned char *buf) {
			long long int i;
			unsigned long long int temp = ((unsigned long long int)buf[0]<<56) |
											((unsigned long long int)buf[1]<<48) |
											((unsigned long long int)buf[2]<<40) |
											((unsigned long long int)buf[3]<<32) |
											((unsigned long long int)buf[4]<<24) |
											((unsigned long long int)buf[5]<<16) |
											((unsigned long long int)buf[6]<<8) |
											((unsigned long long int)buf[7]);

		    // Check sign
		    if (temp <= 0x7fffffffffffffffu) {
		    	i = temp;
		    }
		    else {
		    	i = -1 - (unsigned char)(0xffffffffffffffffu - temp);
		    }

		    return i;
		}

		/*
		** unpacki64() -- unpack a 64-bit int from a char buffer (like ntohl())
		*/
		unsigned long long int unpackUnsignedInteger64bit(const unsigned char *buf) {
			return ((unsigned long long int)buf[0]<<56) |
											((unsigned long long int)buf[1]<<48) |
											((unsigned long long int)buf[2]<<40) |
											((unsigned long long int)buf[3]<<32) |
											((unsigned long long int)buf[4]<<24) |
											((unsigned long long int)buf[5]<<16) |
											((unsigned long long int)buf[6]<<8) |
											((unsigned long long int)buf[7]);
		}

		void packFloat754(unsigned char *buf, long double f, unsigned bits, unsigned expbits) {
			unsigned long long int result;
		    long double fnorm;
		    int shift;
		    long long sign, exp, significand;
		    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

		    if (f == 0.0) return; // get this special case out of the way

		    // check sign and begin normalization
		    if (f < 0) { sign = 1; fnorm = -f; }
		    else { sign = 0; fnorm = f; }

		    // get the normalized form of f and track the exponent
		    shift = 0;
		    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
		    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
		    fnorm = fnorm - 1.0;

		    // calculate the binary form (non-float) of the significand data
		    significand = fnorm * ((1LL<<significandbits) + 0.5f);

		    // get the biased exponent
		    exp = shift + ((1<<(expbits-1)) - 1); // shift + bia s

		    // return the final answer
		    result = (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;

		    switch (bits) {
		    case 16:
		    	packInteger16bit(buf,result);
		    	break;
		    case 32:
		    	packInteger32bit(buf,result);
		    	break;
		    case 64:
		    default:
		    	packInteger64bit(buf,result);
		    	break;
		    }
		}

		long double unpackFloat754(const unsigned char *buf, /*long double &f,*/ unsigned bits, unsigned expbits) {
		    long double result;
		    long long shift;
		    unsigned bias;
		    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

		    unsigned long long int i = 0;

		    switch (bits) {
		    case 16:
		    	i = unpackUnsignedInteger16bit(buf);
		    	break;
		    case 32:
		    	i = unpackUnsignedInteger32bit(buf);
		    	break;
		    case 64:
		    default:
		    	i = unpackUnsignedInteger64bit(buf);
		    	break;
		    }

		    if (i == 0){ return 0.0;}

		    // pull the significand
		    result = (i&((1LL<<significandbits)-1)); // mask
		    result /= (1LL<<significandbits); // convert back to float
		    result += 1.0f; // add the one back on

		    // deal with the exponent
		    bias = (1<<(expbits-1)) - 1;
		    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
		    while(shift > 0) { result *= 2.0; shift--; }
		    while(shift < 0) { result /= 2.0; shift++; }

		    // sign it
		    result *= (i>>(bits-1))&1? -1.0: 1.0;
		    //f = result;
		    return result;

		    //return p;
		}

	}
}
