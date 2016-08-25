/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                          Author: Adrian Lizarraga                          *
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

//
// Created by Adrian Lizarraga on 2/19/16.
//

#ifndef HEXAPOD_ANDROID_ENVIRONMENT_H
#define HEXAPOD_ANDROID_ENVIRONMENT_H

#include <jni.h>
#include <pthread.h>
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
			 * Handle to the Java VM
			 */
			JavaVM *vm;

			/**
			 * Path to resources in Android device
			 */
			std::string resourceDirectory;

			static pthread_key_t key_value;
			static pthread_once_t key_init_once;

			std::map<std::string, JavaStaticClass*> javaAPIs;

			class ThreadJNIEnv {
			public:
				bool _detach;
				JNIEnv *env;

				ThreadJNIEnv() {
					JavaVM* vm = Environment::getInstance().getJavaVM();

					int status = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
					if (status < 0) {
						vm->AttachCurrentThread(&env, NULL);
						_detach = true;
					}
					else {
						_detach = false;
					}
				}
			 
				ThreadJNIEnv(JNIEnv *e) {
					env = e;
					_detach = false;
				}
			 
				~ThreadJNIEnv() {
					if (_detach) {
						JavaVM* vm = Environment::getInstance().getJavaVM();
						vm->DetachCurrentThread();
					}
				}
			};	

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
			 * Sets the Java VM handle used by this singleton
			 *
			 * @param vm the handle to the Java VM
			 */	
			void setJavaVM(JavaVM* vm) {
				this->vm = vm;

				cleanup();
			}

			JavaVM* getJavaVM() const {
				return this->vm;
			}

			/**
			 * Returns the JNI environment handle used by this singleton
			 */	
			JNIEnv* getJNIEnvironment() const;	

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
			Environment(): vm(0) {
			
			}

			~Environment() {
				cleanup();
			}

			void cleanup() {
				// delete any API pointers
				std::map<std::string, JavaStaticClass*>::iterator it;
				for (it = this->javaAPIs.begin(); it != this->javaAPIs.end(); it++) {
					JavaStaticClass* api = it->second;

					if (api) {
						delete api;
					}
				}

				javaAPIs.clear();
			}

			static void initKey();
			static void destroyThreadJNIEnv(void* obj);

			Environment(const Environment&);
			Environment& operator=(const Environment&);
			
		};


	};
};

#endif // HEXAPOD_ANDROID_ENVIRONMENT_H
