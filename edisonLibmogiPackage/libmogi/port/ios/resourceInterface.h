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

#ifndef MOGI_RESOURCE_INTERFACE_H
#define MOGI_RESOURCE_INTERFACE_H

#include <string>
#ifdef BUILD_FOR_IOS
	#include <OpenGLES/gltypes.h>
#elif defined(ANDROID)	// Android?
	#ifdef GL_ES_VERSION_3_0
		#include <GLES3/gl3.h>
	#else
		#include <GLES2/gl2.h>
	#endif
#endif

std::string _getMogiResourceDirectory();

#if defined(BUILD_FOR_IOS) || defined(ANDROID)
GLuint _loadTexture(const char* name, int glslVersion);
#endif



#endif
