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

	DynamicShader::DynamicShader() {
		parameters = NULL;
	}

	DynamicShader::~DynamicShader() {
		if (parameters != NULL) {
			delete parameters;
		}
	}

	MBshader* DynamicShader::getActualShader() {
		return ShaderFactory::getInstance(parameters);//->actualShader;
	}

	GLuint DynamicShader::program() {
		MBshader* theShader = getActualShader();
		if (theShader == NULL) {
			std::cerr << "Error: DynamicShader::program(): cetActualShader returned NULL" << std::endl;
			return -1;
		}
		return getActualShader()->program();
	}

#ifdef _cplusplus
}
#endif
