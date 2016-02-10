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

#ifdef LIBJSONCPP_FOUND

#include <map>

#include <json/json.h>  // I would like to remove the dependency eventually

namespace Mogi {

	/*!
	 * \namespace Mogi::App
	 * \brief Tools for communicating with the downloadable app.
	 */
	namespace App {

		// class JsonKeyObserver;
		class JsonValueObserver;

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
				void update(const Json::Value& newValue); // notifies all value observers
			};

		private:
			std::vector<JsonKeyObserver*> keyObserversToRemove; // TODO: change to std::remove
			std::map<std::string, JsonKeyObserver*> keyObservers;

		protected:

			/**
			 \brief Notifies all observers of a value change.
			 \param jsonObject The new value.
			 */
			void notifyObservers(const Json::Value& jsonObject);

		public:
			~JsonSubject();

			/**
			 Given a key, this will create a key observer or append the listener to an
			 existing key observer
			 */
			void addValueObserver(const std::string& key,
								  JsonValueObserver* valueObserver);

			/**
			 Given a key, this will create a key observer or append the listener to an
			 existing key observer
			 */
			void eraseValueObserver(const std::string& key,
									JsonValueObserver* valueObserver);

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
			virtual void update(const Json::Value& newValue) = 0;
		};

		/*!
		 @class JsonObjectObserver
		 \brief An abstract JSON object observer.
		 Typically we don't care about the json object, but we may have a few listeners at each element.
		 @since 2015-05-12
		 */
		class JsonObjectObserver: public JsonValueObserver, public JsonSubject {
		private:
			void update(const Json::Value& newValue);
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
			void update(const Json::Value& newValue);

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
	bool setIntValueIfSafe(int* storage, Json::Value& value);

	/*! \brief Converts the value to a double, if the type matches.
	 \param storage The location to store the value.  Only set if value is of the correct type.
	 \param value The JSON value to be converted.
	 \return True is success, Faluse if fail.
	 */
	bool setDoubleValueIfSafe(double* storage, Json::Value& value);

	/*! \brief Converts the value to a string, if the type matches.
	 \param storage The location to store the value.  Only set if value is of the correct type.
	 \param value The JSON value to be converted.
	 \return True is success, Faluse if fail.
	 */
	bool setStringValueIfSafe(std::string* storage, Json::Value& value);
}

#endif // LIBJSONCPP_FOUND

#endif
