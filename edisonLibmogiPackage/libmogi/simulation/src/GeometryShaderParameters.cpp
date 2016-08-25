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

	const char* GeometryShaderParameters::getVertexTemplate() const {
		return "// Generated: GeometryShaderParameters::getVertexTemplate()\n"
		"#HEADER\n"
		"// Very simple shader for shadowmapping:\n"
		"\n"
		"#VERTEX_INPUT vec3 position;\n"
		"#VERTEX_INPUT vec3 normal;\n"
		"#VERTEX_INPUT vec3 tangent;\n"
//		"#VERTEX_INPUT vec3 bitangent;\n"
		"#VERTEX_INPUT vec3 color;\n"
		"#VERTEX_INPUT vec2 UV;\n"
		"\n"
		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform mat3 normalMatrix;\n"
		"uniform mat4 modelViewMatrix;\n"
		"//uniform mat4 viewMatrix;\n"
		"\n"
		"#VERTEX_OUTPUT vec3 Normal0;\n"
		"#VERTEX_OUTPUT vec3 outColor;\n"
		"#VERTEX_OUTPUT vec2 TexCoord0;\n"
		"#VERTEX_OUTPUT vec3 fposition;\n"
		"\n"
		"//smooth out vec3 Normal0;\n"
		"//smooth out vec3 outColor;\n"
		"//smooth out vec2 TexCoord0;\n"
		"//smooth out vec3 fposition;\n"
		"//\n"
		"#VERTEX_OUTPUT mat3 TBN;\n"
		"//smooth out mat3 TBN;\n"
		"\n"
		"void main() {\n"
		"	fposition = vec3( modelViewMatrix * vec4( position, 1.0));\n"
		"	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);\n"
		"	Normal0 = (modelViewMatrix*vec4(normal, 0.0)).xyz;// * 0.5 + 0.5;\n"
		"\n"
		"	outColor = color;\n"
		"	TexCoord0 = UV;\n"
		"\n"
		"\n"
		"	vec3 n = normalize( (modelViewMatrix * vec4(normal, 0.0)).xyz);\n"
		"	vec3 t = normalize( (modelViewMatrix * vec4(tangent, 0.0)).xyz);\n"
		"	//n=normalize(  normalMatrix*normal );\n"
		"	vec3 b = cross( t, n);\n"
		"	TBN = mat3( t, b, n);\n"
		"}\n";
	}

	const char* GeometryShaderParameters::getFragmentTemplate() const {
		return "// Generated: GeometryShaderParameters::getFragmentTemplate()\n"
		"#HEADER\n"
		"\n"
		"// Simple fragment shader for shadowmapping:\n"
		"uniform sampler2D normalMap[10];\n"
		"uniform bool normalMapEnable;\n"
		"uniform sampler2D heightMap[10];\n"
		"uniform bool heightMapEnable;\n"
		"uniform sampler2D specularityMap[10];\n"
		"uniform bool specularityMapEnable;\n"
		"uniform sampler2D colorMap[10];\n"
		"uniform bool colorMapEnable;\n"
		"\n"
		"//	surface properties:\n"
		"//uniform vec3 uDiffuseColor = vec3(1.0);\n"
		"//uniform float uSpecularLevel = 1.0;\n"
		"//uniform float uSpecularExponent = 1.0;\n"
		"//uniform float uMetallicLevel = 1.0;\n"
		"//uniform vec2 uDetailTexScale = vec2(1.0);\n"
		"\n"
		"vec3 uDiffuseColor = vec3(1.0);	// Bleh, defaults not allowed in GLES glsl I guess?\n"
		"float uSpecularLevel = 1.0;\n"
		"float uSpecularExponent = 1.0;\n"
		"float uMetallicLevel = 1.0;\n"
		"vec2 uDetailTexScale = vec2(1.0);\n"
		"\n"
		"#FRAGMENT_INPUT vec3 Normal0;\n"
		"#FRAGMENT_INPUT vec3 outColor;\n"
		"#FRAGMENT_INPUT vec2 TexCoord0;\n"
		"#FRAGMENT_INPUT mat3 TBN;\n"
		"#FRAGMENT_INPUT vec3 fposition;\n"
		"\n"
		"//smooth in vec3 Normal0;\n"
		"//smooth in vec3 outColor;\n"
		"//smooth in vec2 TexCoord0;\n"
		"//smooth in mat3 TBN;\n"
		"//smooth in vec3 fposition;\n"
		"\n"
		"//varying vec3 Normal0;\n"
		"//varying vec3 outColor;\n"
		"//varying vec2 TexCoord0;\n"
		"//varying mat3 TBN;\n"
		"//varying vec3 fposition;\n"
		"\n"
		"#FRAGMENT_OUTPUT"
		"\n"
		"void main() {\n"
		"   PositionOut = vec4(fposition, 1.0);\n"
		"\n"
		"	// Height map should be determined first for new texture coordinates:\n"
		"	vec2 newCoords;\n"
		"	vec3 tangentSurface2view = TBN * -fposition;\n"
		"	vec3 eye = normalize(tangentSurface2view);\n"
		"	if (heightMapEnable) {\n"
		"		float height = #TEXTURE_FUNC(heightMap[0], TexCoord0).r;\n"
		"		float v = height * 0.04 - 0.02;\n"
		"\n"
		"		newCoords = TexCoord0 - (eye.xy * v);\n"
		"	} else {\n"
		"		newCoords = TexCoord0;//2.st;\n"
		"	}\n"
		"\n"
		"	// Diffuse:\n"
		"	if (colorMapEnable) {\n"
		"		#FRAGMENT_OUTCOLOR = #TEXTURE_FUNC( colorMap[0], newCoords);\n"
		"	} else {\n"
		"		#FRAGMENT_OUTCOLOR = vec4( outColor, 1.0);\n"
		"		//gl_FragColor= vec4( outColor, 1.0); // TODO: look into deferred rendering methods\n"
		"	}\n"
		"\n"
		"	// Normals:\n"
		"	vec3 normalResult;\n"
		"	if(normalMapEnable) {\n"
		"		normalResult = (#TEXTURE_FUNC( normalMap[0], newCoords)).xyz;\n"
		"		normalResult = TBN * normalize( (normalResult * 2.0 - 1.0));\n"
		"	} else {\n"
		"		normalResult = normalize(  (Normal0));// normalize(Normal0);\n"
		"	}\n"
		"	NormalOut.rgb = normalResult * 0.5 + 0.5;\n"
		"	NormalOut.a = 1.0;\n"
		"\n"
		"\n"
		"	// Specular:\n"
		"	vec4 specular = vec4(uSpecularLevel, uSpecularExponent, uMetallicLevel, 0.0);\n"
		"	if (specularityMapEnable) {\n"
		"		specular *= #TEXTURE_FUNC(specularityMap[0], newCoords) ;\n"
		"	}\n"
		"\n"
		"	float ks = specular.r;\n"
		"	float ke = specular.g;\n"
		"	float km = specular.b;\n"
		"	MaterialOut = vec4(ks, ke, km, 1.0);\n"
		"}\n";
	}

	const void GeometryShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

//#ifndef OPENGLES_FOUND
#ifndef GL_ES_VERSION_2_0 // also defined for 3.0
		if(MogiGLInfo::getInstance()->getVersion() >= 410) {
			macros["#HEADER"] = "#version 410";
		} else if(MogiGLInfo::getInstance()->getVersion() >= 300) {
			macros["#HEADER"] = "#version 330";	// untested
		} else if(MogiGLInfo::getInstance()->getVersion() >= 130) {
			macros["#HEADER"] = "#version 130";	// untested
		}
#endif



		if (MogiGLInfo::getInstance()->getVersion() >= 300) {
			macros["#FRAGMENT_OUTPUT"] =
			"layout(location = 0) out vec4 FragColor;\n"
			"layout(location = 1) out vec4 NormalOut;\n"
			"layout(location = 2) out vec4 MaterialOut;\n"
			"layout(location = 3) out vec4 PositionOut;\n";
		} else {
			macros["#FRAGMENT_OUTPUT"] =
			"//layout(location = 0) out vec4 FragColor; // TODO: look into deferred rendering methods\n"
			"vec4 NormalOut; // TODO: look into deferred rendering methods\n"
			"vec4 MaterialOut; // TODO: look into deferred rendering methods\n"
			"vec4 PositionOut; // TODO: look into deferred rendering methods\n";

			// Not a good way to handle this:
			macros["smooth "] = "";
		}

	}

	const bool GeometryShaderParameters::lessThan(const ShaderParametersDynamic* right) const {
		return false; // always equal?
	}

	ShaderParametersDynamic* GeometryShaderParameters::copy() const {
		GeometryShaderParameters* result = new GeometryShaderParameters;
		*result = *this;
		return result;
	}

#ifdef _cplusplus
}
#endif
