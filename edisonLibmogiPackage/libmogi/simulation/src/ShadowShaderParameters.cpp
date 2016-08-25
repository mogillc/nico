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
#include <iostream>
#include <iomanip>
#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	ShadowShaderParameters::ShadowShaderParameters()
	: mColorSource(COLOR_SOURCE_VERTEX_DATA), specularMapEnable(false), normalMapEnable(false), disparityMapEnable(false), numberOfLights(1), numberOfShadowMaps(1), useShadows(false) {
//		colorMapEnableFromMaterial = colorMapEnable;
		normalMapEnableFromMaterial = normalMapEnable;
		disparityMapEnableFromMaterial = disparityMapEnable;
		specularMapEnableFromMaterial = specularMapEnable;

//#ifdef OPENGLES_FOUND
#ifdef GL_ES_VERSION_2_0 // also defined for 3.0
		pcfKernelSize = 2;
#else
		pcfKernelSize = 3;
#endif
		numberOfLights = 2;
		lightPos = new Math::Matrix[numberOfLights];
		lightColor = new Math::Matrix[numberOfLights];
		kAtt = new Math::Matrix[numberOfLights];
		lightModelViewProjectionMatrix = new Math::Matrix[numberOfLights];
	}

	ShadowShaderParameters::~ShadowShaderParameters() {
		delete [] lightPos;
		delete [] lightColor;
		delete [] kAtt;
		delete [] lightModelViewProjectionMatrix;
	}

	void ShadowShaderParameters::allocateBasedOnLightCount() {
		delete [] lightPos;
		delete [] lightColor;
		delete [] kAtt;
		delete [] lightModelViewProjectionMatrix;

		lightPos = new Math::Matrix[numberOfLights];
		lightColor = new Math::Matrix[numberOfLights];
		kAtt = new Math::Matrix[numberOfLights];
		lightModelViewProjectionMatrix = new Math::Matrix[numberOfLights];

	}

	void ShadowShaderParameters::configureParameters(StaticShader* shader) {
		std::cout << "Configuring parameters for ShadowShader!" << std::endl;
		addAssociationTexture(shader, "colorMap", &colorMap);
		addAssociationTexture(shader, "normalMap", &normalMap);
		addAssociationTexture(shader, "specularityMap", &specularityMap);
		addAssociationTexture(shader, "heightMap", &heightMap);

		addAssociationMatrix(shader, "modelViewProjectionMatrix", &modelViewProjectionMatrix);
		addAssociationMatrix(shader, "mColorDiffuse", &mColorDiffuse);
		addAssociationMatrix(shader, "normalMatrix", &normalMatrix);
		addAssociationMatrix(shader, "modelMatrix", &modelMatrix);
		addAssociationMatrix(shader, "viewPosition", &viewPosition);
		addAssociationMatrix(shader, "viewMatrix", &viewMatrix);
		addAssociationMatrix(shader, "lightPos", lightPos);
		addAssociationMatrix(shader, "lightColor", lightColor);
		addAssociationMatrix(shader, "kAtt", kAtt);
		addAssociationMatrix(shader, "lightModelViewProjectionMatrix", lightModelViewProjectionMatrix);

		addAssociationFloat(shader, "specularProperties", specularProperties);
	}

	const char* ShadowShaderParameters::getVertexTemplate() const {
		return "// Generated: ShadowShaderParameters::getVertexTemplate()\n"
		"#HEADER\n"
		"\n"
		"#VERTEX_INPUT vec3 position;\n"
		"#VERTEX_INPUT vec3 normal;\n"
		"#VERTEX_INPUT vec3 tangent;\n"
//		"#VERTEX_INPUT vec3 bitangent;\n"
		"#VERTEX_INPUT vec3 color;\n"
		"#VERTEX_INPUT vec2 UV;\n"
		"\n"
		"uniform mat4 modelViewProjectionMatrix;\n"
		"uniform vec3 mColorDiffuse;\n"
		"uniform float specularProperties[2];\n"
		"\n"
		"uniform mat3 normalMatrix;\n"
//		"uniform mat4 viewMatrix;\n"
		"uniform mat4 modelMatrix;\n"
		"uniform vec3 viewPosition;\n"
		"uniform highp mat4 viewMatrix;\n"
//		"\n"
//		"uniform bool normalMapEnable;\n"
		"\n"
		"//uniform float mapWidth[#NUMBER_OF_LIGHTS];\n"
		"//uniform float mapHeight[#NUMBER_OF_LIGHTS];\n"
		"\n"
		"uniform highp vec3 lightPos[#NUMBER_OF_LIGHTS];\n"
		"uniform highp mat4 lightModelViewProjectionMatrix[#NUMBER_OF_LIGHTS];\n"
		"\n"
//		"#VERTEX_OUTPUT highp vec3 lightPos2[#NUMBER_OF_LIGHTS];\n"
		"#VERTEX_OUTPUT highp vec4 lightVertexPosition[#NUMBER_OF_LIGHTS];	// for shadows\n"
		"\n"
		"#VERTEX_OUTPUT vec3 fposition;\n"
		"#VERTEX_OUTPUT vec3 outColor;\n"
		"#VERTEX_OUTPUT vec2 texcoord;\n"
		"\n"
		"#VERTEX_OUTPUT vec3 tangentFragPosition;\n"
		"#VERTEX_OUTPUT vec3 tangentLightPosition[#NUMBER_OF_LIGHTS];\n"
		"#VERTEX_OUTPUT vec3 tangentViewPosition;\n"
		"\n"
//		"#VERTEX_OUTPUT mat3 TBN;\n"
//		"mat3 TBN;\n"
//		"//#VERTEX_OUTPUT #PCF_COORDINATES;\n"
		"\n"
		"void main()\n"
		"{\n"
		"    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);\n"
		"    \n"
		"    fposition = vec3( modelMatrix * vec4( position, 1.0));\n"
		"    outColor = color ;\n"
		"    \n"
//		"    //outNormal = vec3(0.0, 0.0, 1.0);//normal;\n"
//		"    \n"
//		"    // Generated unrolled light postion calculations:\n"
//		"#UNROLLED_LIGHT_POSITION_CALCULATION\n"
//		"    // Generated PCF coordinate stuff:\n"
//		"//#UNROLLED_PCF_LOCATIONS\n"
		"    \n"
		"    texcoord = UV;\n"
		"    \n"
		//		"    //vec3 tangent;\n"
		//		"    vec3 n=normal;//normalize( normalMatrix * normal );\n"
		//		"    //vec3 v1=cross( n, vec3(0.0,0.0,1.0));\n"
		//		"    //vec3 v2=cross( n, vec3(0.0,1.0,0.0));\n"
		//		"    //if(length(v1)>length(v2))\n"
		//		"    //    tangent=v1;\n"
		//		"    //else\n"
		//		"    //tangent=v2;\n"
		//		"        \n"
		"    vec3 n=normalize( normalMatrix * normal);\n"
		"    vec3 t=normalize( normalMatrix * tangent);\n"
		"    vec3 b=cross(n,t);\n"
		"    mat3 TBN = mat3( vec3(t.x,b.x,n.x), vec3(t.y,b.y,n.y), vec3(t.z,b.z,n.z));\n"
//		"    mat3 TBN = transpose(mat3( t, b, n ));\n"
		"    tangentFragPosition = TBN * fposition;\n"
		"    tangentViewPosition = TBN * viewPosition;\n"
		"    // Generated unrolled light position calculations:\n"
		"#UNROLLED_LIGHT_POSITION_CALCULATION\n"
		"}";
	}

	const char* ShadowShaderParameters::getFragmentTemplate() const {
		return "// Generated: ShadowShaderParameters::getFragmentTemplate()\n"
		"#HEADER\n"
		"\n"
		"uniform highp sampler2D shadowMap[#NUMBER_OF_LIGHTS];\n"
		"\n"
		"// Generated Uniforms: \n"
		"#UNIFORMS \n"
		"uniform float mapWidth[#NUMBER_OF_LIGHTS];\n"
		"uniform float mapHeight[#NUMBER_OF_LIGHTS];\n"
		"\n"
		"uniform highp vec3 lightPos[#NUMBER_OF_LIGHTS];\n"
		"uniform lowp vec3 lightColor[#NUMBER_OF_LIGHTS];\n"
		//		"uniform float kc[#NUMBER_OF_LIGHTS];\n"
		//		"uniform float kl[#NUMBER_OF_LIGHTS];\n"
		//		"uniform float kq[#NUMBER_OF_LIGHTS];\n"
		"uniform highp vec3 kAtt[#NUMBER_OF_LIGHTS];\n"
		"\n"
		//		"uniform float uSpecularExponent;\n"
		//		"uniform float uSpecularLevel;\n"
//		"uniform vec2 uSpecular;\n"
		"uniform float specularProperties[2];\n"
		"\n"
//		"#FRAGMENT_INPUT highp vec3 lightPos2[#NUMBER_OF_LIGHTS];\n"
		"#FRAGMENT_INPUT highp vec4 lightVertexPosition[#NUMBER_OF_LIGHTS];\n"
		"\n"
		"#FRAGMENT_INPUT vec3 fposition;\n"
		"#FRAGMENT_INPUT vec3 outColor;\n"
		"#FRAGMENT_INPUT vec2 texcoord;\n"
		"\n"
//		"#FRAGMENT_INPUT mat3 TBN;\n"
//		"//#FRAGMENT_INPUT #PCF_COORDINATES;\n"
		"#FRAGMENT_INPUT vec3 tangentFragPosition;\n"
		"#FRAGMENT_INPUT vec3 tangentLightPosition[#NUMBER_OF_LIGHTS];\n"
		"#FRAGMENT_INPUT vec3 tangentViewPosition;\n"
		"\n"
		"#FRAGMENT_OUTPUT\n"
		"\n"
		"void main()\n"
		"{\n"
		//		"    float shininess = 32.0;\n"
		"    vec3 totalColor = vec3(0.0,0.0,0.0);\n"
		"    vec2 coordinates;\n"
		"    \n"
		"    vec3 tangentSurface2view = tangentViewPosition - tangentFragPosition;// TBN * -fposition;\n"
		"    vec3 eye = normalize(tangentSurface2view);\n"
		"    // Generated Height map code:\n"
		"#HEIGHT_SOURCE\n"
		"    \n"
		"    float specularContribution;\n"
		"    // Generated Normal code:\n"
		"#NORMAL_SOURCE\n"
		"    \n"
		"    // Generated Color code:\n"
		"#COLOR_SOURCE\n"
		"    \n"
		"    // Generated light code:\n"
		"#LIGHT_CODE\n"
		"    \n"
		"    #FRAGMENT_OUTCOLOR = vec4(totalColor, 1.0 );\n"
		"}\n"
		"    ";
	}

	const void ShadowShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

		// Currently only number of lights needed for this to unroll the loop and define array sizes
		std::stringstream nLightString("");
		nLightString << numberOfLights;

		macros["#NUMBER_OF_LIGHTS"] = nLightString.str();

		std::stringstream lightCalculation("");
		for (int i = 0; i < numberOfLights; i++) {
			lightCalculation << "    lightVertexPosition[" << i << "] = lightModelViewProjectionMatrix[" << i << "] * vec4( position, 1.0);\n";
//			lightCalculation << "        lightPos2[" << i << "] = vec3( viewMatrix * vec4( lightPos[" << i << "], 1.0));\n";
			lightCalculation << "    tangentLightPosition[" << i << "] = TBN * lightPos[" << i << "];\n";
		}
		macros["#UNROLLED_LIGHT_POSITION_CALCULATION"] = lightCalculation.str();

		// Fragment shader:
		std::stringstream fragmentUniforms("");

		switch (mColorSource) {
			case COLOR_SOURCE_VERTEX_DATA:
				macros["#COLOR_SOURCE"]  = "    vec3 texcolor = outColor;\n";
					break;

			case COLOR_SOURCE_MAP:
				fragmentUniforms << "uniform sampler2D colorMap[1];\n";
				macros["#COLOR_SOURCE"]  = "    vec3 texcolor = vec3( #TEXTURE_FUNC( colorMap[0], newCoords) );\n";

				break;

			case COLOR_SOURCE_MATERIAL:
				fragmentUniforms << "uniform vec3 mColorDiffuse;\n";
				macros["#COLOR_SOURCE"]  = "";//    vec3 texcolor = mColorDiffuse;\n";

				macros["texcolor"]  = "mColorDiffuse\n";

				break;
		}
//		if (colorMapEnable && colorMapEnableFromMaterial) {
//			fragmentUniforms << "uniform sampler2D colorMap[1];\n";
//			macros["#COLOR_SOURCE"]  = "    vec3 texcolor = vec3( #TEXTURE_FUNC( colorMap[0], newCoords) );\n";
//		} else {
//			macros["#COLOR_SOURCE"]  = "    vec3 texcolor = outColor;\n";
//		}

		if (normalMapEnable && normalMapEnableFromMaterial) {
			fragmentUniforms << "uniform sampler2D normalMap[1];\n";
			macros["#NORMAL_SOURCE"] = "    vec3 thisNormal = normalize( (#TEXTURE_FUNC( normalMap[0], newCoords).xyz)*2.0 - 1.0);\n";
			//			"    thisNormal.y = -thisNormal.y;\n";
		} else {
			macros["#NORMAL_SOURCE"] = "    vec3 thisNormal = vec3( 0.0, 0.0, 1.0);\n";
		}

		if (disparityMapEnable && disparityMapEnableFromMaterial) {
			fragmentUniforms << "uniform sampler2D heightMap[1];\n";
			macros["#HEIGHT_SOURCE"] = "    vec2 newCoords = texcoord - (eye.xy * v);\n";
		} else {
			macros["#HEIGHT_SOURCE"] = "    vec2 newCoords = texcoord;\n";
		}

		if (specularMapEnable && specularMapEnableFromMaterial) {
			fragmentUniforms << "    uniform sampler2D specularityMap[1];\n";
//			macros["#LIGHT_CODE"] = unrollLightLoop(numberOfLights, useShadows, "specularContribution *= uSpecular.y * #TEXTURE_FUNC(specularityMap[0],newCoords).x;\n");
			macros["#LIGHT_CODE"] = unrollLightLoop(numberOfLights, useShadows, "specularContribution *= specularProperties[1] * #TEXTURE_FUNC(specularityMap[0],newCoords).x;\n");
		} else {
			//			macros["#LIGHT_CODE"] = unrollLightLoop(numberOfLights, useShadows, "specularContribution *= uSpecularLevel;// Specularity map not enabled.\n");
//			macros["#LIGHT_CODE"] = unrollLightLoop(numberOfLights, useShadows, "specularContribution *= uSpecular.y;// Specularity map not enabled.\n");
			macros["#LIGHT_CODE"] = unrollLightLoop(numberOfLights, useShadows, "specularContribution *= specularProperties[1];// Specularity map not enabled.\n");
		}

		//		std::stringstream pcfVector("");
		//		pcfVector << "vec2 pcfCoordinates[" << pcfKernelSize*pcfKernelSize*numberOfLights << "]";
		//		macros["#PCF_COORDINATES"] = pcfVector.str();
		//
		//		std::stringstream pcfCode("");
		//		float centerX = (float)(pcfKernelSize-1)/2;
		//		float centerY = (float)(pcfKernelSize-1)/2;
		//		for (int i = 0; i < pcfKernelSize; i++) {
		//			for (int j = 0; j < pcfKernelSize; j++) {
		//				for (int k = 0; k < numberOfLights; k++) {
		//					pcfCode << "    pcfCoordinates[" << i + j*pcfKernelSize + k*pcfKernelSize*pcfKernelSize << "] = vec2(" << (float)(i)-centerX << "/mapWidth[" << k << "]," << (float)(j)-centerY << "/mapHeight[" << k << "]);\n";
		//				}
		//			}
		//		}
		//		macros["#UNROLLED_PCF_LOCATIONS"] = pcfCode.str();

		macros["#UNIFORMS"] = fragmentUniforms.str();
	}

	const bool ShadowShaderParameters::lessThan(const ShaderParametersDynamic* right) const {
		//		const ShadowShaderParameters* Left = (const ShadowShaderParameters*)left;
		const ShadowShaderParameters* Right = (const ShadowShaderParameters*)right;

//		if((colorMapEnable && colorMapEnableFromMaterial) > (Right->colorMapEnable && Right->colorMapEnableFromMaterial) ) return false;
//		if((colorMapEnable && colorMapEnableFromMaterial) < (Right->colorMapEnable && Right->colorMapEnableFromMaterial) ) return true;
		if (mColorSource > Right->mColorSource) return false;
		if (mColorSource < Right->mColorSource) return true;
		if((normalMapEnable && normalMapEnableFromMaterial) > (Right->normalMapEnable && Right->normalMapEnableFromMaterial) ) return false;
		if((normalMapEnable && normalMapEnableFromMaterial) < (Right->normalMapEnable && Right->normalMapEnableFromMaterial) ) return true;
		if((specularMapEnable && specularMapEnableFromMaterial) > (Right->specularMapEnable && Right->specularMapEnableFromMaterial) ) return false;
		if((specularMapEnable && specularMapEnableFromMaterial) < (Right->specularMapEnable && Right->specularMapEnableFromMaterial) ) return true;
		if((disparityMapEnable && disparityMapEnableFromMaterial) > (Right->disparityMapEnable && Right->disparityMapEnableFromMaterial) ) return false;
		if((disparityMapEnable && disparityMapEnableFromMaterial) < (Right->disparityMapEnable && Right->disparityMapEnableFromMaterial) ) return true;
		if(numberOfLights > Right->numberOfLights ) return false;
		if(numberOfLights < Right->numberOfLights ) return true;
		if(numberOfShadowMaps > Right->numberOfShadowMaps ) return false;
		if(numberOfShadowMaps < Right->numberOfShadowMaps ) return true;
		if(useShadows > Right->useShadows) return false;
		if(useShadows < Right->useShadows) return true;

		return false;
	}

	ShaderParametersDynamic* ShadowShaderParameters::copy() const {
		ShadowShaderParameters* result = new ShadowShaderParameters;
		*result = *this;
		//		std::cout << "Just created:" << result->toString() << " form: " << toString() << std::endl;
		return result;
	}


//	void ShadowShaderParameters::setUniform(const std::string& name, void* value, int index) {
////		std::cout << "In " <<__PRETTY_FUNCTION__ << "\t Name: " << name << "_" << index << std::endl;
//		if(name.compare("colorSource") == 0) {
//			mColorSource = (ColorSource)*(GLint*)value;
//			return;
//		} else if (name.compare("normalMapEnable") == 0) {
//			normalMapEnable = *(GLint*)value;
//			return;
//		} else if (name.compare("specularityMapEnable") == 0) {
//			specularMapEnable = *(GLint*)value;
//			return;
//		} else if (name.compare("heightMapEnable") == 0) {
//			disparityMapEnable = *(GLint*)value;
//			return;
//		} else if (name.compare("nLights") == 0) {
//			numberOfLights = *(GLint*)value;
//			return;
//		}
//
//
//		ShaderParameters::setUniform(name, value, index);
//	}


	std::string ShadowShaderParameters::unrollLightLoop( int numberOfLights, bool shadowEnable, const std::string& specularCode ) const {
		std::stringstream result("");
		result << std::setprecision(9) << std::fixed;

		result << "    vec3 fragmentToLight;\n";
		result << "    float dist;\n";
		result << "    float att;\n";
		result << "    vec3 fragmentToLightDirection;\n";
		result << "    float diffuseContribution;\n";
		result << "    vec3 reflectionNew;\n";
		result << "    vec3 ambient;\n";
		result << "    vec3 diffuse;\n";
		result << "    vec3 specular;\n";

		for (int i = 0; i < numberOfLights; i++) {
			//result << unrollShadowMapPCF(i);
			result << "    {\n";
			result << unrollShadowMapPCF(i, shadowEnable);

			result << "        fragmentToLight = tangentLightPosition[" << i << "] - tangentFragPosition;\n";//fposition-lightPos2[" << i << "];\n";
			result << "\n";
			result << "        dist = length(fragmentToLight);	//distance from light-source to surface\n";
			//			result << "        att=1.0/(1.0+0.10*dist+1.0*dist*dist);	//attenuation (constant,linear,quadric)\n";
			//			result << "        att=1.0/(kc[" << i << "] + kl[" << i << "]*dist + kq[" << i << "]*dist*dist);	//attenuation (constant,linear,quadric)\n";
			result << "        att=1.0/(kAtt[" << i << "].x + kAtt[" << i << "].y*dist + kAtt[" << i << "].z*dist*dist);	//attenuation (constant,linear,quadric)\n";
			result << "\n";
			result << "        fragmentToLightDirection = normalize( fragmentToLight );//TBN * normalize( -lightToFragment );\n";
			//			result << "        //vec3 surf2camera = eye;// normalize( tangentSurface2view );\n";
			result << "\n";
			result << "        diffuseContribution = max( 0.0, dot( thisNormal, fragmentToLightDirection));\n";
			result << "        reflectionNew = reflect( -fragmentToLightDirection, thisNormal );\n";
			//			result << "        specularContribution = pow( max(0.0, dot(reflectionNew, eye)), shininess);\n";
			//			result << "        specularContribution = pow( max(0.0, dot(reflectionNew, eye)), uSpecularExponent);\n";
//			result << "        specularContribution = pow( max(0.0, dot(reflectionNew, eye)), uSpecular.x);\n";
			result << "        specularContribution = pow( max(0.0, dot(reflectionNew, eye)), specularProperties[0]);\n";
			result << "\n";
			result << "        // Generated Specular code: \n";
			result << "        " << specularCode;
			result << "\n";
			result << "        ambient = texcolor * " << 0.1/(float)numberOfLights << ";// * (0.1/float(nLights));// * mambient;	//the ambient light\n";
			result << "        diffuse = diffuseContribution * texcolor;///float(nLights));\n";
			result << "        specular = vec3(specularContribution);// * mspecular;\n";
			result << "\n";
			//			result << "        totalColor += (ambient + (diffuse+specular)*shadowValue/(dist*dist)*att) * lightColor[" << i << "];\n";
			result << "        totalColor += (ambient + (diffuse+specular)*shadowValue*att) * lightColor[" << i << "];\n";

			result << "}\n";
		}
		return result.str();
	}

	std::string ShadowShaderParameters::unrollShadowMapPCF( int lightIndex, bool shadowEnable ) const {
		// This is limited to a 9x9 grid at the moment:
		std::stringstream result("");
		result << std::setprecision(9) << std::fixed;
		if (shadowEnable) {
			result <<
			"        float shadowValue=0.0;\n"
//			"        highp vec4 lightVertexPosition2 = vec4(tangentLightPosition[" << lightIndex << "], 1.0);\n"// << "lightVertexPosition[" << lightIndex << "];\n"
			"        highp vec4 lightVertexPosition2 = lightVertexPosition[" << lightIndex << "];\n"

			"        lightVertexPosition2 /= lightVertexPosition2.w;\n"
			"        vec2 texcoordNew;\n"
			"        // The following if statement is another form of the implementeation commented below, but may be optimized for GLSL:\n"
			"        //if((lightVertexPosition2.x <= 1.0) && (lightVertexPosition2.x >= 0.0) && (lightVertexPosition2.y <= 1.0) && (lightVertexPosition2.y >= 0.0))\n"
			"        if((abs(sign(lightVertexPosition2.x)+sign(lightVertexPosition2.x - 1.0)) + abs(sign(lightVertexPosition2.y)+sign(lightVertexPosition2.y - 1.0))) == 0.0)\n";
			result << "        {\n";

			Math::Matrix kernel(pcfKernelSize, pcfKernelSize);
			float centerX = (float)(kernel.numColumns()-1)/2;
			float centerY = (float)(kernel.numColumns()-1)/2;
			float total = 0;
			float sigma = (float)pcfKernelSize/2.0;
			for (int i = 0; i < kernel.numRows(); i++) {
				for (int j = 0; j < kernel.numColumns(); j++) {
					kernel(i,j) = exp(-(pow(((float)i - centerX)/sigma, 2) + pow(((float)j - centerY)/sigma, 2)));
					total += kernel(i,j);
				}
			}
			kernel /= total;


			//			result << "        shadowValue = ";
			for (int i = 0; i < pcfKernelSize; i++) {
				for (int j = 0; j < pcfKernelSize; j++) {
					// Original unrolled:
					result << "            texcoordNew = lightVertexPosition2.xy + vec2(" << (float)(i)-centerX << "/mapWidth[" << lightIndex << "]," << (float)(j)-centerY << "/mapHeight[" << lightIndex << "]);\n";
					//result << "            if ((texcoordNew.x <= 1.0) && (texcoordNew.x >= 0.0) && (texcoordNew.y <= 1.0) && (texcoordNew.y >= 0.0))\n";
					//					result << "                if( #TEXTURE_FUNC( shadowMap[" << lightIndex << "], texcoordNew).r >= lightVertexPosition2.z )\n";
					//					result << "                    shadowValue += 0.111111111; // 1.0/9.0\n";
					//					result << "                shadowValue += mix(0.0, " << 1.0/9.0 << ", #TEXTURE_FUNC( shadowMap[" << lightIndex << "], texcoordNew).r >= lightVertexPosition2.z); // 1.0/9.0\n";
					result << "                shadowValue += mix(0.0, " << kernel(i,j) << ", ";
					if (MogiGLInfo::getInstance()->getVersion() < 300) {
						result << "float(";
					}
					result << "#TEXTURE_FUNC( shadowMap[" << lightIndex << "], texcoordNew).r >= lightVertexPosition2.z ";
					if (MogiGLInfo::getInstance()->getVersion() < 300) {
						result << ")";
					}
					result << ");\n";

					// Attempt to make things faster 1:
					//					result << "            texcoordNew = lightVertexPosition2.xy + pcfCoordinates[" << i + j*pcfKernelSize + lightIndex*pcfKernelSize*pcfKernelSize << "];\n";
					//					result << "                shadowValue += mix(0.0, " << kernel(i,j) << ", " << "float(" << "#TEXTURE_FUNC( shadowMap[" << lightIndex << "], texcoordNew).r >= lightVertexPosition2.z " << ")" << ");\n";

					//					// Attempt to make things faster 1.1, variation of the above:
					//					result << "mix(0.0, " << kernel(i,j) << ", ";
					//					if (MogiGLInfo::getInstance()->getVersion() < 300) {
					//						result << "float(";
					//					}
					//					result << "#TEXTURE_FUNC( shadowMap[" << lightIndex << "], lightVertexPosition2.xy + pcfCoordinates[" << i + j*pcfKernelSize + lightIndex*pcfKernelSize*pcfKernelSize << "]).r >= lightVertexPosition2.z ";
					//					if (MogiGLInfo::getInstance()->getVersion() < 300) {
					//						result << ")";
					//					}
					//					result << ")";
					//					if (i+j*pcfKernelSize+1 != pcfKernelSize*pcfKernelSize) {
					//						result << " +\n            ";
					//					}
				}
			}
			//			result << ";\n";
			result << "        }\n";
		} else {
			result << "        float shadowValue = 1.00;\n";
		}
		
		//		std::cout << "Urolled: " << result.str() << std::endl;
		return result.str();
	}


	void ShadowShaderParameters::setNumberOfLights( GLint lightCount )  {
		if (numberOfLights != lightCount) {
			numberOfLights = lightCount;
			allocateBasedOnLightCount();
		}
	}



#ifdef _cplusplus
}
#endif
