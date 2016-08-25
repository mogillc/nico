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

	BokehShader::BokehShader() {
		static int instanceCount = 0;
		std::stringstream shaderName("");
		shaderName << "BokehShader_" << instanceCount++;
//		actualShader->setName(shaderName.str());
//		std::cout << "Generated shader: " << actualShader->getName();
		this->setName(shaderName.str());

	}

	BokehShader::~BokehShader() {
	}

	BokehShaderParameters* BokehShader::getParametersBokeh() {
//		if (params == NULL) {
//			params = new BokehShaderParameters;
//		}
//		return (BokehShaderParameters*)params;
		return (BokehShaderParameters*)getParameters();
	}


	int BokehShader::sendInteger(std::string name, int value) {
		BokehShaderParameters test;
		if (name.compare("autofocus") == 0) {
			getParametersBokeh()->autoFocusEnable = value;
			reuseProgram();
			return 0;
		} else if(name.compare("showFocus") == 0) {
			getParametersBokeh()->debugEnable = value;
			reuseProgram();
			return 0;
		} else if(name.compare("vignettingEnable") == 0) {
			getParametersBokeh()->vignettingEnable = value;
			reuseProgram();
			return 0;
		} else if(name.compare("rings") == 0) {
			getParametersBokeh()->rings = value;
			reuseProgram();
			return 0;
		} else if(name.compare("samples") == 0) {
			getParametersBokeh()->samples = value;
			reuseProgram();
			return 0;
		}

//		getParameters()->
		setInt(name, value);
		return 0;//MBshader::sendInteger(name, value);
	}

	ShaderParameters* BokehShader::allocateParameters() {
		std::cout << "In BokehShader::allocateParameters()" << std::endl;
		if(params == NULL) {
			params = new BokehShaderParameters;
			params->setParent(this);
		}
		return params;
	}
	

#ifdef _cplusplus
}
#endif
