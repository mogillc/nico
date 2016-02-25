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

	GLuint StaticShader::program() {
		return g_program;
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
				return -2;	// TODO: make enums for this
			}
		} else {
			return -1;
		}
		return 0;

	}

#ifdef _cplusplus
}
#endif
