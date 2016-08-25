/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
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

#include "json.h"

#ifdef IDENT_C
static const char* const JSONVALUEOBSERVER_C_Id = "$Id$";
#endif

#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace App;

void JsonObjectObserver::update( JsonValueInterface& newValue) {
	if (!newValue.isObject()) {
		std::cerr << "JsonObjectObserver error: key does not represent an object." << std::endl;
		return;
	}
	notifyObservers(newValue);
}

void JsonArrayObserver::update( JsonValueInterface& newValue) {
	if (newValue.isArray()) {
		for (unsigned int i = 0; i < newValue.size(); i++) {
			JsonValueObserver* jsonValue = jsonArrayValues[i];
			if (jsonValue != NULL) {
//				JsonValueInterface valueToSend = newValue[i];
				jsonValue->update(newValue[i]);
			}
		}
	} else {
		std::cerr << "JsonArrayObserver error: key does not represent an array." << std::endl;
	}
}

void JsonArrayObserver::addElementObserver(int arrayIndex, JsonValueObserver* observer) {
	JsonValueObserver* jsonValue = jsonArrayValues[arrayIndex];
	if (jsonValue == NULL) {
		jsonArrayValues[arrayIndex] = observer;
	}
}

#ifdef _cplusplus
}
#endif

