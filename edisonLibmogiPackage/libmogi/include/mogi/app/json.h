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

			JsonValueInterface();
			JsonValueInterface(const JsonValueInterface&);
			~JsonValueInterface();

			/*! \brief Parses the string containing a JSON string into the value framework.
			 \param jsonString The JSON string, must be well formed, and begin and end with '{' and '}'
			 \param value The value to store the parsing result.
			 \return 0 on success, -1 on a failure.
			 */
			static int parse( std::string jsonString, JsonValueInterface& value );

			/*! \brief Parses the string containing a JSON string into the value framework.
			 \param beginDoc The first character in the JSON string, should be a '{' character.
			 \param endDoc The last character in the JSON string, should be a '}' character.
			 \param value The value to store the parsing result.
			 \return 0 on success, -1 on a failure.
			 */
			static int parse( const char *beginDoc, const char *endDoc, JsonValueInterface& value );

			/*! \brief Checks if the value is a bool type.
			\return true if a bool, false otherwise.
			 */
			bool isBool() const;

			/*! \brief Checks if the value is an integer type.
			 \return true if an int, false otherwise.
			 */
			bool isInt() const;

			/*! \brief Checks if the value is a double type.
			 \return true if a double, false otherwise.
			 */
			bool isDouble() const;

			/*! \brief Checks if the value is a string type.
			 \return true if a string, false otherwise.
			 */
			bool isString() const;

			/*! \brief Checks if the value is an array type.
			 \return true if an array, false otherwise.
			 */
			bool isArray() const;

			/*! \brief Checks if the value is an object type.
			 \return true if an object, false otherwise.
			 */
			bool isObject() const;

			/*! \brief Returns the bool value if of the correct type.
			 \return The current value.
			 */
			int asBool() const;

			/*! \brief Returns the integer value if of the correct type.
			 \return The current value.
			 */
			int asInt() const;

			/*! \brief Returns the double value if of the correct type.
			 \return The current value.
			 */
			double asDouble() const;

			/*! \brief Returns the string value if of the correct type.
			 \return The current value.
			 */
			std::string asString() const;

			/*! \brief Returns the size of the Array if the value is the correct type..
			 \return The number of elements in the array.
			 */
			int size() const;	// array size, if array

			/*! \brief Returns the reference to the value in the array.  If not an array, the value will become an array.
			 \param index The element index in the array.
			 \return The element value reference.
			 */
			JsonValueInterface& operator[](const unsigned int& index);

			/*! \brief Returns the reference to the value in the object.  If not an object, the value will become an object.
			 \param key The key of the element.
			 \return The element value reference.
			 */
			JsonValueInterface& operator[](const std::string& key);

			/*! \brief Returns all keys if this value is an object.
			 \return All keys in the object.
			 */
			std::vector<std::string> getMemberNames() const;

			/*! \brief Makes the value a bool.
			 \param other The value to be set.
			 */
			JsonValueInterface& operator=( const bool& other );

			/*! \brief Makes the value an int.
			 \param other The value to be set.
			 */
			JsonValueInterface& operator=( const int& other );

			/*! \brief Makes the value a double.
			 \param other The value to be set.
			 */
			JsonValueInterface& operator=( const double& other );

			/*! \brief Makes the value a string.
			 \param other The value to be set.
			 */
			JsonValueInterface& operator=( const std::string& other );

			/*! \brief Copies the value.
			 \param other The value to be set.
			 */
			JsonValueInterface& operator=( JsonValueInterface& other );

			/*! \brief Converts the JSON value to a string, i.e. the opposite of parse().
			 \return The JSON values in a string format.
			 */
			std::string toStyledString() const;

			/*! \brief Returns the raw pointer to the true value handler.
			 \return The location of the true JSON value handler.
			 */
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
