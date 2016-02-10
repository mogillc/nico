/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *   Proprietary and confidential.                                            *
 *                                                                            *
 *   Unauthorized copying of this file via any medium is strictly prohibited  *
 *   without the explicit permission of Mogi, LLC.                            *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   http://www.binpress.com/license/view/l/0088eb4b29b2fcff36e42134b0949f93  *
 *                                                                            *
 *****************************************************************************/

#include "json.h"

#ifdef IDENT_C
static const char* const JSONVALUEOBSERVER_C_Id = "$Id$";
#endif

#ifdef LIBJSONCPP_FOUND

#include <json/json.h>
#include <iostream>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace App;

void JsonObjectObserver::update(const Json::Value& newValue) {
	if (newValue.type() != Json::objectValue) {
		std::cerr
				<< "JsonObjectObserver error: key does not represent an object."
				<< std::endl;
		return;
	}
	notifyObservers(newValue);
}

void JsonArrayObserver::update(const Json::Value& newValue) {
	if (newValue.isArray()) {
		for (unsigned int i = 0; i < newValue.size(); i++) {
			JsonValueObserver* jsonValue = jsonArrayValues[i];
			if (jsonValue != NULL) {
				jsonValue->update(newValue[i]);
			}
		}
	} else {
		std::cerr << "JsonArrayObserver error: key does not represent an array."
				<< std::endl;
	}
}

void JsonArrayObserver::addElementObserver(int arrayIndex,
		JsonValueObserver* observer) {
	JsonValueObserver* jsonValue = jsonArrayValues[arrayIndex];
	if (jsonValue == NULL) {
		jsonArrayValues[arrayIndex] = observer;
	}
}

#ifdef _cplusplus
}
#endif

#endif // LIBJSONCPP_FOUND
