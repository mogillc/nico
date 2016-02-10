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

#include <iostream>

#include <math.h>    // sin,cos
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

#include "appcomm.h"

using namespace Mogi;
using namespace App;

class TestValue: public JsonValueObserver {
public:
	int count;
	~TestValue() {
	}
	void update(const Json::Value& newValue) {
		count++;
	}
};

bool testJsonValueObserver();
bool testJsonObjectObserver();
bool testJsonArrayObserver();

int main(int argc, char* argv[]) {
	bool allTestsPass = true;

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
