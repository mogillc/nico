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

#ifndef HEXAPOD_ANDROID_JAVASTATICCLASS_H
#define HEXAPOD_ANDROID_JAVASTATICCLASS_H

#include <jni.h>
#include <string>
#include <map>
#include <cstdarg>

//#include <mogi/port/android/AndroidEnvironment.h>

namespace Mogi {
	namespace Android {
		/**
		 * Calling Java methods from C++ is somewhat cumbersome. This class provides a simple wrapper that
		 * facilitates calling static java methods defined within a Java class.
		 *
		 * Usage ex:
		 *  jstring strArg = env->NewStringUTF("text"); // Argument to Java method
		 *  JavaStaticClass javaClass(env, "mogi/hexapod/gl/GL2JNILib"); // Create class
		 *  javaClass.addMethod("testIntMethod", "(Ljava/lang/String;)I"); // Add a method called "testIntMethod" that takes a Java String and returns an int
		 *
		 *  int result = -1; // Will hold the result of the method call
		 *  javaClass.callMethod(&result, "testIntMethod", strArg); // Call the Java method with the specified params and store return value in result variable.
		 *
		 * TODO: Revise to support Java method overloading!
		 * TODO: Store pointers to JavaMethodInfo objects and implement reference counting
		 * TODO: Implement error checking when re-adding same method
		 * TODO: Implement error checking when callMethod()'s returnVal argument is NULL
		 */
		class JavaStaticClass {
		private:

			/**
			 * Possible return types for Java methods
			 */
			enum ReturnType {
				RETURN_VOID,
				RETURN_BOOLEAN,
				RETURN_BYTE,
				RETURN_CHAR,
				RETURN_SHORT,
				RETURN_INT,
				RETURN_LONG,
				RETURN_FLOAT,
				RETURN_DOUBLE,
				RETURN_OBJECT,
				RETURN_STRING,
				RETURN_ARRAY,
				RETURN_UNKNOWN
			};

			/**
			 * Stores information regarding a Java method's return type
			 */
			struct ReturnTypeInfo {
				ReturnType returnType;
				std::string extraInfo; // Used for Java objects or arrays
			};

			/**
			 * Stores information for a Java method
			 */
			struct JavaMethodInfo {
				jmethodID methodID;
				ReturnTypeInfo returnTypeInfo;
			};

			/**
			 * The Java class's name to which methods will be called.
			 */
			std::string className;

			/**
			 * Handle to the Java class. This is a global reference to 
			 * ensure accessibility in all threads. 
			 * See: https://developer.android.com/training/articles/perf-jni.html 
			 */
			jclass clazz;

			/**
			 * Maps for storing method information
			 */
			std::map<std::string, JavaMethodInfo> methods;


		public:

			/**
			 * Creates a link to an existing Java class so that C++ code can call its (static) methods
			 *
			 * @param className the fully qualified name of an existing Java class (e.g., "mogi/hexapod/gl/GL2JNILib")
			 */
			JavaStaticClass(const std::string& className);

			JavaStaticClass(jclass clazz);

			JavaStaticClass(const std::string& className, jclass clazz);

			~JavaStaticClass();


			/**
			 * Adds a link to one of the class' existing methods so that C++ code can call it later.
			 * Refer to http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
			 * to determine the format of the method name and signature parameters.
			 *
			 * @param methodName the method's name without return type and parameters (e.g., "myMethod")
			 * @param methodSignature specifies the return and parameter types (e.g., "(Ljava/lang/String;)I").
			 */
			void addMethod(const std::string& methodName, const std::string& methodSignature);

			/**
			 * Calls a static Java method within the Java class specified at construction. The user should
			 * first "add" a link to the static Java method via the addMethod() function before calling the method.
			 *
			 * @param returnVal a pointer to the variable that will store the result of the method call (use NULL for void methods)
			 * @param methodName a c-string specifying the name of the method to call
			 * @param ... parameters passed to the Java method
			 */
			void callMethod(void* returnVal, const char* methodName, ...);

			static std::string getJClassName(jclass clazz);

		private:

			/**
			 * Parses a Java method's return type from a method signature.
			 * See http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
			 *
			 * @param methodSignature specifies the return and parameter types (e.g., "(Ljava/lang/String;)I").
			 */
			ReturnTypeInfo parseReturnType(const std::string& methodSignature);
		};

	}; // namespace Android
}; // namespace Mogi
#endif //HEXAPOD_ANDROID_JAVASTATICCLASS_H
