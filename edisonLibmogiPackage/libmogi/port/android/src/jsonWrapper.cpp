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

// Author: Adrian Lizarraga

#include <mogi/port/android/jsonWrapperIOS.h>
#include <mogi/JSONParser.h>
#include <android/log.h>

namespace _JsonWrapperIOS {

	void* getNewJsonValue() {
		return (void*)(new Mogi::JSONValue);
	}

	void deleteJsonValue( void** value ) {
		if (!value) {
			return;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*)(*value);

		if (jsonValue) {
			delete jsonValue;
		}
	}

	int parseJson( void** value, const std::string& jsonString) {
		//__android_log_print(ANDROID_LOG_INFO,"jsonWrapper.cpp","parseJson: %s", jsonString.c_str());



		Mogi::JSONValue* jsonValue = new Mogi::JSONValue;
		Mogi::JSONParser parser;
		int result = parser.parseJSONString(jsonString, *jsonValue);
		
		//__android_log_print(ANDROID_LOG_INFO,"jsonWrapper.cpp","parseJson result: %d", result);



		if (result >= 0) {
			*value = (void*) jsonValue;
			result = 0;
		}	
		else {
			*value = NULL;
			delete jsonValue;
			result = -1;
		}

		return result;
	}

	bool isBoolValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_BOOL);
	}

	bool isIntValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_INT);
	}

	bool isDoubleValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_DOUBLE);
	}

	bool isStringValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_STRING);
	}

	bool isArrayValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_ARRAY);
	}

	bool isConstArrayValue(void* value) { //TODO: What ...
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_ARRAY);
	}

	bool isObjectValue(void* value) {
		if (!value) {
			return false;
		}

		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return (jsonValue->getType() ==  Mogi::JSON_OBJECT);
	}


	void getValueFromIndex(void** storage, void** value, const int& index, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;

		// This is not an array! How the fuck do I convert it?
		if (!isArrayValue(*value)) {

			/*// CASE 1: This json value does not have a parent
			if (!parentJSONValue) {
				//deleteJsonValue(value); // delete it 
				//*value = (void*)(new Mogi::JSONValue(JSON_ARRAY)); // Create new array
				*jsonValue = JSON_ARRAY;
			}
			// CASE 2: Parent is a JSON object
			else if (isObjectValue(*parentValue)) {
				// Try to find value in parent
				
			}*/
			*jsonValue = Mogi::JSON_ARRAY;
		}

		// If this json value is already an array, access (or create) the array element
		if (isArrayValue(*value)) {
			// Access the element at the specified index. 
			// Create an array element if it doesn't exist TODO: ask
			
			int arraySize = jsonValue->size();

			// Create a new element
			if (index >= arraySize) { 

				// What is the element type?
				Mogi::JSONTokenType elemType = Mogi::JSON_DOUBLE; // Should default type be a double? TODO: ask
				if (arraySize > 0) {
					elemType = (*jsonValue)[0].getType(); // Get the element type from first element in array
				}

				(*jsonValue)[index] = elemType; // Underlying class will automatically create the necessary number of elements
			}

			// Now access element
			// Set storage parameter
			*storage = (void*) &((*jsonValue)[index]);
		}
		else {
			// ERROR: THIS SHOULD HAVE BEEN AN ARRAY NO MATTER WHAT
		}
	}

	void getValueFromKey(void** storage, void** value, const std::string& key, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;

		// This is not an object! How the fuck do I convert it?
		if (!isObjectValue(*value)) {

			/*// CASE 1: This json value does not have a parent
			if (!parentJSONValue) {
				//deleteJsonValue(value); // delete it 
				//*value = (void*)(new Mogi::JSONValue(JSON_ARRAY)); // Create new array
				*jsonValue = JSON_ARRAY;
			}
			// CASE 2: Parent is a JSON object
			else if (isObjectValue(*parentValue)) {
				// Try to find value in parent
				
			}*/
			*jsonValue = Mogi::JSON_OBJECT;
		}

		// If this json value is already an object, access (or create) the element
		if (isObjectValue(*value)) {
			// Access the element at the specified key. 
			// Create an element if it doesn't exist TODO: ask
			
			bool hasKey = jsonValue->hasKey(key);

			// Create a new element
			if (!hasKey) { 
				(*jsonValue)[key] = Mogi::JSON_NULL; // Underlying class will automatically create the element
			}

			// Now access element
			// Set storage parameter
			*storage = (void*) &((*jsonValue)[key]);
		}
		else {
			// ERROR: THIS SHOULD HAVE BEEN AN ARRAY NO MATTER WHAT
		}
	}

	std::vector<std::string> getKeys(void* value) {
		//std::vector<std::string> keys;
		//return keys;
		
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return jsonValue->getObjectKeys();

	}

	bool getBoolValue(const void* value) {
		//return false;
		const Mogi::JSONValue* jsonValue = (const Mogi::JSONValue*) value;
		return jsonValue->toBool();
	}

	int getIntValue(const void* value) {
		//return 0;
		const Mogi::JSONValue* jsonValue = (const Mogi::JSONValue*) value;
		return jsonValue->toInt();
	}

	double getDoubleValue(const void* value) {
		//return 0.0;
		const Mogi::JSONValue* jsonValue = (const Mogi::JSONValue*) value;
		return jsonValue->toDouble();
	}

	std::string getStringValue(const void* value) {
		//return "";
		const Mogi::JSONValue* jsonValue = (const Mogi::JSONValue*) value;
		return jsonValue->toString();		
	}

	int getArraySize(const void* value) {
		//return 0;
		const Mogi::JSONValue* jsonValue = (const Mogi::JSONValue*) value;
		return jsonValue->size();
	}

	void setBoolValue(void** value, const bool& val, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;
		*jsonValue = val;
	}

	void setIntValue(void** value, const int& val, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;
		*jsonValue = val;	
	}

	void setDoubleValue(void** value, const double& val, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;
		*jsonValue = val;	
	}

	void setStringValue(void** value, const std::string& val, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;
		*jsonValue = val;	
	}

	void setValueValue(void** value, void *const * val, void** parentValue) {
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) *value;
		const Mogi::JSONValue* otherJSONValue = (const Mogi::JSONValue*)*val;

		*jsonValue = *otherJSONValue;	
	}


	std::string createJsonString(void* value) {
		//return "";
		Mogi::JSONValue* jsonValue = (Mogi::JSONValue*) value;
		return jsonValue->toJSONString();
	}
};
