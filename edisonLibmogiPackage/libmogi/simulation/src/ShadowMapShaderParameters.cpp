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
#include <sstream>
#include <iomanip>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	const char* ShadowMapShaderParameters::getVertexTemplate() const {
		return "#HEADER\n"
		"#VERTEX_INPUT vec3 position;\n"
		"uniform mat4 modelViewProjectionMatrix;\n"
		"\n"
		"void main() {\n"
		"	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);\n"
		"}\n";
	}

	const char* ShadowMapShaderParameters::getFragmentTemplate() const {
		return "#HEADER\n"
		"\n"
		"#FRAGMENT_OUTPUT\n"
		"\n"
		"void main() {\n"
		"	#FRAGMENT_OUTCOLOR = vec4(1.0,0.0,1.0,1.0);\n"
		"}";
	}

	const void ShadowMapShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

	}

	const bool ShadowMapShaderParameters::lessThan(const ShaderParameters* right) const {
		return false; // always equal?
	}

	ShaderParameters* ShadowMapShaderParameters::copy() const {
		ShadowMapShaderParameters* result = new ShadowMapShaderParameters;
		*result = *this;
		return result;
	}

#ifdef _cplusplus
}
#endif
