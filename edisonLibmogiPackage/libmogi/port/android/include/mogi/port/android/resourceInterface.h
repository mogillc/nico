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

extern std::string _getMogiResourceDirectory();

#if defined(BUILD_FOR_IOS) || defined(ANDROID)
extern GLuint _loadTexture(const char* name, int glslVersion);
#endif

#endif
