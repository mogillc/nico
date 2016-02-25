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

	ShadowShader::ShadowShader() {
		static int instanceCount = 0;
		std::stringstream shaderName("");
		shaderName << "ShadowShader_" << instanceCount++;
//		actualShader->setName(shaderName.str());
//		std::cout << "Generated shader: " << actualShader->getName();
		this->setName(shaderName.str());
	}

	ShadowShader::~ShadowShader() {
	}

	ShadowShaderParameters* ShadowShader::getParameters() {
		if (parameters == NULL) {
			parameters = new ShadowShaderParameters;
		}
		return (ShadowShaderParameters*)parameters;
	}


	int ShadowShader::sendInteger(std::string name, int value) {
		if (name.compare("nLights") == 0) {
			getParameters()->numberOfLights = value;
			return 0;
		} else if(name.compare("colorMapEnable") == 0) {
			getParameters()->colorMapEnableFromMaterial = value;
			return 0;
		} else if(name.compare("normalMapEnable") == 0) {
			getParameters()->normalMapEnableFromMaterial = value;
			return 0;
		} else if(name.compare("specularityMapEnable") == 0) {
			getParameters()->specularMapEnableFromMaterial = value;
			return 0;
		} else if(name.compare("heightMapEnable") == 0) {
			getParameters()->disparityMapEnableFromMaterial = value;
			return 0;
		}

		return MBshader::sendInteger(name, value);
	}

	

#ifdef _cplusplus
}
#endif
