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

#import "resourceObject.h"
#import <GLKit/GLKit.h>

@implementation ResourceInterface

#ifdef __cplusplus

std::string _getMogiResourceDirectory()
{
	return std::string([[[NSBundle mainBundle] resourcePath] UTF8String]);
}

GLuint _loadTexture(const char* name)
{
	NSString* filePath = [NSString stringWithCString:name encoding:NSUTF8StringEncoding];
	//NSLog(@"Loading texture: %@", filePath);
	GLKTextureInfo *spriteTexture;
	NSError *theError;
	spriteTexture = [GLKTextureLoader textureWithContentsOfFile:filePath options:nil error:&theError];
	if (spriteTexture == nil) {
		NSLog(@"Error: loadTexture(): %@ ", [theError userInfo] );
	} else {
	//	NSLog(@"target = %d, name = %d", spriteTexture.target, spriteTexture.name);
		glBindTexture(spriteTexture.target, spriteTexture.name);	// TODO: is this needed?
		return spriteTexture.name;
	}
	return -1;
}
#endif

- (int) getInteger
{
	return 1435;
}
@end