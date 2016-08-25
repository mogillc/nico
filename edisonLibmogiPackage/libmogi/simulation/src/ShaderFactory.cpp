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
#include "shader.h"
#include "dynamicShader.h"

#include <string>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <regex.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	bool ShaderParametersCompare::operator()(const ShaderParametersDynamic* left, const ShaderParametersDynamic* right) const
	{
		if (typeid(*left) == typeid(*right)) {
			return left->lessThan(right);
		}

		return typeid(*left).before( typeid(*right) );
	}

	

	std::map<ShaderParametersDynamic*, StaticShader*, ShaderParametersCompare> ShaderFactory::instances = std::map<ShaderParametersDynamic*, StaticShader*, ShaderParametersCompare>(ShaderParametersCompare());
	int ShaderFactory::instanceCount = 0;

	StaticShader* ShaderFactory::getInstance( ShaderParametersDynamic* parameters) {
		ShaderParametersDynamic* key = parameters->copy();
		StaticShader* instance = instances[key];	// need to be careful to not add a dynamic reference here
		if (instance == NULL) {
			instance = create(key);
			instances[key] = instance;
			std::cout << "ShaderFactory instances: " << instances.size()
			<< ", just created shader from : " << abi::__cxa_demangle(typeid(*key).name(), 0, 0, NULL)
			<< std::endl;
		}

		return instance;
	}

	StaticShader* ShaderFactory::create(const ShaderParametersDynamic* parameters) {
		StaticShader* instance = new StaticShader;

		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::map<std::string, std::string> macros;

		getGlobalMacros(macros);

		vertexShaderSource = parameters->getVertexTemplate();
		fragmentShaderSource = ((ShaderParametersDynamic*)parameters)->getFragmentTemplate();
		((ShaderParametersDynamic*)parameters)->getMacros(macros);

		for (std::map<std::string, std::string>::iterator it = macros.begin(); it != macros.end(); it++) {
			replaceAll(vertexShaderSource, it->first, it->second);
			replaceAll(fragmentShaderSource, it->first, it->second);
		}
//		std::cout << vertexShaderSource;
//		std::cout << fragmentShaderSource;

		instance->initializeFromSource(vertexShaderSource, fragmentShaderSource);
		return instance;
	}

	void ShaderFactory::getGlobalMacros(std::map<std::string, std::string>& macros) {

		std::stringstream header("");

		if (MogiGLInfo::getInstance()->getVersion() >= 130) {
			header << "#version " << MogiGLInfo::getInstance()->getGLSLVersionStr();
			if(MogiGLInfo::getInstance()->isGLES()) {
				 header << " es\nprecision highp float;";
			}
			macros["#HEADER"] = header.str();

			macros["#VERTEX_INPUT"] = "in";
			macros["#VERTEX_OUTPUT"] = "out";
			macros["#FRAGMENT_INPUT"] = "in";
			macros["#FRAGMENT_OUTPUT"] = "layout(location = 0) out vec4 FragColor;";
			macros["#FRAGMENT_OUTCOLOR"] = "FragColor";
			macros["#TEXTURE_FUNC"] = "texture";
		} else {
			//header << "#version " << MogiGLInfo::getInstance()->getGLSLVersionStr();
			if(MogiGLInfo::getInstance()->isGLES()) {
				header << "precision highp float;";
			}
			macros["#HEADER"] = header.str();

			macros["#VERTEX_INPUT"] = "attribute";
			macros["#VERTEX_OUTPUT"] = "varying";
			macros["#FRAGMENT_INPUT"] = "varying";
			macros["#FRAGMENT_OUTPUT"] = "";
			macros["#FRAGMENT_OUTCOLOR"] = "gl_FragColor";
			macros["#TEXTURE_FUNC"] = "texture2D";
		}

	}

	bool ShaderFactory::instanceExists(ShaderParametersDynamic* parameters) {
		ShaderParametersDynamic* key = parameters->copy();
//		MBshader* instance = instances[key];	// need to be careful to not add a dynamic reference here

		return instances[key] != NULL;
	}



#ifdef _cplusplus
}
#endif
