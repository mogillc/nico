/*
 * HexapodPayload.cpp
 *
 *  Created on: Apr 29, 2016
 *      Author: adrian
 */

#include <mogi/remoteapp/HexapodPayload.h>

using namespace Mogi::RemoteApp;

unsigned int HexapodPayload::getDataSize() {
	return jsonString.size() + strLenBytes;
}

bool HexapodPayload::packBuffer(unsigned char* buffer, unsigned int size) {
	unsigned int len = jsonString.size();

	if (len + strLenBytes > size) {
		return false;
	}

	Network::packInteger16bit(buffer, len);
	buffer += 2;
	memcpy(buffer, jsonString.c_str(), len);

	return true;
}

bool HexapodPayload::unpackBuffer(const unsigned char* buffer, unsigned int size) {
	int count;
	unsigned int len;

	if (size < strLenBytes) {
		return false;
	}

	len = Network::unpackUnsignedInteger16bit(buffer);
	buffer += strLenBytes;

	if (len > maxStrLen) count = maxStrLen;
	else count = len;

	if (count > 0) {
		jsonString.assign(buffer, buffer + count);
	}
	else {
		jsonString.clear();
	}

	return true;
}
