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
// Created by adrian on 2/19/16.
//

#include <mogi/port/android/JavaStaticClass.h>
#include <mogi/port/android/AndroidEnvironment.h>

using namespace Mogi;

std::string Android::JavaStaticClass::getJClassName(jclass clazz) {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();

    jclass javaClassClass = env->FindClass("java/lang/Class");
    jmethodID javaClassNameMethod = env->GetMethodID(javaClassClass, "getName", "()Ljava/lang/String;");

    jstring javaClassNameJString = (jstring)env->CallObjectMethod(clazz, javaClassNameMethod);

    const char* str = env->GetStringUTFChars(javaClassNameJString, NULL);

    std::string theName(str);

    std::size_t found;
    while ((found = theName.find('.')) != std::string::npos) {
        theName[found] = '/';
    }

    env->ReleaseStringUTFChars(javaClassNameJString, str);

	return theName;
}

Android::JavaStaticClass::JavaStaticClass(const std::string &className): 
	className(className) {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();
	this->clazz = reinterpret_cast<jclass>(
					env->NewGlobalRef(env->FindClass(className.c_str()))
	);
}

Android::JavaStaticClass::JavaStaticClass(jclass clazz) {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();
	this->className = Android::JavaStaticClass::getJClassName(clazz);

	this->clazz = reinterpret_cast<jclass>(
					env->NewGlobalRef(clazz)
	);
}

Android::JavaStaticClass::JavaStaticClass(const std::string& className, 
										 jclass clazz) {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();
	this->className = className;

	this->clazz = reinterpret_cast<jclass>(
					env->NewGlobalRef(clazz)
	);
}

Android::JavaStaticClass::~JavaStaticClass() {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();
	env->DeleteGlobalRef(this->clazz);
}

void Android::JavaStaticClass::addMethod(const std::string& methodName, const std::string& methodSignature) {
    JavaMethodInfo methodInfo;
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();

    methodInfo.methodID = env->GetStaticMethodID(this->clazz, methodName.c_str(), methodSignature.c_str());
    methodInfo.returnTypeInfo = this->parseReturnType(methodSignature);


    this->methods[methodName] = methodInfo;
}

void Android::JavaStaticClass::callMethod(void* returnVal, const char* methodName, ...) {
	JNIEnv* env = Android::Environment::getInstance().getJNIEnvironment();
    JavaMethodInfo methodInfo = this->methods[methodName];
    jstring jstr;
    const char* cstr;
    std::string* cppStr;

    va_list args;
    va_start(args, methodName);

    ReturnTypeInfo& returnTypeInfo = methodInfo.returnTypeInfo;
    switch (returnTypeInfo.returnType) {
        case RETURN_VOID:
            env->CallStaticVoidMethodV(this->clazz, methodInfo.methodID, args);
            break;
        case RETURN_BOOLEAN:
            *((bool*)returnVal) = (bool) (env->CallStaticBooleanMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_BYTE:
            *((char*)returnVal) = (char) (env->CallStaticByteMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_CHAR:
            *((char*)returnVal) = (char) (env->CallStaticCharMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_SHORT:
            *((short*)returnVal) = (short) (env->CallStaticShortMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_INT:
            *((int*)returnVal) = (int)(env->CallStaticIntMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_LONG:
            *((long*)returnVal) = (long) (env->CallStaticLongMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_FLOAT:
            *((float*)returnVal) = (float) (env->CallStaticFloatMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_DOUBLE:
            *((double *)returnVal) = (double) (env->CallStaticDoubleMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_STRING:
            jstr = (jstring) env->CallStaticObjectMethodV(this->clazz, methodInfo.methodID, args);
            cstr = env->GetStringUTFChars(jstr, NULL);

            cppStr = (std::string*) returnVal;
            cppStr->clear();
            cppStr->append(cstr);

            env->ReleaseStringUTFChars(jstr, cstr);
            break;
        case RETURN_OBJECT:
            *((jobject*)returnVal) = (jobject) (env->CallStaticObjectMethodV(this->clazz, methodInfo.methodID, args));
            break;
        case RETURN_ARRAY:
            *((jarray*)returnVal) = (jarray) (env->CallStaticObjectMethodV(this->clazz, methodInfo.methodID, args)); // TODO: make sure this is correct and test it out
                                                                                                                           // See: http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/functions.html
            break;
        default:
            break;
    }
    va_end(args);

    return;
}

// See: http://journals.ecs.soton.ac.uk/java/tutorial/native1.1/implementing/method.html
Android::JavaStaticClass::ReturnTypeInfo Android::JavaStaticClass::parseReturnType(const std::string& methodSignature) {
    ReturnTypeInfo result;
    result.returnType = RETURN_UNKNOWN;
    result.extraInfo = "";

    // A method signature looks something like this: "(Ljava/lang/String;)I"
    // The stuff between parentheses specifies the argument types, and the character(s) after the
    // parentheses specifies the return type, which is what we want.

    // First, search for the closing parentheses character ')'
    char findChar = ')';
    std::size_t location = methodSignature.find(findChar);

    // If found, try to get return type
    if (location != std::string::npos) {

        size_t returnTypeIndex = location + 1; // Return type char is after ')'

        // If the signature is well formed, we should be able to get the return type
        if (returnTypeIndex < methodSignature.size()) {

            // get the return type char
            char returnTypeChar = methodSignature[returnTypeIndex];

            // Examine return type char to set the appropriate return value
            switch (returnTypeChar) {
                case 'V': // void
                    result.returnType = RETURN_VOID;
                    break;
                case 'Z': //boolean
                    result.returnType = RETURN_BOOLEAN;
                    break;
                case 'B': // byte
                    result.returnType = RETURN_BYTE;
                    break;
                case 'C': // char
                    result.returnType = RETURN_CHAR;
                    break;
                case 'S': // short
                    result.returnType = RETURN_SHORT;
                    break;
                case 'I': // int
                    result.returnType = RETURN_INT;
                    break;
                case 'J': // long
                    result.returnType = RETURN_LONG;
                    break;
                case 'F': // float
                    result.returnType = RETURN_FLOAT;
                    break;
                case 'D': // double
                    result.returnType = RETURN_DOUBLE;
                    break;
                case 'L': // fully qualified Java class (e.g., Ljava/lang/String;)
                    result.extraInfo = methodSignature.substr(returnTypeIndex);

                    // Java String
                    if (result.extraInfo == "Ljava/lang/String;") {
                        result.returnType = RETURN_STRING;
                    }
                    else { // Other Objects
                        result.returnType = RETURN_OBJECT;
                    }

                    break;
                case '[': // Array of some type (e.g., "[I" is an int[])
                    result.returnType = RETURN_ARRAY; // TODO: implement
                    result.extraInfo = methodSignature.substr(returnTypeIndex);
                    break;
                default:
                    result.returnType = RETURN_UNKNOWN;
                    break;
            }

            return result;

        }
        else {
            return result;
        }
    }
    else {
        return result;
    }

}
