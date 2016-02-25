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
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>
#include <regex.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	bool ShaderParametersCompare::operator()(const ShaderParameters* left, const ShaderParameters* right) const
	{
		if (typeid(*left) == typeid(*right)) {
			return left->lessThan(right);
		}

		return typeid(*left).before( typeid(*right) );
	}

	bool replace(std::string& str, const std::string& oldString, const std::string& newString);
	void replaceAll(std::string& str, const std::string& oldString, const std::string& newString);

	std::map<ShaderParameters*, MBshader*, ShaderParametersCompare> ShaderFactory::instances = std::map<ShaderParameters*, MBshader*, ShaderParametersCompare>(ShaderParametersCompare());
	int ShaderFactory::instanceCount = 0;

	MBshader* ShaderFactory::getInstance( ShaderParameters* parameters) {
		ShaderParameters* key = parameters->copy();
		MBshader* instance = instances[key];	// need to be careful to not add a dynamic reference here
		if (instance == NULL) {
			instance = create(key);
			instances[key] = instance;
			std::cout << "ShaderFactory instances: " << instances.size()
			<< ", just created shader from : " << abi::__cxa_demangle(typeid(*key).name(), 0, 0, NULL)
			<< std::endl;
		}

		return instance;
	}

	MBshader* ShaderFactory::create(const ShaderParameters* parameters) {
		StaticShader* instance = new StaticShader;
		std::string vertexShaderSource;
		std::string fragmentShaderSource;
		std::map<std::string, std::string> macros;

		getGlobalMacros(macros);

		vertexShaderSource = parameters->getVertexTemplate();
		fragmentShaderSource = parameters->getFragmentTemplate();
		parameters->getMacros(macros);

		for (std::map<std::string, std::string>::iterator it = macros.begin(); it != macros.end(); it++) {
			replaceAll(vertexShaderSource, it->first, it->second);
			replaceAll(fragmentShaderSource, it->first, it->second);
		}

		instance->initializeFromSource(vertexShaderSource, fragmentShaderSource);
		return instance;
	}

	void ShaderFactory::getGlobalMacros(std::map<std::string, std::string>& macros) {

		if (MogiGLInfo::getInstance()->getVersion() >= 300) {
#ifdef OPENGLES_FOUND
			macros["#HEADER"] = "#version 300 es\nprecision highp float;";
#else
			macros["#HEADER"] = "#version 330";
#endif
			macros["#VERTEX_INPUT"] = "in";
			macros["#VERTEX_OUTPUT"] = "out";
			macros["#FRAGMENT_INPUT"] = "in";
			macros["#FRAGMENT_OUTPUT"] = "layout(location = 0) out vec4 FragColor;";
			macros["#FRAGMENT_OUTCOLOR"] = "FragColor";
			macros["#TEXTURE_FUNC"] = "texture";
		} else {
#ifdef OPENGLES_FOUND
			macros["#HEADER"] = "precision highp float;";
#else
			macros["#HEADER"] = "";
#endif

			macros["#VERTEX_INPUT"] = "attribute";
			macros["#VERTEX_OUTPUT"] = "varying";
			macros["#FRAGMENT_INPUT"] = "varying";
			macros["#FRAGMENT_OUTPUT"] = "";
			macros["#FRAGMENT_OUTCOLOR"] = "gl_FragColor";
			macros["#TEXTURE_FUNC"] = "texture2D";
		}

	}

	bool replace(std::string& str, const std::string& from, const std::string& to) {
		size_t start_pos = str.find(from);
		if(start_pos == std::string::npos)
			return false;
		str.replace(start_pos, from.length(), to);
		return true;
	}

	void replaceAll(std::string& str, const std::string& from, const std::string& to) {
		if(from.empty())
			return;
		size_t start_pos = 0;
		while((start_pos = str.find(from, start_pos)) != std::string::npos) {
			str.replace(start_pos, from.length(), to);
			start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
		}
	}


#ifdef _cplusplus
}
#endif
