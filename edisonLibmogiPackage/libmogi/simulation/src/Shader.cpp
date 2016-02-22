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

#include "shader.h"

#include "mogi.h"
#include <iostream>
#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBshader::MBshader() {
	g_program = -1;
	textureTracker = 0;
}

MBshader::~MBshader() {
	for (int i = 0; i < uniforms.size(); i++) {
		delete uniforms[i];
	}
	for (int i = 0; i < textureUniforms.size(); i++) {
		delete textureUniforms[i];
	}
}

GLuint MBshader::program() {
	return g_program;
}

int MBshader::initialize(const char *vertShaderPath,
		const char *fragShaderPath) {
	int ret = 0;
	bool everythingIsCool;
	everythingIsCool = loadShaders(&g_program, vertShaderPath, fragShaderPath);
	if (everythingIsCool) {
		everythingIsCool = validateProgram(g_program);
		if (everythingIsCool == false) {
			ret = -2;
		}
	} else {
		ret = -1;
	}
	return ret;
}

void MBshader::enableAttributes() {
	hasAttributes = true;
}

void MBshader::disableAttributes() {
	hasAttributes = false;
}

void MBshader::useProgram() {
	glUseProgram(g_program);

	if (hasAttributes) {
		glEnableVertexAttribArray(glGetAttribLocation(g_program, "position"));
		glVertexAttribPointer(glGetAttribLocation(g_program, "position"), 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(0));
		glEnableVertexAttribArray(glGetAttribLocation(g_program, "normal"));
		glVertexAttribPointer(glGetAttribLocation(g_program, "normal"), 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(12));
		glEnableVertexAttribArray(glGetAttribLocation(g_program, "color"));
		glVertexAttribPointer(glGetAttribLocation(g_program, "color"), 4, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(24));
	} else {
		int vertex = glGetAttribLocation(g_program, "vertex");
		glEnableVertexAttribArray(vertex);
		glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(0));
	}
}

void MBshader::stopProgram() {
	if (hasAttributes) {
		glDisableVertexAttribArray(glGetAttribLocation(g_program, "position"));
		glDisableVertexAttribArray(glGetAttribLocation(g_program, "normal"));
		glDisableVertexAttribArray(glGetAttribLocation(g_program, "color"));
	}
	glUseProgram(0);
}

GLint MBshader::getUniformLocation(std::string name) {
	// Find the uniform, if it exists:
	for (int i = 0; i < uniforms.size(); i++) {
		if (name == uniforms[i]->name) {
			return uniforms[i]->location;
		}
	}

	// Doesn't exist yet, so find and add it:
	MBuniform *uniform = new MBuniform;
	if ((uniform->location = glGetUniformLocation(g_program, name.c_str()))
			>= 0) {
		uniform->name = name;
		uniforms.push_back(uniform);

		return uniform->location;
	}

	return -1;
}

TextureUniform *MBshader::getTextureUniform(std::string name) {
	// Find the uniform, if it exists:
	for (int i = 0; i < textureUniforms.size(); i++) {
		if (name == textureUniforms[i]->name) {
			return textureUniforms[i];
		}
	}

	// Doesn't exist yet, so find and add it:
	TextureUniform *uniform = new TextureUniform;
	if ((uniform->location = glGetUniformLocation(g_program, name.c_str()))
			>= 0) {
		uniform->name = name;
		uniform->value = textureTracker++;
		textureUniforms.push_back(uniform);

		return uniform;
	}

	return NULL;
}

int MBshader::sendInteger(std::string name, int value) {
	GLint location = getUniformLocation(name);

	if (location >= 0) {
		glUniform1i(location, value);
	}

	return location;
}

int MBshader::sendFloat(std::string name, float value) {
	GLint location = getUniformLocation(name);

	if (location >= 0) {
		glUniform1f(location, value);
	}

	return location;
}

int MBshader::sendMatrix(std::string name, Math::Matrix &matrix) {
	GLint location = getUniformLocation(name);

	if (matrix.numColumns() == matrix.numRows()) {
		switch (matrix.numRows()) {
		case 3:
			glUniformMatrix3fv(location, 1, GL_FALSE, matrix.dataAsFloat());
			break;

		case 4:
			glUniformMatrix4fv(location, 1, GL_FALSE, matrix.dataAsFloat());
			break;

		default:
			break;
		}
	} else if (matrix.numColumns() == 1) {
		switch (matrix.numRows()) {
		case 3:
			glUniform3fv(location, 1, matrix.dataAsFloat());
			break;

		case 4:
			glUniform4fv(location, 1, matrix.dataAsFloat());
			break;

		default:
			break;
		}
	}

	return location;
}

int MBshader::sendTexture(std::string name, GLuint texture) {
	TextureUniform *uniform = getTextureUniform(name);

	if (uniform != NULL) {
		glActiveTexture(GL_TEXTURE0 + uniform->value);
		glBindTexture(GL_TEXTURE_2D, texture);

		glUniform1i(uniform->location, uniform->value);

		return 0;
	}

	return -1;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////// SHADERS
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//#pragma mark -  OpenGL ES 2 shader compilation

// bool loadShaders( GLuint *g_program)
//{
//	return loadShaders( g_program, "Shaders/Shader.vsh",
//"Shaders/Shader.fsh");
//}

bool Simulation::loadShaders(GLuint *g_program, const char *vertShaderPath,
		const char *fragShaderPath) {
	GLuint vertShader, fragShader;
	std::string vertShaderPathname, fragShaderPathname;

	// Create shader program.
	// printf("creating program...");
	*g_program = glCreateProgram();

	// printf("Done.\ncreating vertex shader...");
	vertShader = glCreateShader(GL_VERTEX_SHADER);
#ifdef BUILD_FOR_IOS
	vertShaderPathname = Mogi::getResourceDirectory();
	vertShaderPathname.append("/");
	vertShaderPathname.append( vertShaderPath );
	fragShaderPathname = Mogi::getResourceDirectory();
	fragShaderPathname.append("/");
	fragShaderPathname.append( fragShaderPath );
#else
	vertShaderPathname = vertShaderPath;
	fragShaderPathname = fragShaderPath;
#endif
	// vertShader = setShaderSource( vertShaderPathname, GL_VERTEX_SHADER );
	// printf("Done.\ncreating frag shader...");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	// fragShader = setShaderSource( fragShaderPathname, GL_FRAGMENT_SHADER );

	// Create and compile vertex shader.
	// printf("Done.\nSetting up vertex shader:\n");
	if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertShaderPathname)) {
		// glCompileShader( vertShader );
		printf("Failed to compile vertex shader\n");
		return false;
	}

	// Create and compile fragment shader.
	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragShaderPathname)) {
		// glCompileShader( fragShader );
		printf("Failed to compile fragment shader\n");
		return false;
	}

	// Attach vertex shader to program.
	glAttachShader(*g_program, vertShader);

	// Bind attribute locations.
	// This needs to be done prior to linking.
	// glBindAttribLocation(*g_program, ATTRIB_VERTEX, "position");
	// glBindAttribLocation(*g_program, ATTRIB_NORMAL, "normal");
	// glBindAttribLocation(*g_program, ATTRIB_COLOR, "color");

	// Attach fragment shader to program.
	glAttachShader(*g_program, fragShader);

	// Link program.
	if (!linkProgram(*g_program)) {
		printf("Failed to link program: %d\n", *g_program);

		if (vertShader) {
			glDeleteShader(vertShader);
			vertShader = 0;
		}
		if (fragShader) {
			glDeleteShader(fragShader);
			fragShader = 0;
		}
		if (*g_program) {
			glDeleteProgram(*g_program);
			*g_program = 0;
		}

		return false;
	}

	// Release vertex and fragment shaders.
	if (vertShader) {
		glDetachShader(*g_program, vertShader);
		glDeleteShader(vertShader);
	}
	if (fragShader) {
		glDetachShader(*g_program, fragShader);
		glDeleteShader(fragShader);
	}


	return true;
}

bool Simulation::compileShader(GLuint *shader, GLenum type, std::string file) {
	GLint status;
	GLchar *source;
	GLint length;

	// printf("Compiling %s!\n", file.c_str());

	source = read_text_file(file.c_str());
	if (!source) {
		printf("Failed to load vertex shader\n");
		return false;
	}
	// printf("Read file conents:\n%s\n", source);

	*shader = glCreateShader(type);

	length = strlen(source);
	glShaderSource(*shader, 1, (const GLchar **) &source, &length);

	glCompileShader(*shader);

#if defined(DEBUG)
	GLint logLength;
	glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar *log = (GLchar *)malloc(logLength);
		glGetShaderInfoLog(*shader, logLength, &logLength, log);
		printf("Shader compile log:\n%s\n", log);
		free(log);
	}
#endif

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char *log;

		/* get the shader info log */
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &length);
		log = (char *) malloc(length);
		glGetShaderInfoLog(*shader, length, &status, log);

		/* print an error message and the info log */
		fprintf(stderr, "shaderCompileFromFile(): Unable to compile %s: %s\n",
				file.c_str(), log);
		free(log);

		glDeleteShader(*shader);
		return false;
	}

	// printf("Woohoo compiled!\n");
	if (source > 0) {
		free(source);
	}
	return true;
}

bool Simulation::linkProgram(GLuint prog) {
	GLint status;
	glLinkProgram(prog);

#if defined(DEBUG)
	GLint logLength;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar *log = (GLchar *)malloc(logLength);
		glGetProgramInfoLog(prog, logLength, &logLength, log);
		printf("Program link log:\n%s\n", log);
		free(log);
	}
#endif

	glGetProgramiv(prog, GL_LINK_STATUS, &status);
	if (status == 0) {
		return false;
	}

	// printf("linkProgram succesful!\n");
	return true;
}

bool Simulation::validateProgram(GLuint prog) {
	GLint logLength, status;

	glValidateProgram(prog);
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0) {
		GLchar *log = (GLchar *) malloc(logLength);
		glGetProgramInfoLog(prog, logLength, &logLength, log);
		printf("Program validate log:\n%s\n", log);
		free(log);
	}

	glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
	if (status == 0) {
		return false;
	}

	// printf("Program has been validated!\n");
	return true;
}

GLchar *Simulation::read_text_file(const char *name) {
	FILE *fp;
	GLchar *content = NULL;
	int count = 0;

	if (name != NULL) {
		// Attempt to open the file
		// On Windows systems, may want to use "rt" here
		fp = fopen(name, "r");
		if (fp != NULL) {
			// Determine its length
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				// Allocate the string buffer
				content = (GLchar *) malloc(sizeof(char) * (count + 1));
				// Read the file into the buffer
				count = fread(content, sizeof(char), count, fp);
				// Add a NUL terminator
				content[count] = '\0';
			}

			fclose(fp);
		} else {
			perror(name);
		}
	} else {
		fprintf(stderr, "error:  no file name specified\n");
	}

	return content;
}

#ifdef _cplusplus
}
#endif
