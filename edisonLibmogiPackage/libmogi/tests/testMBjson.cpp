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

#include <iostream>

#include <math.h>    // sin,cos
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

#include "appcomm.h"

#include <json/json.h>

using namespace Mogi;
using namespace App;

class TestValue: public JsonValueObserver {
public:
	int count;
	~TestValue() {
	}
	void update( JsonValueInterface& newValue) {
		count++;
	}
};

bool testJsonValueInterface();
bool testJsonValueObserver();
bool testJsonObjectObserver();
bool testJsonArrayObserver();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

	std::cout << " - Beginning ValueInterface tests:" << std::endl;
	allTestsPass = testJsonValueInterface() ? allTestsPass : false;
	std::cout << " - Beginning Value Observer tests:" << std::endl;
	allTestsPass = testJsonValueObserver() ? allTestsPass : false;
	std::cout << " - Beginning Json Object Observer tests:" << std::endl;
	allTestsPass = testJsonObjectObserver() ? allTestsPass : false;
	std::cout << " - Beginning Json Array Observer tests:" << std::endl;
	allTestsPass = testJsonArrayObserver() ? allTestsPass : false;

	if (allTestsPass) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

bool testJsonValueInterface() {
	bool allTestsPass = true;

	JsonValueInterface value;

	value["testArray"][6] = 4;

	JsonValueInterface value2(value);

	value["testDouble"][3] = 5.6;
	value["testString"] = std::string("yoyoy");

	JsonValueInterface result;

	std::cout << " - Checking value creation/parsing ... ";
	if(JsonValueInterface::parse(value.toStyledString(), result)) {
		std::cout << "FAILED during parsing." << std::endl;
		return false;
	}
	
	if ((!result["testArray"].isArray()) ||
		result["testArray"].size() != 7 ||
		!result["testArray"][6].isInt() ||
		result["testArray"][6].asInt() != 4) {
		std::cout << "FAILED: testArray incorrect" << std::endl;
		allTestsPass = false;
	}

	if ((!result["testDouble"].isArray()) ||
		result["testDouble"].size() != 4 ||
		!result["testDouble"][3].isDouble() ||
		result["testDouble"][3].asDouble() != 5.6) {
		std::cout << "FAILED: testDouble incorrect" << std::endl;
		allTestsPass = false;
	}

	if (!result["testString"].isString() ||
		result["testString"].asString().compare("yoyoy") != 0) {
		std::cout << "FAILED: testString incorrect" << std::endl;
		allTestsPass = false;
	}

	if (allTestsPass) {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testJsonValueObserver() {
	bool allTestsPass = true;

	JsonSubject jsonSubject;

	TestValue testValue;
	testValue.count = 0;
	jsonSubject.addValueObserver("testKey", &testValue);

	std::cout << " - Checking value before ............. ";
	if (testValue.count != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	std::string jsonString = "{\"testKey\" : 3.4}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking value after .............. ";
	if (testValue.count != 1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	TestValue testValue2;
	testValue2.count = 0;

	jsonSubject.addValueObserver("testKey", &testValue2);

	jsonString = "{\"testKey\" : 3.4}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking two values, same key ..... ";
	if (testValue.count != 2 || testValue2.count != 1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	TestValue testValue3;
	testValue3.count = 0;

	jsonSubject.addValueObserver("testKey2", &testValue3);

	jsonString = "{\"testKey2\" : 3.4}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking two keys, one value ...... ";
	if (testValue.count != 2 || testValue2.count != 1
			|| testValue3.count != 3) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	jsonString = "{\"testKey2\" : 3.4, \n   \"testKey\": 7.8}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking two keys, two values ..... ";
	if (testValue.count != 5 || testValue2.count != 4
			|| testValue3.count != 6) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testJsonObjectObserver() {
	bool allTestsPass = true;

	JsonSubject jsonSubject;

	JsonObjectObserver testObject;
	TestValue testValue, testValue2;
	testValue.count = 0;
	testValue2.count = 0;
	// add an object observer to the subject root
	jsonSubject.addValueObserver("testKey", &testObject);

	// add a value observer to the subject root
	jsonSubject.addValueObserver("value1", &testValue2);

	// add a value observer to the object observer
	testObject.addValueObserver("value1", &testValue);

	std::string jsonString = "{\"testKey\" : {\r\n\t \"value1\": true}}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking object in/out of object .. ";
	if (testValue.count != 1 || testValue2.count != 0) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}

bool testJsonArrayObserver() {
	bool allTestsPass = true;

	JsonSubject jsonSubject;

	JsonObjectObserver testObject;
	JsonArrayObserver testArray;
	TestValue testValue, testValue2;
	testValue.count = 0;
	testValue2.count = 0;
	// add observer elements to the array observer
	testArray.addElementObserver(4, &testValue);
	testArray.addElementObserver(3, &testObject);

	// add a value observer to the object observer
	testObject.addValueObserver("key", &testValue2);

	// add the array observer to the subject
	jsonSubject.addValueObserver("testKey", &testArray);

	std::string jsonString =
			"{\"testKey\" : [  3.4, true, \"something\", {\"key\": 6.734}]}";

	jsonSubject.parseJson(jsonString.begin(), jsonString.end());
	std::cout << " - Checking array with object ........ ";
	if (testValue.count != 0 || testValue2.count != 1) {
		std::cout << "FAILED" << std::endl;
		allTestsPass = false;
	} else {
		std::cout << "Passed" << std::endl;
	}

	return allTestsPass;
}
