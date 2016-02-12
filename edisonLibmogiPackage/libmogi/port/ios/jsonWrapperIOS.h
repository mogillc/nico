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

#ifndef MOGI_JSONWRAPPERIOS_H
#define MOGI_JSONWRAPPERIOS_H

#include <string>
#include <vector>

namespace _JsonWrapperIOS {

	void* getNewJsonValue();
	void deleteJsonValue( void** value );

	int parseJson( void** value, const std::string& jsonString);

	bool isBoolValue(void* value);
	bool isIntValue(void* value);
	bool isDoubleValue(void* value);
	bool isStringValue(void* value);
	bool isArrayValue(void* value);
	bool isObjectValue(void* value);

	void getValueFromIndex(void** storage, void** value, const int& index, void** parentValue);
	void getValueFromKey(void** storage, void** value, const std::string& key, void** parentValue);

	std::vector<std::string> getKeys(void* value);

	bool getBoolValue(const void* value);
	int getIntValue(const void* value);
	double getDoubleValue(const void* value);
	std::string getStringValue(const void* value);
	int getArraySize(const void* value);

	void setBoolValue(void** value, const bool& val, void** parentValue);
	void setIntValue(void** value, const int& val, void** parentValue);
	void setDoubleValue(void** value, const double& val, void** parentValue);
	void setStringValue(void** value, const std::string& val, void** parentValue);
	void setValueValue(void** value, void *const * val, void** parentValue);

	std::string createJsonString(void* value);
}

#endif
