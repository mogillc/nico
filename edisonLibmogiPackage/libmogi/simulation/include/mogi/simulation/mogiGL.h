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

#ifndef MOGI_GL_H
#define MOGI_GL_H

#ifdef __APPLE__
	#ifdef OPENGLES_FOUND
		#include <OpenGLES/ES3/gl.h>

	#else // OPENGLES_FOUND
		#define GL3_PROTOTYPES 1
		#include <OpenGL/gl3.h>
		//#include <GLUT/glut.h>
	#endif // OPENGLES_FOUND
#else // __APPLE__

	#if defined(ANDROID) && defined(OPENGLES_FOUND) // Android
		#ifdef ARM64_V8A // 64 bit arm 
			#include <GLES3/gl3.h>
			#include <GLES3/gl3ext.h>
		#else // 32 bit arm or others ... ?
			#include <GLES2/gl2.h>
			#include <GLES2/gl2ext.h>
		#endif
	#else // Desktop linux
		#include <GL/glew.h>
	//	#include <string.h>
	//	#include <GL/glut.h>
		//#include <GL/gl.h>
		//#include <GL/glu.h>
		//#include <GL/glext.h>
	//	#include <SDL2/SDL.h>
	//	#include <SDL2/SDL_image.h>
	#endif
#endif // __APPLE__


namespace Mogi {
	namespace Simulation {

		// Singletone class to handle GL information
		class MogiGLInfo {
		private:
			static MogiGLInfo* instance;
			int versionMajor;
			int versionMinor;

			MogiGLInfo();
		public:
			static MogiGLInfo* getInstance();

			int getVersion();

		};
	}
}

#endif
