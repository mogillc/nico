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

	const char* FinalProcessShaderParameters::getVertexTemplate() const {
		return "// Generated: FinalProcessShaderParameters::getVertexTemplate()\n"
		"#HEADER\n"
		"\n"
		"#VERTEX_INPUT vec3 position;\n"
		"\n"
		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 biasedModelViewProjectionMatrix;\n"
		"\n"
		"#VERTEX_OUTPUT vec4 TexCoord[1];\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position=modelViewProjectionMatrix*vec4(position,1.0);\n"
		"    TexCoord[0]=biasedModelViewProjectionMatrix*vec4(position,1.0);\n"
		"}\n"
		;
	}

	const char* FinalProcessShaderParameters::getFragmentTemplate() const {
		return "// Generated: FinalProcessShaderParameters::getFragmentTemplate()\n"
		"#HEADER\n"
		"\n"
		"//uniform sampler2D bgl_RenderedTexture;\n"
		"//uniform sampler2D bgl_DepthTexture;\n"
		"uniform highp sampler2D bgl_RenderedTexture;\n"
		"uniform highp sampler2D bgl_DepthTexture;\n"
		"uniform float bgl_RenderedTextureWidth;\n"
		"uniform float bgl_RenderedTextureHeight;\n"
		"\n"
		"#FRAGMENT_INPUT vec4 TexCoord[1];\n"
		"\n"
		"#FRAGMENT_OUTPUT\n"
		"\n"
		"void main()\n"
		"{\n"
		"    #FRAGMENT_OUTCOLOR = #TEXTURE_FUNC(bgl_RenderedTexture, TexCoord[0].st);\n"
		"    //#FRAGMENT_OUTCOLOR = vec4(1.0,1.0,0.0,1.0);\n"
		"}\n";
	}

	const void FinalProcessShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

	}

	const bool FinalProcessShaderParameters::lessThan(const ShaderParametersDynamic* right) const {
		return false; // always equal?
	}

	ShaderParametersDynamic* FinalProcessShaderParameters::copy() const {
		FinalProcessShaderParameters* result = new FinalProcessShaderParameters;
		*result = *this;
		return result;
	}

#ifdef _cplusplus
}
#endif
