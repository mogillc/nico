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

#ifndef MOGI_JSON_H
#define MOGI_JSON_H

#include <map>
#include <vector>
#include <string>

namespace Mogi {

	/*!
	 * \namespace Mogi::App
	 * \brief Tools for communicating with the downloadable app.
	 */
	namespace App {

		// class JsonKeyObserver;
		class JsonValueObserver;

		/*!
		 @class JsonValueInterface
		 \brief Acts as an interface to JSON values handled by separate parsers on varying platforms.
		 @since 2016-02-10
		 */
		class JsonValueInterface {
		private:
			void* value;
			JsonValueInterface* child;
			JsonValueInterface* parent;
		public:

			static int parse( std::string jsonString, JsonValueInterface& value );
			static int parse( const char *beginDoc, const char *endDoc, JsonValueInterface& value );

			JsonValueInterface();
			JsonValueInterface(const JsonValueInterface&);
			~JsonValueInterface();

			bool isBool() const;
			bool isInt() const;
			bool isDouble() const;
			bool isString() const;
			bool isArray() const;
			bool isObject() const;

			int asBool() const;
			int asInt() const;
			double asDouble() const;
			std::string asString() const;
			int size() const;	// array size, if array
			JsonValueInterface& operator[](const unsigned int&);
			//JsonValueInterface operator[](const char*) const;
			JsonValueInterface& operator[](const std::string&);

			std::vector<std::string> getMemberNames() const;

			JsonValueInterface& operator=( const bool& other );
			JsonValueInterface& operator=( const int& other );
			JsonValueInterface& operator=( const double& other );
			JsonValueInterface& operator=( const std::string& other );
			JsonValueInterface& operator=( const JsonValueInterface& other );

			std::string toStyledString() const;

			void* getRaw();
		};

		/*!
		 @class JsonSubject
		 \brief Handles JSON parsing a nd observer notifications.
		 Notifications occur when the value, array, or object is changed from parsing.
		 @since 2015-05-12
		 */
		class JsonSubject {
		private:
			class JsonKeyObserver {
			private:
				std::vector<JsonValueObserver*> valuesObserversToErase; // TODO: change to std::remove
				std::vector<JsonValueObserver*> valueObservers;

			public:
				void addValueObserver(JsonValueObserver* valueObserver);
				void eraseValueObserver(JsonValueObserver* valueObserver);
				int valueObserverCount();
				void update( JsonValueInterface& newValue); // notifies all value observers
			};

		private:
			std::vector<JsonKeyObserver*> keyObserversToRemove; // TODO: change to std::remove
			std::map<std::string, JsonKeyObserver*> keyObservers;

		protected:

			/**
			 \brief Notifies all observers of a value change.
			 \param jsonObject The new value.
			 */
			void notifyObservers( JsonValueInterface& jsonObject);

		public:
			~JsonSubject();

			/**
			 Given a key, this will create a key observer or append the listener to an
			 existing key observer
			 */
			void addValueObserver(const std::string& key, JsonValueObserver* valueObserver);

			/**
			 Given a key, this will create a key observer or append the listener to an
			 existing key observer
			 */
			void eraseValueObserver(const std::string& key, JsonValueObserver* valueObserver);

			/**
			 Notifies all key observers when the json string contains the key
			 */
			bool parseJson(const std::string::iterator& begin,
						   const std::string::iterator& end);
		};

		/*!
		 @class JsonValueObserver
		 \brief An abstract JSON value observer.
		 Notifications occur when the value is changed from parsing.
		 @since 2015-05-12
		 */
		class JsonValueObserver {
		public:
			virtual ~JsonValueObserver() {
			}

			/**
			 \brief Called when a JSON value has been updated.
			 \param newValue The new JSON value.
			 */
			virtual void update( JsonValueInterface& newValue) = 0;
		};

		/*!
		 @class JsonObjectObserver
		 \brief An abstract JSON object observer.
		 Typically we don't care about the json object, but we may have a few listeners at each element.
		 @since 2015-05-12
		 */
		class JsonObjectObserver: public JsonValueObserver, public JsonSubject {
		private:
			void update( JsonValueInterface& newValue);
		};

		/*!
		 @class JsonArrayObserver
		 \brief An abstract JSON array observer.
		 Typically we don't care about the json array, but we may have a few listeners at each element.
		 @since 2015-05-12
		 */
		class JsonArrayObserver: public JsonValueObserver {
		private:
			std::map<int, JsonValueObserver*> jsonArrayValues;
			void update( JsonValueInterface& newValue);

		public:

			/**
			 \brief Adds a new observer to a specific index.
			 \param arrayIndex The array index to be observed.
			 \param observer The observer to be notified upon value detection.
			 */
			void addElementObserver(int arrayIndex, JsonValueObserver* observer);
		};
	}

	/*! \brief Converts the value to an integer, if the type matches.
	 \param storage The location to store the value.  Only set if value is of the correct type.
	 \param value The JSON value to be converted.
	 \return True is success, Faluse if fail.
	 */
	bool setIntValueIfSafe(int* storage, App::JsonValueInterface& value);

	/*! \brief Converts the value to a double, if the type matches.
	 \param storage The location to store the value.  Only set if value is of the correct type.
	 \param value The JSON value to be converted.
	 \return True is success, Faluse if fail.
	 */
	bool setDoubleValueIfSafe(double* storage, App::JsonValueInterface& value);

	/*! \brief Converts the value to a string, if the type matches.
	 \param storage The location to store the value.  Only set if value is of the correct type.
	 \param value The JSON value to be converted.
	 \return True is success, Faluse if fail.
	 */
	bool setStringValueIfSafe(std::string* storage, App::JsonValueInterface& value);
}

#endif
