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

//#include <jni.h>
#include <mogi/port/android/resourceInterface.h>
#include <mogi/port/android/AndroidEnvironment.h>
#include <mogi/Logger.h>



std::string _getMogiResourceDirectory() {
	return Mogi::Android::Environment::getInstance().getResourceDirectory();
}

GLuint _loadTexture(const char* name, int glslVersion) {
	/*JNIEnv* env = Mogi::Android::Environment::getInstance().getJNIEnvironment();

	if (env) {
		jstring jtextureName = env->NewStringUTF(name); // convert the asset's name 

		Mogi::Android::JavaStaticClass javaStaticClass(env, "mogi/hexapod/jni/GL2JNILib");
		javaStaticClass.addMethod("testLoadTexture", "(Ljava/lang/String;)I");

		int textureID = -1;
		javaStaticClass.callMethod(&textureID, "testLoadTexture", jtextureName);

		return textureID;
	}

	return -1;*/
Mogi::Logger& logger = Mogi::Logger::getInstance();
	logger << "loading texture " << name << std::endl;
	Mogi::Android::Environment& androidEnv = Mogi::Android::Environment::getInstance();
	JNIEnv* env = androidEnv.getJNIEnvironment();
	std::string apiName = "mogi/hexapod/jni/gl/TextureLoader";
	int textureID = -1;

	if (androidEnv.hasAPI(apiName)) {
		logger << "using loadTexture API ... " << std::endl;
		// Get reference to the texture loading api 
		Mogi::Android::JavaStaticClass* textureLoaderAPI = androidEnv.getJavaAPI(apiName);

		if (!textureLoaderAPI) {
			logger << "Invalid textureLoaderAPI" << std::endl;
		}
		logger << "converting asset's name" << std::endl;
		jstring jtextureName = env->NewStringUTF(name); // convert the asset's name 

		logger << "calling method" << std::endl;
		textureLoaderAPI->callMethod(&textureID, "loadTexture", jtextureName, glslVersion); // Call load method
	}

	logger << "loaded texture with id: " << textureID << std::endl;
	return textureID;
}
