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

#include <mogi/port/android/AndroidEnvironment.h>
#include <mogi/Logger.h>

pthread_key_t Mogi::Android::Environment::key_value;
pthread_once_t Mogi::Android::Environment::key_init_once = PTHREAD_ONCE_INIT;

void Mogi::Android::Environment::destroyThreadJNIEnv(void* obj) {
	Mogi::Android::Environment::ThreadJNIEnv* jniEnv = (Mogi::Android::Environment::ThreadJNIEnv*)obj;
	
	if (jniEnv) {
		delete jniEnv;
	}
}

void Mogi::Android::Environment::initKey()
{
    //while you can pass a NULL as the second argument, you 
    //should pass some valid destrutor function that can properly
    //delete a pointer for your MyClass
    pthread_key_create(&key_value, Mogi::Android::Environment::destroyThreadJNIEnv);
}


JNIEnv* Mogi::Android::Environment::getJNIEnvironment() const {

  	//Initialize the key value
	pthread_once(&key_init_once, initKey);

  	//this is where the thread-specific pointer is obtained
  	//if storage has already been allocated, it won't return NULL

  	Mogi::Android::Environment::ThreadJNIEnv *jniEnv = NULL;
  	if ((jniEnv = (Mogi::Android::Environment::ThreadJNIEnv*)pthread_getspecific(key_value)) == NULL) {
    	jniEnv = new Mogi::Android::Environment::ThreadJNIEnv();
    	pthread_setspecific(key_value, (void*)jniEnv);
  	}

  	return jniEnv->env;
}
