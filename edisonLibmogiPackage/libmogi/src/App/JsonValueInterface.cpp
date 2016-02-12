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
static const char* const JSONVALUEINTERFACE_C_Id = "$Id$";
#endif

#include <iostream>

#ifdef LIBJSONCPP_FOUND
#include <json/json.h>
#define VALUE_TYPE Json::Value
#define VALUE_CASTED ((VALUE_TYPE*)value)
#elif BUILD_FOR_IOS
#include "jsonWrapperIOS.h"
#endif // LIBJSONCPP_FOUND

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace App;

#ifdef LIBJSONCPP_FOUND

	int JsonValueInterface::parse( std::string jsonString, JsonValueInterface& value ) {
		Json::Reader reader;

		if (!reader.parse(jsonString, *(VALUE_TYPE*)(value.getRaw()))) {
			std::cerr << "Unable to parse JSON configuration from: " << jsonString << std::endl;
			return -1;
		}
		return 0;
	}

	int JsonValueInterface::parse( const char *beginDoc, const char *endDoc, JsonValueInterface& value ) {
		Json::Reader reader;

		if (!reader.parse(beginDoc, endDoc, *(VALUE_TYPE*)(value.getRaw()))) {
			std::cerr << "Unable to parse JSON configuration... " << std::endl;
			return -1;
		}
		return 0;
	}

	JsonValueInterface::JsonValueInterface()
 :value(NULL), child(NULL) {
		value = new VALUE_TYPE;
		child = NULL;
	}
	JsonValueInterface::JsonValueInterface(const JsonValueInterface& other) {
		value = new VALUE_TYPE;
		*VALUE_CASTED = *(VALUE_TYPE*)(other.value);
		if (other.child != NULL) {
			child = new JsonValueInterface;
			*child = *other.child;
		} else {
			child = NULL;
		}
	}

	JsonValueInterface::~JsonValueInterface() {
		if (child != NULL) {
			delete child;
			child = NULL;
		}
		delete VALUE_CASTED;
		value = NULL;
	}

	bool JsonValueInterface::isBool() const {
		return VALUE_CASTED->isBool();
	}
	bool JsonValueInterface::isInt() const {
		return VALUE_CASTED->isInt();
	}
	bool JsonValueInterface::isDouble() const {
		return VALUE_CASTED->isDouble();
	}
	bool JsonValueInterface::isString() const {
		return VALUE_CASTED->isString();
	}
	bool JsonValueInterface::isArray() const {
		return VALUE_CASTED->isArray();
	}
	bool JsonValueInterface::isObject() const {
		return VALUE_CASTED->isObject();
	}



	int JsonValueInterface::asBool() const {
		return VALUE_CASTED->asBool();
	}
	int JsonValueInterface::asInt() const {
		return VALUE_CASTED->asInt();
	}
	double JsonValueInterface::asDouble() const {
		return VALUE_CASTED->asDouble();
	}
	std::string JsonValueInterface::asString() const {
		return VALUE_CASTED->asString();
	}
	int JsonValueInterface::size() const {
		return VALUE_CASTED->size();
	}

	JsonValueInterface& JsonValueInterface::operator[](const unsigned int& index) {
		if (child == NULL) {
			child = new JsonValueInterface();
		}
		*((VALUE_TYPE*)(child->value)) = (*VALUE_CASTED)[index];
		return *child;
	}

//	JsonValueInterface JsonValueInterface::operator[](const char* key) const {
//		JsonValueInterface result;
//		*((VALUE_TYPE*)result.value) = (*VALUE_CASTED)[key];
//		return result;
//	}

	JsonValueInterface& JsonValueInterface::operator[](const std::string& key) {
		if (child == NULL) {
			child = new JsonValueInterface();
		}
		*((VALUE_TYPE*)(child->value)) = (*VALUE_CASTED)[key];
		return *child;
	}

	std::vector<std::string> JsonValueInterface::getMemberNames() const {
		return VALUE_CASTED->getMemberNames();
	}

	void* JsonValueInterface::getRaw() {
		return value;
	}

	JsonValueInterface& JsonValueInterface::operator=( const bool& other ) {
		*VALUE_CASTED = other;
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const int& other ) {
		*VALUE_CASTED = other;
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const double& other ) {
		*VALUE_CASTED = other;
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const std::string& other ) {
		*VALUE_CASTED = other;
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const JsonValueInterface& other ) {
		*VALUE_CASTED = *(VALUE_TYPE*)(other.value);
		return *this;
	}

	std::string JsonValueInterface::toStyledString() const {
		return VALUE_CASTED->toStyledString();
	}

#elif BUILD_FOR_IOS // LIBJSONCPP_FOUND

	using namespace _JsonWrapperIOS;

	int JsonValueInterface::parse( std::string jsonString, JsonValueInterface& value ) {
		//std::cerr << "Error: JsonValueInterface::parse() is unsupported in this build." << std::endl;
		return parseJson( &value.value, jsonString);
	}

	int JsonValueInterface::parse( const char *beginDoc, const char *endDoc, JsonValueInterface& value ) {
		std::cerr << "Error: JsonValueInterface::parse() is unsupported in this build." << std::endl;
		return -1;
		return parseJson( &value.value, std::string(beginDoc, endDoc-beginDoc) );
	}

	JsonValueInterface::JsonValueInterface()
	:value(NULL), child(NULL), parent(NULL) {
		value = getNewJsonValue();
	}
	JsonValueInterface::JsonValueInterface(const JsonValueInterface& other) {
		std::cout << "In copy constructor)" << std::endl;
		value = getNewJsonValue();
//		setValueValue( &value, &other.value, &parent->value);
		parent = NULL;
		child = NULL;
		setValueValue( &value, &other.value, parent == NULL ? NULL : &parent->value );
		//*VALUE_CASTED = *(VALUE_TYPE*)(other.value);	// TODO: make copy function.
		//parent = other.parent;
//		if (other.child != NULL) {
//			child = new JsonValueInterface;
//			*child = *other.child;
//		} else {
//			child = NULL;
//		}
	}

	JsonValueInterface::~JsonValueInterface() {
		// Anything to be done here?
		//deleteJsonValue(&value);
		if (child != NULL) {
			child->parent = NULL;
			delete child;
			child = NULL;
		}
	}

	bool JsonValueInterface::isBool() const {
		return isBoolValue( value );
	}
	bool JsonValueInterface::isInt() const {
		return isIntValue( value );
	}
	bool JsonValueInterface::isDouble() const {
		return isDoubleValue( value );
	}
	bool JsonValueInterface::isString() const {
		return isStringValue( value );
	}
	bool JsonValueInterface::isArray() const {
		return isArrayValue( value );
	}
	bool JsonValueInterface::isObject() const {
		return isObjectValue( value );
	}



	int JsonValueInterface::asBool() const {
		return getBoolValue( value );
	}
	int JsonValueInterface::asInt() const {
		return getIntValue( value );
	}
	double JsonValueInterface::asDouble() const {
		return getDoubleValue( value );
	}
	std::string JsonValueInterface::asString() const {
		return getStringValue( value );
	}
	int JsonValueInterface::size() const {
		return getArraySize( value );
	}






	JsonValueInterface& JsonValueInterface::operator[](const unsigned int& index) {
		if (child == NULL) {
			child = new JsonValueInterface();
		}
//		if (!this->isArray()) {
//			makeArray( &value, other, parent == NULL ? NULL : &parent->value );
//		}
		getValueFromIndex( &(child->value), &value, index, parent == NULL ? NULL : &parent->value  );
		child->parent = this;
		return *child;
	}




	JsonValueInterface& JsonValueInterface::operator[](const std::string& key) {
		if (child == NULL) {
			child = new JsonValueInterface();
		}
//		if (this->isObject()) {
//			std::cout << " - operator[str] before: " << this->toStyledString() << std::endl;
//		} else {
//			std::cout << " - Oops, this is not yet an object..." << std::endl;
//		}

		getValueFromKey( &(child->value), &value, key, parent == NULL ? NULL : &parent->value  );
		child->parent = this;
//		std::cout << " - operator[str] after : " << this->toStyledString() << std::endl;
//		std::cout << " - operator[str] child value ptr : " << (long)child->value << std::endl;
		return *child;
	}

	std::vector<std::string> JsonValueInterface::getMemberNames() const {
		return getKeys( value );
	}

	void* JsonValueInterface::getRaw() {
		return value;
	}

	JsonValueInterface& JsonValueInterface::operator=( const bool& other ) {
		setBoolValue( &value, other, parent == NULL ? NULL : &parent->value );
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const int& other ) {
		setIntValue( &value, other, parent == NULL ? NULL : &parent->value );
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const double& other ) {
		setDoubleValue( &value, other, parent == NULL ? NULL : &parent->value );
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const std::string& other ) {
		std::cout << "In operator=(str), ptr = " << (long) value << std::endl;
		setStringValue( &value, other, parent == NULL ? NULL : &parent->value );
		std::cout << " - Result:" << getStringValue(value) << std::endl;
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const JsonValueInterface& other ) {
		setValueValue( &value, &other.value, parent == NULL ? NULL : &parent->value );
		return *this;
	}

	std::string JsonValueInterface::toStyledString() const {
		return createJsonString( value );
	}


#else

	int JsonValueInterface::parse( std::string jsonString, JsonValueInterface& value ) {
		std::cerr << "Error: JsonValueInterface::parse() is unsupported in this build." << std::endl;
		return -1;
	}

	int JsonValueInterface::parse( const char *beginDoc, const char *endDoc, JsonValueInterface& value ) {
		std::cerr << "Error: JsonValueInterface::parse() is unsupported in this build." << std::endl;
		return -1;
	}

	JsonValueInterface::JsonValueInterface()
	:value(NULL), child(NULL) {
	}
	JsonValueInterface::JsonValueInterface(const JsonValueInterface& other)
	:value(NULL), child(NULL) {
	}
	JsonValueInterface::~JsonValueInterface() {
	}

	bool JsonValueInterface::isBool() const {
		return false;
	}
	bool JsonValueInterface::isInt() const {
		return false;
	}
	bool JsonValueInterface::isDouble() const {
		return false;
	}
	bool JsonValueInterface::isString() const {
		return false;
	}
	bool JsonValueInterface::isArray() const {
		return false;
	}
	bool JsonValueInterface::isObject() const {
		return false;
	}



	int JsonValueInterface::asBool() const {
		return false;
	}
	int JsonValueInterface::asInt() const {
		return 0;
	}
	double JsonValueInterface::asDouble() const {
		return 0;
	}
	std::string JsonValueInterface::asString() const {
		return "";
	}
	int JsonValueInterface::size() const {
		return 0;
	}

	JsonValueInterface& JsonValueInterface::operator[](const unsigned int& index) {
		return *this;
	}

	JsonValueInterface& JsonValueInterface::operator[](const std::string& key) {
		return *this;
	}

	std::vector<std::string> JsonValueInterface::getMemberNames() const {
		return std::vector<std::string>();
	}

	void* JsonValueInterface::getRaw() {
		return value;
	}

	JsonValueInterface& JsonValueInterface::operator=( const bool& other ) {
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const int& other ) {
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const double& other ) {
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const std::string& other ) {
		return *this;
	}
	JsonValueInterface& JsonValueInterface::operator=( const JsonValueInterface& other ) {
		return *this;
	}

	std::string JsonValueInterface::toStyledString() const {
		return "";
	}


#endif // LIBJSONCPP_FOUND

#ifdef _cplusplus
}
#endif
