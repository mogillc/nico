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

	const char* DeferredLightingShaderParameters::getVertexTemplate() const {
		return "// Generated: DeferredLightingShaderParameters::getVertexTemplate()\n"
		"#HEADER\n"
		"\n"
		"layout(location=0) in vec3 position;\n"
		"\n"
		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat4 biasedModelViewProjectionMatrix;\n"
		"uniform float uTanHalfFov;\n"
		"uniform float uAspectRatio;\n"
		"\n"
		"smooth out vec2 TexCoord;\n"
		"//smooth out vec3 positionOut;\n"
		"noperspective out vec3 viewRay;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	// Do nothing.\n"
		"	vec3 positionOut = (modelViewProjectionMatrix*vec4(position,1.0)).xyz;\n"
		"	TexCoord = (biasedModelViewProjectionMatrix*vec4(position,1.0)).st;\n"
		"\n"
		"	viewRay = vec3(\n"
		"					positionOut.x * uTanHalfFov * uAspectRatio,\n"
		"					positionOut.y * uTanHalfFov,\n"
		"					1.0\n"
		"					);\n"
		"\n"
		"	gl_Position = vec4(positionOut, 1.0);\n"
		"}\n";
	}

	const char* DeferredLightingShaderParameters::getFragmentTemplate() const {
		return "// Generated: DeferredLightingShaderParameters::getFragmentTemplate()\n"
		"#HEADER\n"
		"\n"
		"uniform sampler2D uGBufferDiffuseTex;\n"
		"uniform sampler2D uGBufferMaterialTex;\n"
		"uniform sampler2D uGBufferGeometricTex;	// normals\n"
		"uniform sampler2D uGBufferDepthTex;\n"
		"\n"
		"\n"
		"smooth in vec2 TexCoord;\n"
		"//smooth in vec3 positionOut;\n"
		"noperspective in vec3 viewRay;\n"
		"\n"
		"layout(location = 0) out vec4 FragColor;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	// Diffuse:\n"
		"	vec4 kd = texture(uGBufferDiffuseTex, TexCoord);\n"
		"\n"
		"	// Material:\n"
		"	vec4 material = texture(uGBufferMaterialTex, TexCoord);\n"
		"	float ks = material.r;\n"
		"	float ke = material.g;\n"
		"	float km = material.b * 0.5;\n"
		"\n"
		"	// Geometry:\n"
		"	vec4 geometric = texture(uGBufferGeometricTex, TexCoord);\n"
		"	vec3 normal = geometric.xyz * 2.0 - 1.0;\n"
		"	float ko = geometric.a; // local occlusion\n"
		"\n"
		"	// depth doesn't need to be linear in this case\n"
		"	vec3 position = viewRay * texture(uGBufferDepthTex, TexCoord).r;\n"
		"\n"
		"	vec4 ld = vec4(1.0);//textureLod(uEnvTex, (uInverseViewMatrix * vec4(n, 0.0)).xyz, uEnvTexMaxLod);\n"
		"\n"
		"	vec3 r = reflect(position, normal);\n"
		"	float lod = pow(1.0 - ke, 0.5); // linearize, since mips aren't linearly spaced\n"
		"	vec4 ls = vec4(1.0);//textureLod(uEnvTex, (uInverseViewMatrix * vec4(r, 0.0)).xyz, uEnvTexMaxLod * lod);\n"
		"\n"
		"	vec4 intensity = mix(kd * ld + ks * ls, ks * kd * ls, km) * ko;\n"
		"\n"
		"	FragColor = intensity;\n"
		"}\n";
	}

	const void DeferredLightingShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

//#ifndef OPENGLES_FOUND
#ifndef GL_ES_VERSION_2_0 // also defined for 3.0
		macros["#HEADER"] = "#version 410";
#endif

		// Not a good way to handle this:
		macros["smooth "] = "";

	}

	const bool DeferredLightingShaderParameters::lessThan(const ShaderParametersDynamic* right) const {
		return false; // always equal?
	}

	ShaderParametersDynamic* DeferredLightingShaderParameters::copy() const {
		DeferredLightingShaderParameters* result = new DeferredLightingShaderParameters;
		*result = *this;
		return result;
	}

#ifdef _cplusplus
}
#endif
