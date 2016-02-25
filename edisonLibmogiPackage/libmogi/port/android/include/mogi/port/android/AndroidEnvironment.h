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

//
// Created by Adrian Lizarraga on 2/19/16.
//

#ifndef HEXAPOD_ANDROID_ENVIRONMENT_H
#define HEXAPOD_ANDROID_ENVIRONMENT_H

#include <jni.h>
#include <map>
#include <string>
#include <mogi/port/android/JavaStaticClass.h>

namespace Mogi {
	namespace Android {

		/**
		 * Environment singleton (Scott Meyers' singleton) that holds a reference to the Android JNI environment and other environment variables.
		 */
		class Environment {
		private:
			/**
			 * Handle to the JNI environment
			 */
			JNIEnv* env;

			/**
			 * Path to resources in Android device
			 */
			std::string resourceDirectory;

			std::map<std::string, JavaStaticClass*> javaAPIs;

		public:

			/**
			 * Returns a singleton Environment instance.
			 */
			static Environment& getInstance() {
				static Environment instance; // Guaranteed to be destroyed.
		                                     // Instantiated on first use.

				return instance;
			}	

			/**
			 * Sets the JNI environment handle used by this singleton
			 *
			 * @param env the handle to the JNI environment
			 */			
			void setJNIEnvironment(JNIEnv* env) {
				this->env = env;
			}

			/**
			 * Returns the JNI environment handle used by this singleton
			 */		
			JNIEnv* getJNIEnvironment() const {
				return this->env;
			}

			void setResourceDirectory(std::string path) {
				this->resourceDirectory = path;
			}

			std::string getResourceDirectory() const {
				return this->resourceDirectory;
			}

			/**
			 * Adds a new API (i.e., a Java class with static methods) that the C++ code can get/use to 
			 * communicate with the Java side. IMPORTANT: this object now owns the memory for the api pointer
			 *
			 * @param key the name of the API (e.g., "mogi/hexapod/jni/GL2JNILib")
			 * @param api the C++ wrapper that enables calls to static Java methods within a Java class. This memory is now owned by this object
			 */
			void addJavaAPI(const std::string& key, JavaStaticClass* api) {
				this->javaAPIs[key] = api;
			}

			bool hasAPI(const std::string& key) const {
				return (this->javaAPIs.find(key) != this->javaAPIs.end());
			}

			JavaStaticClass* getJavaAPI(const std::string& key) {
				return (this->javaAPIs[key]);
			}

		private:
			// Private constructors and assignment operator
			Environment() {
			
			}

			~Environment() {
				// delete any API pointers
				std::map<std::string, JavaStaticClass*>::iterator it;
				for (it = this->javaAPIs.begin(); it != this->javaAPIs.end(); it++) {
					JavaStaticClass* api = it->second;

					if (api) {
						delete api;
					}
				}
			}

			Environment(const Environment&);
			Environment& operator=(const Environment&);
			
		};


	};
};

#endif // HEXAPOD_ANDROID_ENVIRONMENT_H
