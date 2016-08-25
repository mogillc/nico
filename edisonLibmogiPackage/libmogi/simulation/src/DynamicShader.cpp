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
#include "dynamicShader.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	DynamicShader::DynamicShader()
	{//: parameters(NULL) {
		params = NULL;
	}

	DynamicShader::~DynamicShader() {
//		if (parameters != NULL) {
//			delete parameters;
//		}
	}

	// This will only set the uniform for the currently attached shader.  If a dynamic parameter changes, this would need to be called again.
	void DynamicShader::setUniform(const std::string& name, void* value, int index) {
		getActualShader()->setUniform(name, value, index);
	}

	void DynamicShader::updateUniforms() {
		getActualShader()->updateUniforms();
	}

	ShaderParametersDynamic* DynamicShader::getParametersDynamic() {
		return (ShaderParametersDynamic*)getParameters();
	}

	MBshader* DynamicShader::getActualShader() {
		StaticShader* trueShader = ShaderFactory::getInstance(getParametersDynamic());
		if (trueShader->parent != this) {
			getParametersDynamic()->configureParameters(trueShader);
		}
		trueShader->parent = this;	// What if there are two dynamic shaders that use the same instance?
		trueShader->setExternalParameters(getParameters());

		return trueShader;
//		return ShaderFactory::getInstance(parameters);//->actualShader;
	}

	StaticShader* DynamicShader::getActiveShader() {
		return getActualShader()->getActiveShader();
	}

	GLuint DynamicShader::program() {
		MBshader* theShader = getActualShader();
		if (theShader == NULL) {
			std::cerr << "Error: DynamicShader::program(): cetActualShader returned NULL" << std::endl;
			return -1;
		}
		return theShader->program();
	}

	void DynamicShader::reuseProgram() {
		if( !ShaderFactory::instanceExists(getParametersDynamic()) ) {
			this->stopProgram();
			this->useProgram();
//			attributes.clear();	// All prior attributes are now invalid
//			uniforms.clear();
//			textureUniforms.clear();
			// TODO: Resend values?
			std::cout << "!!!!!!!!! Reusing program!" << std::endl;
		} else {
			this->stopProgram();
			this->useProgram();
		}
	}

	GLint DynamicShader::getAttributeLocation(std::string name) {
//		std::cout << "In DynamicShader::getAttributeLocation(std::string name)" << std::endl;
		return getActualShader()->getAttributeLocation(name);
	}

//	GLint DynamicShader::getUniformLocation(std::string name) {
////		std::cout << "In DynamicShader::getAttributeLocation(std::string name)" << std::endl;
//		return getActualShader()->getUniformLocation(name);
//	}

//	TextureUniform* DynamicShader::getTextureUniform(std::string name) {
////		std::cout << "In DynamicShader::getTextureLocation(std::string name)" << std::endl;
//		return getActualShader()->getTextureUniform(name);
//
//	}

#ifdef _cplusplus
}
#endif
