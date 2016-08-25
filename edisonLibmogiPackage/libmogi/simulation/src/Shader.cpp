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
#include "mogiGL.h"
#include "uniform.h"

#include "shader.h"

#include "mogi.h"
#include <iostream>
#include <cstring>

#include "texture.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	MBshader::MBshader()
//	:textureTracker(0), parent(NULL) {
	: parent(NULL) {
		params = NULL;
		hasDynamicParent = false;
	}

	MBshader::~MBshader() {
		//	for (int i = 0; i < uniforms.size(); i++) {
		//		delete uniforms[i];
		//	}
//		for (std::map<std::string, GLint*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
//		for (std::map<std::string, Uniform*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
//			delete it->second;
//		}
//		for (std::map<std::string, TextureUniform*>::iterator it = textureUniforms.begin(); it != textureUniforms.end(); it++) {
//			delete it->second;
//		}

		if(!hasDynamicParent && params != NULL) {
			delete params;
		}
	}

	void MBshader::setExternalParameters( ShaderParameters* newParameters ) {
		if (!hasDynamicParent) {
			if(params != NULL) {
				delete params;
			}
			params = newParameters;
			hasDynamicParent = true;
		}
	}

//	GLint MBshader::getAttributeLocation(std::string name) {
//		GLint* result = attributes[name];
//		if (result == NULL) {
//			result = new GLint;
//			*result = glGetAttribLocation(this->program(), name.c_str());
//			if (*result < 0) {
//				std::cout << "Warning, could not find attribute \"" << name << "\" in shader:" << this->label << std::endl;
//			}
//			attributes[name] = result;
//		}
//		return *result;
//	}

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
		if(checkGLError() != GL_NO_ERROR) {
			exit(-1);
		}
	}

	void MBshader::stopProgram() {
		//	if (hasAttributes) {
		//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "position"));
		//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "normal"));
		//		glDisableVertexAttribArray(glGetAttribLocation(this->program(), "color"));
		//	}
		glUseProgram(0);
		if(checkGLError() != GL_NO_ERROR) {
			exit(-1);
		}

	}

//	void MBshader::updateUniforms() {
//		getParameters()->sendAllUniformsToShader();
//	}

//	GLint MBshader::getUniformLocation(std::string name) {
////		if (getParameters()->uniforms[name] != NULL) {
////			return getParameters()->uniforms[name]->getLocation();
////		}
////		getParameters()->uniforms.erase(name);
////		std::cout << "Couldn't find uniform:" << name << std::endl;
////		getParameters()->initializeUniforms();
//		return -1;
//		/*
//		// Find the uniform, if it exists:
//		//	for (int i = 0; i < uniforms.size(); i++) {
//		//		if (name == uniforms[i]->name) {
//		//			return uniforms[i]->location;
//		//		}
//		//	}
//
//		Uniform* result = uniforms[name];
//		if (result == NULL) {
////			result = new Uniform;
//			result = Uniform::create(name, this->program());
//			//std::cerr << "Finding uniform location " << name << " for shader " << label;
////			*result = glGetUniformLocation(this->program(), name.c_str());
//			if (result == NULL) {
////				std::cerr << " - Warning: MBshader::getUniformLocation(): Unable to find uniform: " << name << " for shader " << label << std::endl;
//			}
//			uniforms[name] = result;
//		}
//
//
////		return *result;
//		return -1;//result->location;
//		//	// Doesn't exist yet, so find and add it:
//		//	MBuniform *uniform = new MBuniform;
//		//	if ((uniform->location = glGetUniformLocation(g_program, name.c_str())) >= 0) {
//		//		uniform->name = name;
//		//		uniforms.push_back(uniform);
//		//
//		//		return uniform->location;
//		//	}
//		//	delete uniform;
//		//
//		//	return -1;
//		 */
//	}

//	TextureUniform* MBshader::getTextureUniform(std::string name) {
//		// Find the uniform, if it exists:
//		TextureUniform* uniform = textureUniforms[name];
//
//		if (uniform == NULL) {
//			TextureUniform *uniform = new TextureUniform;
//			uniform->uniformTextureID = textureTracker++;
//			uniform->location = glGetUniformLocation(this->program(), name.c_str());
//			textureUniforms[name] = uniform;
//			if(checkGLError() != GL_NO_ERROR) {
//				std::cerr << " - Bad shader?" << std::endl;
//				exit(-1);
//			}
//		}
//		return uniform;
//	}

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

	void printUniformInfo(GLint program) {

		GLint i;
		GLint count;

		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		const GLsizei bufSize = 1000; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
		printf("Active Uniforms: %d\n", count);

		for (i = 0; i < count; i++)
		{
			glGetActiveUniform(program, (GLuint)i, bufSize, &length, &size, &type, name);

			const char* strVal = "";
			switch (type) {
				case GL_BOOL:
					strVal = "bool ";
					break;

				case GL_INT:
					strVal = "int  ";
					break;

				case GL_FLOAT:
					strVal = "float";
					break;

				case GL_FLOAT_VEC2:
					strVal = "vec2f";
					break;

				case GL_FLOAT_VEC3:
					strVal = "vec3f";
					break;

				case GL_FLOAT_MAT3:
					strVal = "mat3f";
					break;

				case GL_FLOAT_MAT4:
					strVal = "mat4f";
					break;

				case GL_SAMPLER_2D:
					strVal = "sam2d";
					break;

				default:
					break;
			}

			printf("Uniform #%d\tType: %s\tSize: %d\tName: %s, uLocation: %d\n", i, strVal, size, name, glGetUniformLocation(program, name));
			
		}

	}

		void MBshader::setTexture(const std::string& name, const GLint& value, int index) {
			setUniform(name, (void*)&value, index);
		}
	
		void MBshader::setInt(const std::string& name, const int& value, int index) {
	//		Uniform* uniform = uniforms[name];
	//
	//		if (uniform != NULL) {
	//			uniform->setValue((void*)&value, index);
	//		} else {
	//			uniforms.erase(name);
	//		}
			setUniform(name, (void*)&value, index);
		}
		void MBshader::setFloat(const std::string& name, const float& value, int index) {
	//		Uniform* uniform = uniforms[name];
	//
	//		if (uniform != NULL) {
	//			uniform->setValue((void*)&value, index);
	//		} else {
	//			uniforms.erase(name);
	//		}
			setUniform(name, (void*)&value, index);
		}
		void MBshader::setMatrix(const std::string& name, const Math::Matrix& value, int index) {
	//		Uniform* uniform = uniforms[name];
	//
	//		if (uniform != NULL) {
	//			uniform->setValue((void*)&value, index);
	//		} else {
	//			uniforms.erase(name);
	//		}
			setUniform(name, (void*)&value, index);
		}
//	int MBshader::sendInteger(std::string name, int value) {
//		GLint location = getUniformLocation(name);
//
//		if (location >= 0) {
//			glUniform1iv(location, 1, &value);
//		}
//
//		if (checkGLError() != GL_NO_ERROR) {
//			std::cout << "Shader: " << this->getName() << std::endl;
//			std::cout << "Sending: " << name << " Location: " << location << " Program: " << program() << " tLoc: " << glGetUniformLocation(this->program(), name.c_str()) << std::endl;
//
////			uniforms[name] = NULL;
//
//			exit(-1);
//		}
//
//		return location;
//	}
//
//	int MBshader::sendFloat(std::string name, float value) {
//
//		GLint location = getUniformLocation(name);
//
//		if (location >= 0) {
//			glUniform1fv(location, 1, &value);
//		}
//
//		if (checkGLError() != GL_NO_ERROR) {
//			std::cout << "Shader: " << this->getName() << std::endl;
//			std::cout << "Sending: " << name << " Location: " << location << " Program: " << program() << " tLoc: " << glGetUniformLocation(this->program(), name.c_str()) << std::endl;
//
//			GLint prog;
//			glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
//			std::cout << "\tCurrent program is: " << prog << ", this->program() is:" << this->program() << std::endl;
//
//			printUniformInfo(this->program());
//
////			uniforms[name] = NULL;
//
//			std::cout << "This shader has a parent: " << (parent ? "true" : "false") << std::endl;
//
//			exit(-1);
//		}
//
//		return location;
//	}
//
//	int MBshader::sendFloats(std::string name, float* values, int count) {
//		GLint location = getUniformLocation(name);
//
//		if (location >= 0) {
//			glUniform1fv(location, count, values);
//		}
//
//		if (checkGLError() != GL_NO_ERROR) {
//			std::cout << "Shader: " << this->getName() << std::endl;
//			std::cout << "Sending: " << name << " Location: " << location << " Program: " << program() << " tLoc: " << glGetUniformLocation(this->program(), name.c_str()) << std::endl;
////
////			uniforms[name] = NULL;
//
//			exit(-1);
//		}
//
//		return location;
//	}
//
//	int MBshader::sendMatrix(std::string name, Math::Matrix &matrix) {
//		checkGLError();
//
//		GLint location = getUniformLocation(name);
//
//		if (matrix.numColumns() == matrix.numRows()) {
//			switch (matrix.numRows()) {
//				case 2:
//					glUniformMatrix2fv(location, 1, GL_FALSE, matrix.dataAsFloat());
//					break;
//
//				case 3:
//					glUniformMatrix3fv(location, 1, GL_FALSE, matrix.dataAsFloat());
//					break;
//
//				case 4:
//					glUniformMatrix4fv(location, 1, GL_FALSE, matrix.dataAsFloat());
//					break;
//
//				default:
//					break;
//			}
//		} else if (matrix.numColumns() == 1) {
//			switch (matrix.numRows()) {
//				case 2:
//					glUniform2f(location, matrix.valueLinearIndex(0), matrix.valueLinearIndex(1));
//					//			glUniform1fv(location, 2, matrix.dataAsFloat());
//					//			glUniform2fv(location, 1, matrix.dataAsFloat());
//					break;
//
//				case 3:
//					//			glUniform1fv(location, 3, matrix.dataAsFloat());
//					//			glUniform3fv(location, 1, matrix.dataAsFloat());
//					glUniform3f(location, matrix.valueLinearIndex(0), matrix.valueLinearIndex(1), matrix.valueLinearIndex(2));
//					break;
//
//				case 4:
//					//			glUniform4fv(location, 4, matrix.dataAsFloat());
//					//			glUniform4fv(location, 1, matrix.dataAsFloat());
//					glUniform4f(location, matrix.valueLinearIndex(0), matrix.valueLinearIndex(1), matrix.valueLinearIndex(2), matrix.valueLinearIndex(3));
//					break;
//
//				default:
//					break;
//			}
//		}
//
//
//		if (checkGLError() != GL_NO_ERROR) {
//			std::cout << "Shader: " << this->getName() << std::endl;
//			std::cout << "Sending: " << name << " Location: " << location << " Program: " << program() << " tLoc: " << glGetUniformLocation(this->program(), name.c_str()) << std::endl;
//
////			uniforms[name] = NULL;
//			matrix.print();
//
//			printUniformInfo(this->program());
//			exit(-1);
//		}
//
//		return location;
//	}

//	int MBshader::sendTexture(std::string name, GLuint texture) {
//		TextureUniform *uniform = getTextureUniform(name);
//
//		if (uniform != NULL) {
//			uniform->texture = texture;
//			glActiveTexture(GL_TEXTURE0 + uniform->value);
//			glBindTexture(GL_TEXTURE_2D, uniform->texture);
//
////			glUniform1i(uniform->location, uniform->value);
////			getParameters()->setInt(name, uniform->value);
////			getParameters()->setTexture(name, *uniform);
//			setTexture(name, *uniform);
//
//			if (checkGLError() != GL_NO_ERROR) {
//				std::cout << "Shader: " << this->getName() << std::endl;
////				std::cout << "Sending: " << name << " Location: " << location << " Program: " << program() << " tLoc: " << glGetUniformLocation(this->program(), name.c_str()) << std::endl;
//
////				uniforms[name] = NULL;
//
//				exit(-1);
//			}
//
//			return 0;
//		}
//
//		return -1;
//	}

	const std::string& MBshader::getName() {
		return label;
	}

	void MBshader::setName(const std::string& newName) {
		label = newName;
	}

	MBshader* MBshader::getMainShader() {
		return (parent != NULL) ? parent : this;
	}

	ShaderParameters* MBshader::getParameters() {
		if (params == NULL) {
			params = allocateParameters();
		}
		return params;
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
