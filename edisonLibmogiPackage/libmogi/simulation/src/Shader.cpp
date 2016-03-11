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
#include <cstring>

#ifdef _cplusplus
extern "C" {
#endif

using namespace Mogi;
using namespace Simulation;

MBshader::MBshader() {
	textureTracker = 0;
}

MBshader::~MBshader() {
//	for (int i = 0; i < uniforms.size(); i++) {
//		delete uniforms[i];
//	}
	for (std::map<std::string, GLint*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
		delete it->second;
	}
	for (std::map<std::string, TextureUniform*>::iterator it = textureUniforms.begin(); it != textureUniforms.end(); it++) {
		delete it->second;
	}
}

	GLint MBshader::getAttributeLocation(std::string name) {
		GLint* result = attributes[name];
		if (result == NULL) {
			result = new GLint;
			*result = glGetAttribLocation(this->program(), name.c_str());
			if (*result < 0) {
				std::cout << "Warning, could not find attribute \"" << name << "\" in shader:" << this->label << std::endl;
			}
			attributes[name] = result;
		}
		return *result;
	}

//void MBshader::enableAttributes() {
//	hasAttributes = true;
//}
//
//void MBshader::disableAttributes() {
//	hasAttributes = false;
//}

void MBshader::useProgram() {
	glUseProgram(this->program());

//	if (hasAttributes) {
//		glEnableVertexAttribArray(glGetAttribLocation(this->program(), "position"));
//		glVertexAttribPointer(glGetAttribLocation(this->program(), "position"), 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(0));
//		glEnableVertexAttribArray(glGetAttribLocation(this->program(), "normal"));
//		glVertexAttribPointer(glGetAttribLocation(this->program(), "normal"), 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(12));
//		glEnableVertexAttribArray(glGetAttribLocation(this->program(), "color"));
//		glVertexAttribPointer(glGetAttribLocation(this->program(), "color"), 4, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(24));
//	} else {
//		int vertex = glGetAttribLocation(this->program(), "vertex");
//		glEnableVertexAttribArray(vertex);
//		glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, 40, BUFFER_OFFSET(0));
//	}
}

void MBshader::stopProgram() {
//	if (hasAttributes) {
//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "position"));
//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "normal"));
//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "color"));
//	}
	glUseProgram(0);
}

GLint MBshader::getUniformLocation(std::string name) {
	// Find the uniform, if it exists:
//	for (int i = 0; i < uniforms.size(); i++) {
//		if (name == uniforms[i]->name) {
//			return uniforms[i]->location;
//		}
//	}
	GLint* result = uniforms[name];
	if (result == NULL) {
		result = new GLint;
		//std::cerr << "Finding uniform location " << name << " for shader " << label;
		*result = glGetUniformLocation(this->program(), name.c_str());
		if (*result < 0) {
			std::cerr << " - Warning: MBshader::getUniformLocation(): Unable to find uniform: " << name << " for shader " << label << std::endl;
		}
		uniforms[name] = result;
	}

	return *result;
//	// Doesn't exist yet, so find and add it:
//	MBuniform *uniform = new MBuniform;
//	if ((uniform->location = glGetUniformLocation(g_program, name.c_str())) >= 0) {
//		uniform->name = name;
//		uniforms.push_back(uniform);
//
//		return uniform->location;
//	}
//	delete uniform;
//
//	return -1;
}

TextureUniform* MBshader::getTextureUniform(std::string name) {
	// Find the uniform, if it exists:
	TextureUniform* uniform = textureUniforms[name];

	if (uniform == NULL) {
		TextureUniform *uniform = new TextureUniform;
		uniform->value = textureTracker++;
		uniform->location = glGetUniformLocation(this->program(), name.c_str());
		textureUniforms[name] = uniform;
	}
	return uniform;
}

//	int MBshader::initializeFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
//		if (loadShaders(&g_program, vertexSource, fragmentSource)) {
//			if (validateProgram(g_program) == false) {
//				return -2;	// TODO: make enums for this
//			}
//		} else {
//			return -1;
//		}
//		return 0;
//
//	}

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
		case 2:
			glUniformMatrix2fv(location, 1, GL_FALSE, matrix.dataAsFloat());
			break;

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
		case 2:
			glUniform2fv(location, 1, matrix.dataAsFloat());
			break;

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

	const std::string& MBshader::getName() {
		return label;
	}

	void MBshader::setName(const std::string& newName) {
		label = newName;
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

	bool Simulation::loadShaders(GLuint *g_program, const std::string& vertexSource, const std::string& fragmentSource) {
	GLuint vertShader, fragShader;

	// Create shader program.
	// printf("creating program...");
	*g_program = glCreateProgram();

	// printf("Done.\ncreating vertex shader...");
	vertShader = glCreateShader(GL_VERTEX_SHADER);
	// vertShader = setShaderSource( vertShaderPathname, GL_VERTEX_SHADER );
	// printf("Done.\ncreating frag shader...");

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	// fragShader = setShaderSource( fragShaderPathname, GL_FRAGMENT_SHADER );


	// Create and compile vertex shader.
	// printf("Done.\nSetting up vertex shader:\n");

	if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertexSource)) {
		// glCompileShader( vertShader );
		std::cerr << "Failed to compile vertex shader, source: " << vertexSource << std::endl;
		return false;
	}

	// Create and compile fragment shader.
	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragmentSource)) {
		// glCompileShader( fragShader );
		std::cerr << "Failed to compile fragment shader, source: " << fragmentSource << std::endl;
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

		std::cerr << "Failed to compile vertex shader, source: " << vertexSource << std::endl;
		std::cerr << "Failed to compile fragment shader, source: " << fragmentSource << std::endl;

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

bool Simulation::compileShader(GLuint *shader, GLenum type, const std::string& source) {
	GLint status;

	*shader = glCreateShader(type);

	GLint length = (GLint)source.length();
	const GLchar* sourceString = (const GLchar *)source.c_str(); // Need to be const to compile on linux

	glShaderSource(*shader, 1, &sourceString, &length);

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
		fprintf(stderr, "shaderCompileFromFile(): Unable to compile: %s\n", log);
		free(log);

		glDeleteShader(*shader);
		return false;
	}

	// printf("Woohoo compiled!\n");
//	if (source > 0) {
//		free(source);
//	}
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
			count = (int)ftell(fp);
			rewind(fp);

			if (count > 0) {
				// Allocate the string buffer
				content = (GLchar *) malloc(sizeof(char) * (count + 1));
				// Read the file into the buffer
				count = (int)fread(content, sizeof(char), count, fp);
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
