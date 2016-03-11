/*
 * JSONParser.cpp
 *
 *  Created on: Oct 5, 2014
 *      Author: adrian
 */

#include <mogi/JSONParser.h>
#include <cstring>
#include <sstream>
#include <fstream>
//#include <iostream>

namespace Mogi {

	const char* JSONValue::OUT_OF_RANGE = " - no such key found";

	JSONParser::JSONParser(): strPos(0), currentCompoundToken(0){}

	JSONParser::~JSONParser() {}

	void JSONParser::reset() {
		strPos = 0; currentCompoundToken = 0;
		tokenList = JSONTokenList();
	}

	int JSONParser::parseJSONFile(const std::string& filename, JSONValue& root) {
		std::ifstream inputStream(filename.c_str());
		std::stringstream buffer;

		buffer << inputStream.rdbuf();

		return this->parseJSONString(buffer.str(), root);
	}

	int JSONParser::parseJSONString(const std::string& jsonStr,
			JSONValue& root) {

		// reset
		reset();

		for (strPos = 0; strPos < jsonStr.length(); ++strPos) {
			char c = jsonStr[strPos];
			JSONTokenType type;
			JSONToken* token;

			switch (c) {
				case '{': case '[':
					// create new token (compound)
					token = addNewToken(tokenList);

					// if this is a nested object/array, increment parent's # children
					if (currentCompoundToken != 0) {
						currentCompoundToken->numChildren++;
						token->parent = currentCompoundToken;
					}

					token->startIndex = strPos;
					token->type = (c == '{') ? JSON_OBJECT : JSON_ARRAY;

					currentCompoundToken = token; // make the current token the currentCompoundToken

					break;
				case '}': case ']':

					// Shouldn't find ending brace if have no tokens
					if (tokenList.size() < 1) {
						return -1;
					}

					type = (c == '}') ? JSON_OBJECT : JSON_ARRAY;
					token = tokenList.back();

					// iterate through parent connections to find corresponding '{' or '['
					while (token != 0) {

						// Found a token missing ending index
						if (token->startIndex != -1 && token->endIndex == -1) {
							if (token->type != type) { // we shouldn't find incomplete tokens of diff type
								return -1;
							}
							token->endIndex = strPos;
							currentCompoundToken = token->parent; // update
							break;
						}

						token = token->parent;
					}

					break;
				case '\"': case '\'':

					// Parse string and add it as a token
					if (parseString(jsonStr, tokenList) < 0) {
						return -1;
					}

					// update size
					if (currentCompoundToken)
						currentCompoundToken->numChildren++;

					break;
				case '-': case '0': case '1' : case '2': case '3' : case '4':
				case '5': case '6': case '7' : case '8': case '9':
				case 't': case 'f': case 'n' :

					if (parsePrimitive(jsonStr, tokenList) < 0) {
						return -1;
					}

					// update size
					if (currentCompoundToken)
						currentCompoundToken->numChildren++;

					break;
				case ':':
					if (tokenList.size() < 1) {
						return -1;
					}

					token = tokenList.back();

					if (token->startIndex == -1 || token->endIndex == -1) {
						return -1;
					}

					token->isKey = true;
					break;
				case ' ': case '\n': case '\r': case '\t': case ',':
					// Skip these characters
					break;
			}
		}

		int conversion = initJSONValue(root, tokenList, jsonStr);
		return conversion;

	}

	int JSONParser::initJSONValue(JSONValue& root, JSONTokenList& tokenList, const std::string& jsonStr) {
		// DEBUG
		//std::cout << tokenList.toString() << std::endl;

		JSONToken* token = 0;
		std::istringstream iss;
		std::string currKey = "";

		std::vector<JSONValue*> parentQueue;
		std::vector<int> parentEnd;

		// Make sure the first token is either an object or array, otherwise return
		token = tokenList.front();
		if (token->type == JSON_OBJECT || token->type == JSON_ARRAY) {
			root = token->type;
			parentQueue.push_back(&root);
			parentEnd.push_back(token->endIndex);
		}
		else {
			return -2;
		}


		// Convert every other token (that is not a key) to a JSONValue.
		// From now on, all objects and primitives need a key (string). However,
		// tokens immediately inside an array should not have keys.
		for (token = tokenList.front()->next; token != 0; token = token->next) {
			JSONTokenType tokenType = token->type;
			bool isKey = token->isKey;

			while (token->startIndex > parentEnd.back()) {
				//std::cout << "popping parent that ends at " << parentEnd.back() << " for token starting at " << token->startIndex << std::endl;
				parentQueue.pop_back();
				parentEnd.pop_back();
			}

			JSONTokenType parentType = parentQueue.back()->getType();

			if (parentQueue.size() == 0 || parentEnd.size() == 0) {
				return -2;
			}

			if (isKey && token->next) {
				int keySize = token->endIndex - token->startIndex + 1;
				currKey = jsonStr.substr(token->startIndex, keySize);

				//std::cout << "key: " << currKey << std::endl;
			}
			else if (isKey && !token->next) {
				return -2;
			}
			else {
				int valSize = token->endIndex - token->startIndex + 1;
				std::string valStr = jsonStr.substr(token->startIndex, valSize);
				JSONValue value;

				//std::cout << "value: " << valStr << std::endl;
				//std::cout << "val type: " << tokenType << " parent type: " << parentType << " start: " << token->startIndex << " end: " << token->endIndex << std::endl;

				if(tokenType == JSON_INT){
					int intVal = 0;
					iss.clear();
					iss.str(valStr);
					iss >> intVal;
					value = intVal;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;
				}
				else if(tokenType == JSON_DOUBLE){
					double intVal = 0;
					iss.clear();
					iss.str(valStr);
					iss >> intVal;
					value = intVal;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;
				}
				else if(tokenType == JSON_BOOL){
					bool intVal = false;
					if (valStr[0] == 't')
						intVal = true;

					value = intVal;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;
				}
				else if (tokenType == JSON_STRING) {
					value = valStr;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;

				}
				else if (tokenType == JSON_ARRAY) {
					value = JSON_ARRAY;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;

					// Add new parent & end index
					if (parentType == JSON_ARRAY)
						parentQueue.push_back(&(*parentQueue.back()).back());
					else
						parentQueue.push_back(&(*parentQueue.back())[currKey]);

					parentEnd.push_back(token->endIndex);
				}
				else if (tokenType == JSON_OBJECT) {
					value = JSON_OBJECT;

					// Add this value to current parent value
					if (parentType == JSON_ARRAY)
						(*parentQueue.back()).pushBack(value);
					else
						(*parentQueue.back())[currKey] = value;

					// Add new parent & end index
					if (parentType == JSON_ARRAY)
						parentQueue.push_back(&(*parentQueue.back()).back());
					else
						parentQueue.push_back(&(*parentQueue.back())[currKey]);

					parentEnd.push_back(token->endIndex);

				}
				else { // UKNOWN tokenValType
					return -2;
				}


			}

		}

		return 0;

	}

	JSONToken* JSONParser::addNewToken(JSONTokenList& tokenList) {
		JSONToken token;
		token.startIndex = -1; token.endIndex = -1;
		token.numChildren = 0;
		token.parent = 0;
		token.isKey = false;

		tokenList.pushBack(token);

		return tokenList.back();
	}

	int JSONParser::parsePrimitive(const std::string& jsonStr,
			JSONTokenList& tokenList) {
		unsigned int i;
		char c = jsonStr[strPos];
		JSONTokenType type;

		if (c == 'f' || c == 't') {
			type = JSON_BOOL;
		}
		else if (c == 'n') {
			type = JSON_NULL;
		}
		else {
			type = JSON_INT; // or double if we find a '.' later on
		}

		// Scan the rest of the json string until find end of primitive (space, comma, or end-brackets)
		for (i = strPos; i < jsonStr.length(); i++) {
			c = jsonStr[i];
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
				c == ',' || c == '}' || c == ']') { // Found end of primitive
				// Create token for primitive
				JSONToken* token = addNewToken(tokenList);

				// Initialize token
				token->startIndex = strPos;
				token->endIndex = i - 1;
				token->type = type;
				token->parent = currentCompoundToken;

				// move strPos to end location and return
				strPos = i-1;
				return 0;
			}
			else if (c < 32 || c >= 127) {
				return -1;
			}
			else if (c == '.') {
				if (type == JSON_INT)
					type = JSON_DOUBLE;
				else
					return -1;
			}

		}

		return -1; // never found ending quote
	}

	int JSONParser::parseString(const std::string& jsonStr,
			JSONTokenList& tokenList) {
		unsigned int i;

		// Scan the rest of the string until find the ending quote
		// Start after the beginning quote
		for (i = strPos + 1; i < jsonStr.length(); i++) {
			char c = jsonStr[i];
			if (c == '\"' || c == '\'') { // Found end of string
				// Create token for string
				JSONToken* token = addNewToken(tokenList);

				// Initialize token
				token->startIndex = strPos+1;
				token->endIndex = i - 1;
				token->type = JSON_STRING;
				token->parent = currentCompoundToken;

				// move strPos to next location and return
				strPos = i;
				return 0;
			}
			else if (c == '\\') { // allow string to contain escaped chars
				i++;// skip next char
			}
		}

		return -1; // never found ending quote
	}


	/*
	 *  JSONValue Implementation
	 */

	JSONValue::JSONValue(JSONTokenType type) {
		mType = type;
		switch(type) {
		case JSON_NULL:
			break;
		case JSON_BOOL:
			mValue.boolVal = false;
			break;
		case JSON_INT:
			mValue.intVal = 0;
			break;
		case JSON_DOUBLE:
			mValue.doubleVal = 0.0;
			break;
		case JSON_STRING:
			mValue.stringVal = new char[1];
			mValue.stringVal = '\0';
			break;
		case JSON_ARRAY:
			mValue.arrayVal = new std::vector<JSONValue>();
			break;
		case JSON_OBJECT:
			mValue.objectVal = new std::map<std::string,JSONValue>();
			break;
		}

		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(bool value) {
		mType = JSON_BOOL;
		mValue.boolVal = value;
		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(int value) {
		mType = JSON_INT;
		mValue.intVal = value;
		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(double value) {
		mType = JSON_DOUBLE;
		mValue.doubleVal = value;
		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(const char* value) {
		mType = JSON_STRING;
		mValue.stringVal = new char[strlen(value) + 1];
		strcpy(mValue.stringVal, (char*)value);
		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(const std::string& value) {
		mType = JSON_STRING;
		mValue.stringVal = new char[value.length() + 1];
		strcpy(mValue.stringVal, (char*)value.c_str());
		mRefCount = new int;
		*mRefCount = 1;
	}

	JSONValue::JSONValue(const JSONValue& other) {
		mType = other.mType;
		mValue = other.mValue;
		mRefCount = other.mRefCount;
		*mRefCount = *mRefCount + 1;
	}

	JSONValue::~JSONValue() {
		*mRefCount = *mRefCount - 1;
		switch(mType) {
		case JSON_STRING:
			if (*mRefCount <= 0) {
				delete[] mValue.stringVal;
			}
			break;
		case JSON_ARRAY:
			if (*mRefCount <= 0){
				delete mValue.arrayVal;
			}
			break;
		case JSON_OBJECT:
			if (*mRefCount <= 0) {
				delete mValue.objectVal;
			}
			break;
		default:
			break;
		}

		if (*mRefCount <= 0) {
			delete mRefCount;
		}
	}

	JSONValue& JSONValue::operator =(JSONValue other) {
		swap(other);
		return *this;
	}

	bool JSONValue::operator ==(const JSONValue& other) const {
		if (other.mType != mType) {
			return false;
		}

		switch(mType)  {
		case JSON_NULL:
			return true;
		case JSON_BOOL:
			return (mValue.boolVal == other.mValue.boolVal);
		case JSON_INT:
			return (mValue.intVal == other.mValue.intVal);
		case JSON_DOUBLE:
			return (mValue.doubleVal == other.mValue.doubleVal);
		case JSON_STRING:
			return (strcmp(mValue.stringVal , other.mValue.stringVal) == 0);
		case JSON_ARRAY:
			return (mValue.arrayVal->size() == other.mValue.arrayVal->size() &&
					*(mValue.arrayVal) == *(other.mValue.arrayVal));
		case JSON_OBJECT:
			return (mValue.objectVal->size() == other.mValue.objectVal->size() &&
							*(mValue.objectVal) == *(other.mValue.objectVal));
		default:
			return false;
		}

		return false; // unreachable
	}

	bool JSONValue::operator !=(const JSONValue& other) const {
		return !(*this == other);

	}

	bool JSONValue::toBool() const {
		switch(mType)  {
		case JSON_NULL:
			return false;
		case JSON_BOOL:
			return mValue.boolVal;
		case JSON_INT:
			return (mValue.intVal != 0);
		case JSON_DOUBLE:
			return (mValue.doubleVal != 0.0);
		case JSON_STRING:
		case JSON_ARRAY:
		case JSON_OBJECT:
			return false;
		default:
			return false;
		}

		return false;
	}

	int JSONValue::toInt() const {
		switch(mType)  {
		case JSON_NULL:
			return 0;
		case JSON_BOOL:
			return mValue.boolVal ? 1: 0;
		case JSON_INT:
			return mValue.intVal;
		case JSON_DOUBLE:
			return (int)mValue.doubleVal;
		case JSON_STRING:
		case JSON_ARRAY:
		case JSON_OBJECT:
			return -1;
		default:
			return -1;
		}
	}

	double JSONValue::toDouble() const {
		switch(mType)  {
		case JSON_NULL:
			return 0;
		case JSON_BOOL:
			return mValue.boolVal ? 1.0 : 0.0;
		case JSON_INT:
			return mValue.intVal;
		case JSON_DOUBLE:
			return mValue.doubleVal;
		case JSON_STRING:
		case JSON_ARRAY:
		case JSON_OBJECT:
			return -1;
		default:
			return -1;
		}
	}

	float JSONValue::toFloat() const {
		switch(mType)  {
		case JSON_NULL:
			return 0;
		case JSON_BOOL:
			return mValue.boolVal ? 1.0 : 0.0;
		case JSON_INT:
			return mValue.intVal;
		case JSON_DOUBLE:
			return mValue.doubleVal;
		case JSON_STRING:
		case JSON_ARRAY:
		case JSON_OBJECT:
			return -1;
		default:
			return false;
		}
	}

	std::string JSONValue::toString() const {
		std::ostringstream oss;
		std::map<std::string,JSONValue>::iterator it;

		switch(mType)  {
		case JSON_NULL:
			return "null";
		case JSON_BOOL:
			return mValue.boolVal ? "true": "false";
		case JSON_INT:
			oss << mValue.intVal;
			return oss.str();
		case JSON_DOUBLE:
			oss << mValue.doubleVal;
			return oss.str();
		case JSON_STRING:
			oss << mValue.stringVal;
			return oss.str();
		case JSON_ARRAY:
			return this->toJSONString();
		case JSON_OBJECT:
			return this->toJSONString();
		default:
			return "";
		}
	}

	std::string JSONValue::toJSONString() const {
		std::ostringstream oss;
		std::map<std::string,JSONValue>::iterator it;
		unsigned int i = 0;


		switch(mType)  {
		case JSON_NULL:
			return "null";
		case JSON_BOOL:
			return mValue.boolVal ? "true": "false";
		case JSON_INT:
			oss << mValue.intVal;
			return oss.str();
		case JSON_DOUBLE:
			oss << mValue.doubleVal;
			return oss.str();
		case JSON_STRING:
			oss << "\"" << mValue.stringVal << "\"";
			return oss.str();
		case JSON_ARRAY:
			oss << "[";
			for(i=0; i < size(); i++) {
				oss << (*mValue.arrayVal)[i].toJSONString();
				if (i != size() - 1) {
					oss << ", ";
				}
			}
			oss << "]";
			return oss.str();
		case JSON_OBJECT:
			oss << "{";
			for(it = (*mValue.objectVal).begin(), i=0; it != (*mValue.objectVal).end(); it++, i++) {
				oss << "\"" << (it->first) << "\" : " <<(it->second).toJSONString();
				if (i != size() - 1) {
					oss << ", ";
				}
			}
			oss << "} ";
			return oss.str();
		default:
			return "";
		}
	}

	std::string JSONValue::toJSONStringPretty(const std::string& prefix) const {
		std::ostringstream oss;
		std::map<std::string,JSONValue>::iterator it;
		unsigned int i = 0;


		switch(mType)  {
		case JSON_NULL:
			return "null";
		case JSON_BOOL:
			return mValue.boolVal ? ("true") : ("false");
		case JSON_INT:
			oss << mValue.intVal;
			return oss.str();
		case JSON_DOUBLE:
			oss << mValue.doubleVal;
			return oss.str();
		case JSON_STRING:
			oss << "\"" << mValue.stringVal << "\"";
			return oss.str();
		case JSON_ARRAY:
			oss << "[";
			for(i=0; i < size(); i++) {
				oss << "\n" << prefix << "\t" << (*mValue.arrayVal)[i].toJSONStringPretty(prefix + "\t");
				if (i != size() - 1) {
					oss << ", ";
				}
			}
			oss << "\n" << prefix << "]";
			return oss.str();
		case JSON_OBJECT:
			oss << "{";
			for(it = (*mValue.objectVal).begin(), i=0; it != (*mValue.objectVal).end(); it++, i++) {
				oss << "\n" << prefix << "\t" << "\"" << (it->first) << "\" : " <<(it->second).toJSONStringPretty(prefix + "\t");
				if (i != size() - 1) {
					oss << ", ";
				}
			}
			oss << "\n" << prefix << "}";
			return oss.str();
		default:
			return "";
		}
	}

	unsigned int JSONValue::size() const {
		if (mType == JSON_ARRAY) {
			return (*mValue.arrayVal).size();
		}
		else if(mType == JSON_OBJECT) {
			return (*mValue.objectVal).size();
		}
		else {
			return 0;
		}
	}

	JSONValue& JSONValue::operator [](unsigned int index) {
		unsigned int numElem = (*mValue.arrayVal).size();

		// Return existing value
		if (index < numElem) {
			return (*mValue.arrayVal)[index];
		}

		// Create values in array up to size index+1
		for (unsigned int i = numElem; i <= index; i++) {
			(*mValue.arrayVal).push_back(JSONValue(JSON_NULL));
		}

		// return value at index
		return (*mValue.arrayVal).back();
	}

	const JSONValue& JSONValue::operator[](unsigned int index) const {
		return (*mValue.arrayVal)[index];
	}

	void JSONValue::pushBack(const JSONValue& val) {
		(*mValue.arrayVal).push_back(val);
	}

	JSONValue& JSONValue::back() {
		return (*mValue.arrayVal).back();
	}

	JSONValue& JSONValue::operator[](const std::string& key) {
		return (*mValue.objectVal)[key];
	}

	const JSONValue& JSONValue::operator[](const std::string& key) const{
		return (*mValue.objectVal)[key];
	}

	std::vector<std::string> JSONValue::getObjectKeys() {
		std::vector<std::string> keys;

		if (mType == JSON_OBJECT) {
			for(std::map<std::string,JSONValue>::iterator it = (*mValue.objectVal).begin(); it != (*mValue.objectVal).end(); it++) {
				keys.push_back(it->first);
			}
		}

		return keys;
	}

	bool JSONValue::hasKey(const std::string& key) const {
		bool has = false;
		if (mType == JSON_OBJECT) {
			const std::map<std::string,JSONValue>& m = *(mValue.objectVal);
			has = m.find(key) != m.end();
		}

		return has;
	}

	bool JSONValue::isValid() const {
		return (mType != JSON_NULL);
	}

	JSONTokenType JSONValue::getType() const {
		return mType;
	}

	void JSONValue::swap(JSONValue& other) {
		// Swap types
		JSONTokenType tempType = mType;
		mType = other.mType;
		other.mType = tempType;

		// Swap internal values
		std::swap(this->mValue, other.mValue);

		// Swap reference counts
		std::swap(this->mRefCount, other.mRefCount);
	}
} /* namespace Mogi */
