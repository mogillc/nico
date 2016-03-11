/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Adrian Lizarraga                        *
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

/*
 * JSONParser.h
 *
 *  Created on: Oct 5, 2014
 *      Author: adrian
 */

#ifndef JSONPARSER_H_
#define JSONPARSER_H_
#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace Mogi {

	enum JSONTokenType {
		JSON_NULL = 0,
		JSON_BOOL = 1,
		JSON_INT = 2,
		JSON_DOUBLE = 3,
		JSON_STRING = 4,
		JSON_ARRAY = 5,
		JSON_OBJECT = 6
	};

	struct JSONToken {
		JSONTokenType type;
		int startIndex;
		int endIndex;
		int numChildren;
		bool isKey;

		JSONToken* parent;
		JSONToken* next;
		JSONToken* prev;
	};

	class JSONTokenList {
	public:
		JSONTokenList() {
			numTokens = 0;
			head = 0;
			tail = 0;
			refCount = new int;
			*refCount = 1;
		}

		JSONTokenList(const JSONTokenList& other) {
			numTokens = other.numTokens;
			head = other.head;
			tail = other.tail;
			refCount = other.refCount;
			*refCount = *refCount + 1;
		}

		~JSONTokenList() {
			// deallocate memory
			*refCount = *refCount - 1;
			if (*refCount <= 0) {
				deallocateList();
				delete refCount;
			}
		}

		JSONTokenList& operator=(const JSONTokenList& other) {
			if (this != &other) {
				// deallocate memory
				*refCount = *refCount - 1;
				if (*refCount <= 0) {
					deallocateList();
					delete refCount;
				}

				numTokens = other.numTokens;
				head = other.head;
				tail = other.tail;
				refCount = other.refCount;
				*refCount = *refCount + 1;
			}

			return *this;
		}

		std::string toString() {
			std::ostringstream oss; 
			for (JSONToken* token = front(); token != 0; token = token->next) {
				oss << "type: " << token->type << ", start: " << token->startIndex << ", end: " << token->endIndex << ", #child: " << token->numChildren << ", isKey: " << token->isKey << std::endl;
			}

			return oss.str();
		}

		JSONToken* front() {return head;}
		JSONToken* back() {return tail;}
		int size() {return numTokens;}

		void deallocateList() {
			JSONToken* token = head;
			JSONToken* next = 0;
			while (token != 0) {
				next = token->next;
				delete token;
				token = next;
			}

			head = tail = 0;
			numTokens = 0;
		}

		void pushBack(const JSONToken& token) {
			JSONToken* token_cpy = new JSONToken(token);

			if (head == 0) {
				head = token_cpy;
				tail = token_cpy;
				token_cpy->next = 0;
				token_cpy->prev = 0;
			}
			else {
				tail->next = token_cpy;
				token_cpy->prev = tail;
				token_cpy->next = 0;
				tail = token_cpy;
			}
			numTokens++;
		}

		void pushFront(const JSONToken& token) {
			JSONToken* token_cpy = new JSONToken(token);

			if (head == 0) {
				head = token_cpy;
				tail = token_cpy;
				token_cpy->next = 0;
				token_cpy->prev = 0;
			}
			else {
				head->prev = token_cpy;
				token_cpy->prev = 0;
				token_cpy->next = head;
				head = token_cpy;
			}
			numTokens++;
		}

		JSONToken* findValueToken(const std::string& key, const std::string& jsonStr) {
			for(JSONToken* token = head; token != 0; token = token->next) {
				int size = token->endIndex - token->startIndex + 1;
				std::string subStr = jsonStr.substr(token->startIndex,size);

				if (token->isKey && subStr == key) {
					return token->next;
				}
			}

			return 0;
		}


	private:
		int numTokens;
		JSONToken* head;
		JSONToken* tail;

		int* refCount;
	};


	class JSONValue {
	public:
		JSONValue(JSONTokenType type = JSON_NULL);
		JSONValue(bool value);
		JSONValue(int value);
		JSONValue(double value);
		JSONValue(const char* value);
		JSONValue(const std::string& value);
		JSONValue(const JSONValue& other); // copy constructor

		~JSONValue();
		JSONValue& operator=(JSONValue other); // assignment operator
								               // note that it does not take argument by const reference
			                                   // in order to enable the use of the copy-swap idiom

		// Comparison operators
		bool operator==(const JSONValue& other) const;
		bool operator!=(const JSONValue& other) const;

		// Conversion "getters"
		bool toBool() const;
		int toInt() const;
		double toDouble() const;
		float toFloat() const;
		std::string toString() const;
		std::string toJSONString() const;
		std::string toJSONStringPretty(const std::string& prefix) const;

		// Returns number of children, zero if not an array or object
		unsigned int size() const;

		// Array accessors
		// creates value if index non-existent
		JSONValue& operator[](unsigned int index);
		// throws exception if index non existent
		const JSONValue& operator[](unsigned int index) const;
		void pushBack(const JSONValue& val);
		JSONValue& back();


		// Object member accessors
		// creates null values if key non-existent
		JSONValue& operator[](const std::string& key);
		const JSONValue& operator[](const std::string& key) const;
		std::vector<std::string> getObjectKeys();
		bool hasKey(const std::string& key) const;

		bool isValid() const; // Returns true if type is not JSON_NULL
		JSONTokenType getType() const;

	private:
		union value_u {
			bool boolVal;
			int intVal;
			double doubleVal;
			char * stringVal;
			std::map<std::string,JSONValue>* objectVal;
			std::vector<JSONValue>* arrayVal;
		} mValue; // Union to represent all possible json values
		int* mRefCount; // Reference count to account for object copying
		JSONTokenType mType;

		static const char* OUT_OF_RANGE;

		void swap(JSONValue& other); // copy-swap idiom
	};

	class JSONParser {
	public:
		JSONParser();
		virtual ~JSONParser();


		int parseJSONString(const std::string& jsonStr, JSONValue& root);
		int parseJSONFile(const std::string& filename, JSONValue& root);

	private:
		unsigned int strPos;
		JSONToken* currentCompoundToken;
		JSONTokenList tokenList;

		JSONToken* addNewToken(JSONTokenList& tokenList);
		int parsePrimitive(const std::string& jsonStr, JSONTokenList& tokenList);
		int parseString(const std::string& jsonStr, JSONTokenList& tokenList);
		void reset();
		int initJSONValue(JSONValue& root, JSONTokenList& tokenList, const std::string& jsonStr);
	};

} /* namespace Mogi */

#endif /* JSONPARSER_H_ */
