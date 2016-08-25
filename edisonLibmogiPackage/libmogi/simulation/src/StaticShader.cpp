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


	StaticShader::StaticShader()
	: g_program(-1) {
	}

	void StaticShader::sendAllUniformsToShader() {
		for (std::map<std::string,Uniform*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
			if (it->second != NULL) {
				it->second->sendToShader();
//				std::cout << " - sending:" << it->second->getLocation() << "\tvalue:" << it->second->valueAsString() << "\tname:" << it->second->name << std::endl;
			}
		}
	}

	void StaticShader::initializeUniforms() {
		Uniform::buildUniforms(program(), &uniforms);
		for (std::map<std::string,Uniform*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
			std::cout << "Initialized Uniform ID: " << it->second->getLocation() << "\tname: " << it->first << std::endl;
		}
		std::cout << "!!!!!!!!!!! Initialized " << uniforms.size() << " uniforms!" << std::endl;
	}

	void StaticShader::setUniform(const std::string& name, void* value, int index) {

		//		for (std::map<std::string,Uniform*>::iterator it = uniforms.begin(); it != uniforms.end(); it++) {
		//			std::cout << "Setting Uniform: " << it->first << "\tptr: " << it->second  << std::endl;
		//		}

		if ( uniforms.find(name) != uniforms.end() ) {
			Uniform* uniform = uniforms.at(name);
//			std::cout << "Setting: " << name << "\tFor shader:" << parent->program() << "\tThis:" << this;
			uniform->setValue(value, index);
//			std::cout << "\tTo Value: " << uniform->valueAsString() << std::endl;
		}
//		else {
//			std::cout << "Erasing: " << name << std::endl;
//			uniforms.erase(name);
//			Uniform::buildUniforms(parent->program(), &uniforms);
//		}

	}


	StaticShader* StaticShader::getActiveShader() {
		return this;
	}

	void StaticShader::updateUniforms() {
		sendAllUniformsToShader();
	}

	GLuint StaticShader::program() {
		return g_program;
	}

	ShaderParameters* StaticShader::allocateParameters() {
		ShaderParameters* result = new ShaderParameters;
		result->setParent( this );
		return result;
	}

	GLint StaticShader::getAttributeLocation(std::string name) {
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

	Uniform* StaticShader::getUniform(const std::string& name) {
		std::map<std::string, Uniform*>::iterator result = uniforms.find(name);
		if(result != uniforms.end()) {
			return result->second;
		}
		return NULL;
	}

	int StaticShader::initializeFromPath(const char *vertShaderPath, const char *fragShaderPath) {
		label = vertShaderPath;

		GLchar *sourceVertex = read_text_file(vertShaderPath);
		if (!sourceVertex) {
			std::cerr << "Failed to load vertex shader file: " << vertShaderPath << std::endl;
			return -1;
		}
		std::string vertexSourceString = sourceVertex;

		GLchar *sourceFragment = read_text_file(fragShaderPath);
		if (!sourceFragment) {
			std::cerr << "Failed to load fragment shader file: " << fragShaderPath << std::endl;
			return -1;
		}
		std::string fragmentSourceString = std::string(sourceFragment);

		return initializeFromSource(vertexSourceString, fragmentSourceString);

	}

	int StaticShader::initializeFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
		if (loadShaders(&g_program, vertexSource, fragmentSource)) {
			if (validateProgram(g_program) == false) {
				std::cerr << "Error: Shader Program failed to validate" << std::endl;
				exit(-1);
			}
			initializeUniforms();
		} else {
			std::cerr << "Error: Shader Program failed to compile" << std::endl;
			exit(-1);
		}
		return 0;
	}
	
#ifdef _cplusplus
}
#endif
