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

#include "mogi/network/Packet.h"


using namespace Mogi::Network;


unsigned int ChecksumErrorDetection::getCheckValueSize() {
	return numBytes;
}

const unsigned char* ChecksumErrorDetection::calculateCheckValue(const unsigned char* data, unsigned int size) {
	  int i;
	  int chk = 0x12345678;

	  for (i = 0; i < size; i++) {
	    chk += ((int)(data[i]) * (i + 1));
	  }

	  packInteger32bit(checksum, chk);
	  return checksum;
}
