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

	ShadowShader::ShadowShader()  {
		static int instanceCount = 0;
		std::stringstream shaderName("");
		shaderName << "ShadowShader_" << instanceCount++;
		//		actualShader->setName(shaderName.str());
		//		std::cout << "Generated shader: " << actualShader->getName();
		this->setName(shaderName.str());
		//		delete params;
		//		params = new ShadowShaderParameters;
	}

	ShadowShader::~ShadowShader() {
	}

	ShadowShaderParameters* ShadowShader::getParametersMaterial() {
		//		if (params == NULL) {
		//			params = new ShadowShaderParameters;
		//		}
		return (ShadowShaderParameters*)getParameters();
	}


	void ShadowShader::setUniform(const std::string& name, void* value, int index) {
//	int ShadowShader::sendInteger(std::string name, int value) {
		if (name.compare("nLights") == 0) {
			if(getParametersMaterial()->numberOfLights != *(GLint*)value) {
				getParametersMaterial()->numberOfLights = *(GLint*)value;
				getParametersMaterial()->allocateBasedOnLightCount();
			}

			reuseProgram();
			return;
		} else if(name.compare("colorSource") == 0) {
			//			getParameters()->colorMapEnableFromMaterial = value;
			getParametersMaterial()->mColorSource = *(ShadowShaderParameters::ColorSource*)value;
			//			std::cout << "Reconfiguring shader after setting colorSource" << std::endl;
			reuseProgram();
			return;
		} else if(name.compare("normalMapEnable") == 0) {
			getParametersMaterial()->normalMapEnableFromMaterial = *(GLint*)value;
			reuseProgram();
			return;
		} else if(name.compare("specularityMapEnable") == 0) {
			getParametersMaterial()->specularMapEnableFromMaterial = *(GLint*)value;
			reuseProgram();
			return;
		} else if(name.compare("heightMapEnable") == 0) {
			getParametersMaterial()->disparityMapEnableFromMaterial = *(GLint*)value;
			reuseProgram();
			return;
		}

//		getParameters()->
//		setInt(name, value);
		DynamicShader::setUniform(name, value, index);
//		return 0;//MBshader::sendInteger(name, value);
	}

	ShaderParameters* ShadowShader::allocateParameters() {
		std::cout << "In ShadowShader::allocateParameters()" << std::endl;
		if(params == NULL) {
			params = new ShadowShaderParameters;
			params->setParent(this);
		}
		return params;
	}


	
#ifdef _cplusplus
}
#endif
