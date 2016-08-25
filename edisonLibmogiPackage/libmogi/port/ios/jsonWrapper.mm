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

#import "jsonWrapper.h"

#include <iostream>
#include <vector>
#include <string>

@implementation JsonWrapper

#ifdef __cplusplus

namespace _JsonWrapperIOS {

	// This will set the value, and if the parent is an array/object, the value will be swapped to maintain structure:
	void setNSObjectForParent(void** value, NSObject* val, void** parentValue) {
		NSObject* storageLocation = (NSObject*)*value;//CFBridgingRelease(*value);	// transfer to ARC

		if(parentValue == NULL ||
		   *parentValue == NULL) {
//			storageLocation = val;	// No parent, so safe to set:
			*value = (void*)(val);
			return;
		}
		if (isObjectValue(*parentValue) ) {
			for(id key in (NSMutableDictionary*)*parentValue) {
				if([(NSMutableDictionary*)*parentValue objectForKey:key] == *value) {
//					*value = (void*)CFBridgingRelease(val);
//					storageLocation = val;
//					*value = (void*)CFBridgingRetain(storageLocation);
					*value = (void*)(val);	// Will not be deleted by CF, so no transfer to CF
					[(NSMutableDictionary*)*parentValue removeObjectForKey:key];
					[(NSMutableDictionary*)*parentValue setObject:val forKey:key];
					return;
				}
			}
			NSLog(@" ERROR!!! should not be reached in objec!");
			return;
		} else if (isConstArrayValue(*parentValue) && !isArrayValue(*parentValue)) {
			NSLog(@"Const array!");
//			CFBridgingRelease(*parentValue);	// need to release it?
			*parentValue = (void*)([NSMutableArray arrayWithArray:(NSArray*)*parentValue]);
		}
		if (isArrayValue(*parentValue) ) {
			for(int i = 0; i < [(NSMutableArray*)*parentValue count]; i++) {
				if([(NSMutableArray*)*parentValue objectAtIndex:i] == *value) {
//					storageLocation = val;
					//					*value = (void*)CFBridgingRetain(storageLocation);
					*value = (void*)(val);	// Will not be deleted by CF, so no transfer to CF
					[(NSMutableArray*)*parentValue replaceObjectAtIndex:i withObject:val];
					return;
				}
			}
			NSLog(@" ERROR!!! should not be reached in array!");
		}

		*value = (void*)(storageLocation);

	}

	int parseJson( void** value, const std::string& jsonString) {
		NSError *error;
//		CFBridgingRelease(*value);
		*value = (void*)([NSJSONSerialization
										  JSONObjectWithData:[NSData dataWithBytes:jsonString.data()
																			length:jsonString.length()]
										  options:NSJSONReadingMutableContainers|NSJSONReadingMutableLeaves
										  error:&error]);

		if (*value == nil) {
			NSLog(@"Error serializing response data %@ with user info %@.", error, error.userInfo);
			NSLog(@"Data: %s", jsonString.c_str());
			return -1;
		}

		return 0;
	}

	void* getNewJsonValue() {
//		@autoreleasepool {
		NSObject* newObject = [[NSMutableDictionary new] retain];
//		NSLog(@"[newObject retainCount] = %lu", [newObject retainCount]);
//		{
//			NSObject* newObject2 = newObject;
//			NSLog(@"[newObject retainCount] cpy = %lu, newObject retainCount] = %lu", [newObject retainCount], [newObject2 retainCount]);
//		}

//		NSLog(@"[newObject retainCount] after = %lu", [newObject retainCount]);
		void* returnValue = (void*)(newObject);
//		NSLog(@"[newObject retainCount] after CFretain= %lu", [newObject retainCount]);
//		NSLog(@"[(NSObject*)returnValue retainCount] = %lu", [(NSObject*)returnValue retainCount]);
//		newObject = CFBridgingRelease(returnValue);
//		NSLog(@"[newObject retainCount] after CFretain= %lu", [newObject retainCount]);
//		NSLog(@"[(NSObject*)returnValue retainCount] = %lu", [(NSObject*)returnValue retainCount]);
			return returnValue ;
//		}
	}

	void deleteJsonValue( void** value ) {
//		*(NSObject**)value = nil;

//		NSObject* tobereleased = CFBridgingRelease(*value);
//		NSLog(@" - desc: %@", NSStringFromClass ([(NSObject*)(*value) class])  );
//		while([(NSObject*)*value retainCount] > 0)
//		NSUInteger count = [(NSObject*)*value retainCount];
//		while(count > 0) {
//			NSLog(@"REtain count = %ld", (long)[(NSObject*)*value retainCount]);
//			[(NSObject*)(*value) release];
//			count--;
//		}

//		for(int i = 0; i < (long)[tobereleased retainCount]; )
//		{
//			NSLog(@"REtain count = %ld", (long)[(NSObject*)*value retainCount]);
//			tobereleased = CFBridgingRelease(tobereleased);
//		}
//		NSLog(@" - desc: %@", NSStringFromClass ([tobereleased class])  );
		//[tobereleased release];
////		(NSObject*)*value = nil;
//		*(NSObject**)value = nil;
	}

	bool isBoolValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSNumber class]] &&
		strcmp([(NSNumber*)value objCType], @encode(BOOL)) == 0;
	}
	bool isIntValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSNumber class]] &&
		strcmp([(NSNumber*)value objCType], @encode(int)) == 0;
	}
	bool isDoubleValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSNumber class]] &&
		strcmp([(NSNumber*)value objCType], @encode(double)) == 0;
	}
	bool isStringValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSString class]];
	}
	bool isArrayValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSMutableArray class]];
	}
	bool isConstArrayValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSArray class]];
	}
	bool isObjectValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSMutableDictionary class]];
	}

	// If value is not an array, we need to make it one and let the parent know.
	// The storage represents the element child object in the value array.
	void getValueFromIndex(void** storage, void** value, const int& index, void** parentValue) {
		if(!isArrayValue( *value ))	// Not an array, need to make one:
		{
			NSMutableArray* tempArray;
			if(isConstArrayValue(*value) ) {	// Let's convert it to be mutable:
//				CFBridgingRelease(*value);
				tempArray = [NSMutableArray arrayWithArray:(NSArray*)*value];
			} else {
				tempArray = [NSMutableArray new];
			}
//			if(parentValue != NULL)
//			NSLog(@"gatValueFromIndex parent       = %lu", (long)*parentValue);
			setNSObjectForParent(value, tempArray, parentValue);// create the array for the parent
//			if(parentValue != NULL)
//			NSLog(@"gatValueFromIndex parent after = %lu", (long)*parentValue);

		}
		if ([(NSMutableArray*)*value count] < index+1) {	// Allocate proper size:
			for(NSUInteger i = [(NSMutableArray*)*value count]; i < index+1; i++) {
				[(NSMutableArray*)*value addObject:[NSNumber numberWithInt:0]];
			}
		}
//		CFBridgingRelease(*storage);
		*storage = (void*)([(NSMutableArray*)*value objectAtIndex:index]);
		setNSObjectForParent(storage, [(NSMutableArray*)*value objectAtIndex:index], value);
	}
	void getValueFromKey(void** storage, void** value, const std::string& key, void** parentValue) {
		if(!isObjectValue( *value ))	// Not a JSON object, need to make it one:
		{
			setNSObjectForParent(value, [NSMutableDictionary new], parentValue);
		}

		*storage = (NSObject*)[(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
		if(*storage == nil) {	// Need to create a key:
			[(NSMutableDictionary *)*value setObject:[NSMutableDictionary new] forKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
			*storage = (NSObject*)[(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
		}
		NSObject* newValue = [(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
		setNSObjectForParent(storage, newValue, parentValue);
	}

	std::vector<std::string> getKeys( void* value ) {
		std::vector<std::string> result;
		NSMutableDictionary* mValue = (NSMutableDictionary*)value;

		for (id key in [mValue allKeys]) {
			if([key isKindOfClass:[NSString class]]) {
				result.push_back(std::string([(NSString*)key UTF8String]));
			}
		}

		return result;
	}

	bool getBoolValue(const void* value) {
		return [(NSNumber*)value boolValue];
	}
	int getIntValue(const void* value) {
		return [(NSNumber*)value intValue];
	}
	double getDoubleValue(const void* value) {
		return [(NSNumber*)value doubleValue];
	}
	std::string getStringValue(const void* value) {
		return std::string( [(NSString*)value UTF8String] );
	}
	int getArraySize(const void* value) {
		return (int)[(NSMutableArray*)value count];
	}



	void setBoolValue(void** value, const bool& val, void** parentValue) {
		setNSObjectForParent(value, [NSNumber numberWithBool:val], parentValue);
	}
	void setIntValue(void** value, const int& val, void** parentValue) {
		setNSObjectForParent(value, [NSNumber numberWithInt:val], parentValue);
	}
	void setDoubleValue(void** value, const double& val, void** parentValue) {
		setNSObjectForParent(value, [NSNumber numberWithDouble:val], parentValue);
	}
	void setStringValue(void** value, const std::string& val, void** parentValue) {
		setNSObjectForParent(value, [NSString stringWithCString:val.c_str() encoding:NSUTF8StringEncoding], parentValue);
	}
	void setValueValue(void** value, void *const * val, void** parentValue) {
		NSObject* temp = [NSNull new];
		if([(NSObject*)*val isKindOfClass:[NSNumber class]]) {
			temp = (NSNumber*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSString class]]) {
			temp = (NSString*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSMutableArray class]]) {
			temp = (NSMutableArray*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSArray class]]) {
			temp = [NSMutableArray arrayWithArray:(NSArray*)*val];
		}else if([(NSObject*)*val isKindOfClass:[NSMutableDictionary class]]) {
			temp = (NSMutableDictionary*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSNull class]]) {
			*value = (NSNull*)*val;
		} else {
			temp = (NSObject*)*val;
			std::cerr << "ERROR: setValueValue() unrecognized class!" << std::endl;
		}
		*value = (NSObject*)*val;//temp;
//		NSObject* valueToCopy = (NSObject*)*val;
//		setNSObjectForParent(value, valueToCopy, parentValue);
	}

	std::string createJsonString(void* value) {
		NSString* result = nil;
		NSObject* mValue = (NSObject*)value;
		if([mValue isKindOfClass:[NSNumber class]]) {
			result = [(NSNumber*)mValue stringValue];
		} else if([mValue isKindOfClass:[NSString class]]) {
			result = (NSString*)mValue;
		} else if(isArrayValue(value) || isConstArrayValue(value) || isObjectValue(value)) {
//			NSLog(@" class is %@", NSStringFromClass ([mValue class]));
			if([NSJSONSerialization isValidJSONObject:mValue]) {
				NSData *jsonData = [NSJSONSerialization dataWithJSONObject:mValue options:kNilOptions error:nil];
				result = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
			} else {
				NSLog(@"Woah!  not a valid object in createJsonString(void* value)");
			}
		} else if([mValue isKindOfClass:[NSNull class]]) {
			result = @"NULL";
		} else {
			std::cerr << "ERROR: setValueValue() unrecognized class!" << std::endl;
		}

		return std::string([result UTF8String]);
	}
}

#endif

- (int) getInteger
{
	return 1435;
}
@end