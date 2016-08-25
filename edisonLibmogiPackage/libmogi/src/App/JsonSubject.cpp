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
static const char* const JSONSUBJECT_C_Id = "$Id$";
#endif

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace App;

	void JsonSubject::addValueObserver(const std::string& key,
									   JsonValueObserver* valueObserver) {
		JsonKeyObserver* keyObserver = keyObservers[key];
		if (keyObserver == NULL) {
			keyObserver = new JsonKeyObserver;
			keyObservers[key] = keyObserver;
		}
		keyObserver->addValueObserver(valueObserver);
	}

	void JsonSubject::eraseValueObserver(const std::string& key,
										 JsonValueObserver* valueObserver) {
		JsonKeyObserver* keyObserver = keyObservers[key];
		if (keyObserver == NULL) {
			return;
		}
		keyObserver->eraseValueObserver(valueObserver);
		if (keyObserver->valueObserverCount() <= 0) {
			for (std::map<std::string, JsonKeyObserver*>::iterator it =
				 keyObservers.begin(); it != keyObservers.end(); it++) {
				if (it->second == keyObserver) {
					// std::cerr << "Erasing keyObserver:" << it->first << std::endl;
					// std::cerr << " - map size is     " << keyObservers.size() <<
					// std::endl;
					// keyObservers.erase(it);
					keyObserversToRemove.push_back(it->second);
					// std::cerr << " - map size is now " << keyObservers.size() <<
					// std::endl;
					return;
				}
			}
		}
	}

	JsonSubject::~JsonSubject() {
		for (std::map<std::string, JsonKeyObserver*>::iterator it =
			 keyObservers.begin(); it != keyObservers.end(); it++) {
			delete it->second;
		}
	}

	bool JsonSubject::parseJson(const std::string::iterator& begin, const std::string::iterator& end) {
		JsonValueInterface root; // will contains the root value after parsing.
		
		bool parsingSuccessful = !JsonValueInterface::parse(&*begin, &*end, root);

		if (parsingSuccessful) {
			notifyObservers(root);
		}

		return parsingSuccessful;
	}

	bool JsonSubject::parseJson(const std::string::iterator& begin, const std::string::iterator& end,
											std::string& resp) {
		JsonValueInterface root; // will contains the root value after parsing.
		
		bool parsingSuccessful = !JsonValueInterface::parse(&*begin, &*end, root);

		if (parsingSuccessful) {
			notifyObservers(root,resp);
		}

		return parsingSuccessful;
	}

	void JsonSubject::notifyObservers( JsonValueInterface& jsonObject) {
		for (std::vector<JsonKeyObserver*>::iterator it =
			 keyObserversToRemove.begin(); it != keyObserversToRemove.end();
			 it++) {
			for (std::map<std::string, JsonKeyObserver*>::iterator it2 =
				 keyObservers.begin(); it2 != keyObservers.end(); it2++) {
				if (*it == it2->second) {
					keyObservers.erase(it2);
					break;
				}
			}
		}
		keyObserversToRemove.clear();

		std::vector<std::string> keys = jsonObject.getMemberNames();
		for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end();
			 it++) {
			JsonKeyObserver* keyObserver = keyObservers[*it];
			if (keyObserver != NULL) {
				keyObserver->update(jsonObject[*it]);
			}
		}
	}

	void JsonSubject::notifyObservers( JsonValueInterface& jsonObject,std::string& resp) {
		for (std::vector<JsonKeyObserver*>::iterator it =
			 keyObserversToRemove.begin(); it != keyObserversToRemove.end();
			 it++) {
			for (std::map<std::string, JsonKeyObserver*>::iterator it2 =
				 keyObservers.begin(); it2 != keyObservers.end(); it2++) {
				if (*it == it2->second) {
					keyObservers.erase(it2);
					break;
				}
			}
		}
		keyObserversToRemove.clear();

		std::vector<std::string> keys = jsonObject.getMemberNames();
		for (std::vector<std::string>::iterator it = keys.begin(); it != keys.end();
			 it++) {
			JsonKeyObserver* keyObserver = keyObservers[*it];
			if (keyObserver != NULL) {
				keyObserver->update(jsonObject[*it],resp);
			}
		}
	}
	void JsonSubject::JsonKeyObserver::addValueObserver(
														JsonValueObserver* valueObserver) {
		valueObservers.push_back(valueObserver);
	}

	void JsonSubject::JsonKeyObserver::eraseValueObserver(
														  JsonValueObserver* valueObserver) {
		for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			 it != valueObservers.end(); it++) {
			if (valueObserver == *it) {
				//				std::cerr << "Erasin valueObserver: " <<
				//valueObserver << std::endl;
				//				valueObservers.erase(it);	// This
				//breaks the iterators above this method dammit!
				valuesObserversToErase.push_back(*it);
				return;
			}
		}
	}

	int JsonSubject::JsonKeyObserver::valueObserverCount() {
		return valueObservers.size();
	}

	void JsonSubject::JsonKeyObserver::update( JsonValueInterface& newValue) {
		//		if (valuesObserversToErase.size() > 0) {
		//			std::cout << " - - number of value obxervers BEFORE pruning: "
		//<< valueObservers.size() << std::endl;
		//		}

		for (std::vector<JsonValueObserver*>::iterator it =
			 valuesObserversToErase.begin(); it != valuesObserversToErase.end();
			 it++) {
			//			std::cerr << "Actually erasing value obs now " <<
			//std::endl;
			for (std::vector<JsonValueObserver*>::iterator it2 =
				 valueObservers.begin(); it2 != valueObservers.end(); it2++) {
				if (*it == *it2) {
					valueObservers.erase(it2);
					break;
				}
			}
		}
		//		if (valuesObserversToErase.size() > 0) {
		//			std::cout << " - - number of value obxervers AFTER pruning: " <<
		//valueObservers.size() << std::endl;
		//		}
		valuesObserversToErase.clear();

		for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			 it != valueObservers.end(); it++) {
			(*it)->update(newValue);
		}
	}

	void JsonSubject::JsonKeyObserver::update( JsonValueInterface& newValue,std::string& resp) {
		//		if (valuesObserversToErase.size() > 0) {
		//			std::cout << " - - number of value obxervers BEFORE pruning: "
		//<< valueObservers.size() << std::endl;
		//		}

		for (std::vector<JsonValueObserver*>::iterator it =
			 valuesObserversToErase.begin(); it != valuesObserversToErase.end();
			 it++) {
			//			std::cerr << "Actually erasing value obs now " <<
			//std::endl;
			for (std::vector<JsonValueObserver*>::iterator it2 =
				 valueObservers.begin(); it2 != valueObservers.end(); it2++) {
				if (*it == *it2) {
					valueObservers.erase(it2);
					break;
				}
			}
		}
		//		if (valuesObserversToErase.size() > 0) {
		//			std::cout << " - - number of value obxervers AFTER pruning: " <<
		//valueObservers.size() << std::endl;
		//		}
		valuesObserversToErase.clear();

		for (std::vector<JsonValueObserver*>::iterator it = valueObservers.begin();
			 it != valueObservers.end(); it++) {
			(*it)->update(newValue,resp);
		}
	}

	bool Mogi::setIntValueIfSafe(int* storage, JsonValueInterface& value) {
		if (value.isInt()) {
			*storage = value.asInt();
			return true;
		}
		return false;
	}

	bool Mogi::setDoubleValueIfSafe(double* storage, JsonValueInterface& value) {
		if (value.isDouble()) {
			*storage = value.asDouble();
			return true;
		}
		return false;
	}

	bool Mogi::setStringValueIfSafe(std::string* storage, JsonValueInterface& value) {
		if (value.isString()) {
			*storage = value.asString();
			return true;
		}
		return false;
	}
	
#ifdef _cplusplus
}
#endif
