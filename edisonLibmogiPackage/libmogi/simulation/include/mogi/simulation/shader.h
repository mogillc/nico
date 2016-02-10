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

#ifndef MOGI_SHADER_H
#define MOGI_SHADER_H

#ifdef __APPLE__
//#define __gl_h_
//#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef __APPLE__
	#ifdef OPENGLES_FOUND
		#include <OpenGLES/ES3/gl.h>

	#else // OPENGLES_FOUND
		#define GL3_PROTOTYPES 1
		#include <OpenGL/gl3.h>
		#include <GLUT/glut.h>
	#endif // OPENGLES_FOUND
#else // __APPLE__

	#include <GL/glew.h>
	#include <string.h>
	//#include <GL/glut.h>
	//#include <GL/gl.h>
	//#include <GL/glu.h>
	//#include <GL/glext.h>
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_image.h>
#endif // __APPLE__

#include "mogi/math/mmath.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace Mogi {
namespace Simulation {

extern GLint uniforms[];

struct MBuniform {
	std::string name;
	GLint location;
};

struct TextureUniform {
	std::string name;
	GLint location;
	GLint value;
};

bool loadShaders(GLuint *g_program, const char *vertShaderPath,
		const char *fragShaderPath);
// bool loadShaders( GLuint *g_program );
bool compileShader(GLuint *shader, GLenum type, std::string file);
bool linkProgram(GLuint prog);
bool validateProgram(GLuint prog);
GLchar *read_text_file(const char *name);

class MBshader {
private:
	GLuint g_program;
	bool hasAttributes;

	std::vector<MBuniform *> uniforms;
	std::vector<TextureUniform *> textureUniforms;
	int textureTracker;

	GLint getUniformLocation(std::string name);
	TextureUniform *getTextureUniform(std::string name);

public:
	MBshader();
	~MBshader();
	MBshader &operator=(const MBshader &param);
	MBshader(const MBshader &param);

	GLuint program();
	// int initialize();
	int initialize(const char *vertShaderPath, const char *fragShaderPath);
	void useProgram();
	void stopProgram();
	void enableAttributes();
	void disableAttributes();

	int sendInteger(std::string name, int value);
	int sendFloat(std::string name, float value);
	int sendMatrix(std::string name, Math::Matrix &matrix);
	int sendTexture(std::string name, GLuint texture);
};
}
}

#endif
