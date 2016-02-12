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

#import "jsonWrapper.h"

#include <iostream>
#include <vector>
#include <string>

@implementation JsonWrapper

#ifdef __cplusplus

namespace _JsonWrapperIOS {

	void setNSObjectForParent(void** value, NSObject* val, void** parentValue) {
		if(parentValue == NULL ||
		   *parentValue == NULL) {
			NSLog(@" - No parent! safe to set!");
			*value = val;
		} else if (isObjectValue(*parentValue) ) {
			NSLog(@" - There exists a parent object...");
			for(id key in (NSMutableDictionary*)*parentValue) {
				NSLog(@" in loop...");
				if([(NSMutableDictionary*)*parentValue objectForKey:key] == *value) {
					NSLog(@" - Found the key: %@", key );
					*value = val;
					[(NSMutableDictionary*)*parentValue removeObjectForKey:key];
					[(NSMutableDictionary*)*parentValue setObject:val forKey:key];
					return;
				}
			}
			NSLog(@" ERROR!!! should no be reached in objec!");
		} else if (isArrayValue(*parentValue) ) {
//			NSLog(@" - This is a part of an array size %lu", (unsigned long)[(NSMutableArray*)*parentValue count]);
//			NSLog(@" - Searching for pointer %lu", (long)*value);
//			NSLog(@" - Index to set: %d", index);
			for(int i = 0; i < [(NSMutableArray*)*parentValue count]; i++) {
				if([(NSMutableArray*)*parentValue objectAtIndex:i] == *value) {
//					NSLog(@" - Found the index: %d", i );
					*value = val;
					[(NSMutableArray*)*parentValue replaceObjectAtIndex:i withObject:val];
//					[(NSMutableArray*)*parentValue removeObjectAtIndex:i];
//					[(NSMutableArray*)*parentValue insertObject:val atIndex:i];
					return;
				}
//				[(NSMutableArray*)*parentValue removeObjectAtIndex:index];
//				[(NSMutableArray*)*parentValue insertObject:val atIndex:index];
			}
			NSLog(@" ERROR!!! should no be reached in array!");
		}

	}

	int parseJson( void** value, const std::string& jsonString) {
		NSError *error;
		*value = (NSMutableDictionary*)[NSJSONSerialization JSONObjectWithData:[NSData dataWithBytes:jsonString.data() length:jsonString.length()] options:NSJSONReadingMutableContainers error:&error];

		if (*value == nil) {
			NSLog(@"Error serializing response data %@ with user info %@.", error, error.userInfo);
			NSLog(@"Data: %s", jsonString.c_str());
			return -1;
		}

		return 0;
	}

	void* getNewJsonValue() {
		return (void*)[NSNull new];
	}

	void deleteJsonValue( void** value ) {
		*(NSObject**)value = nil;
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
	bool isObjectValue(void* value) {
		return [(NSObject*)value isKindOfClass:[NSMutableDictionary class]];
	}


	// If value is not an array, we need to make it one and let the parent know.
	// The storage represents the element child object in the value array.
	void getValueFromIndex(void** storage, void** value, const int& index, void** parentValue) {
		if(!isArrayValue( *value ))
		{
			setNSObjectForParent(value, [NSMutableArray new], parentValue);// create the array for the parent
		}
		if ([(NSMutableArray*)*value count] < index+1) {
			for(NSUInteger i = [(NSMutableArray*)*value count]; i < index+1; i++) {
				[(NSMutableArray*)*value addObject:[NSNumber numberWithInt:0]];
			}
		}
		*storage = [(NSMutableArray*)*value objectAtIndex:index];
		setNSObjectForParent(storage, [(NSMutableArray*)*value objectAtIndex:index], value);
	}
	void getValueFromKey(void** storage, void** value, const std::string& key, void** parentValue) {
		if(!isObjectValue( *value ))
		{
			setNSObjectForParent(value, [NSMutableDictionary new], parentValue);
		}

		*storage = [(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
		if(*storage == nil) {
			NSLog(@" Missing, key, going to allocate one for key %s : %ld", key.c_str(), (long)*storage );
			[(NSMutableDictionary *)*value setObject:[[NSNumber alloc] initWithDouble:0] forKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
			*storage = [(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]];
		}
		setNSObjectForParent(storage, [(NSMutableDictionary *)*value objectForKey:[NSString stringWithCString:key.c_str() encoding:NSUTF8StringEncoding]], value);
	}

	std::vector<std::string> getKeys( void* value ) {
		std::vector<std::string> result;

		for (id key in [(NSMutableDictionary*)value allKeys]) {
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
//		*value = (NSObject*)*val;
		NSObject* temp = [NSNull new];
		if([(NSObject*)*val isKindOfClass:[NSNumber class]]) {
//			std::cerr << "In setValueValue(), should copy:" << getDoubleValue(*val) << std::endl;
			temp = (NSNumber*)*val;
//			std::cerr << " - result:" << getDoubleValue(*value) << std::endl;
		} else if([(NSObject*)*val isKindOfClass:[NSString class]]) {
			temp = (NSString*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSMutableArray class]]) {
			temp = (NSMutableArray*)*val;
		} else if([(NSObject*)*val isKindOfClass:[NSMutableDictionary class]]) {
			temp = (NSMutableDictionary*)*val;
//		} else if([(NSObject*)*val isKindOfClass:[NSNull class]]) {
//			*value = (NSNull*)*val;
		} else {
			std::cerr << "ERROR: setValueValue() unrecognized class!" << std::endl;
		}
		*value = temp;
//		setNSObjectForParent(value, temp, parentValue);
	}

	std::string createJsonString(void* value) {
		NSData *jsonData = [NSJSONSerialization dataWithJSONObject:(NSMutableDictionary*)value options:kNilOptions error:nil];
		NSString* result = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
		return std::string([result UTF8String]);
	}
}

#endif

- (int) getInteger
{
	return 1435;
}
@end