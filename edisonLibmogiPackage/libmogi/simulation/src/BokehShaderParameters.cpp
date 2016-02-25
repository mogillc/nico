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

#include <math.h>

#ifdef _cplusplus
extern "C" {
#endif

	using namespace Mogi;
	using namespace Simulation;

	BokehShaderParameters::BokehShaderParameters()
	: debugEnable(false), autoFocusEnable(false), vignettingEnable(false), samples(3), rings(3) {
	}
	
	const char* BokehShaderParameters::getVertexTemplate() const {
		return "#HEADER\n"
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
		"}";
	}

	const char* BokehShaderParameters::getFragmentTemplate() const {
		return "#HEADER\n"
		"\n"
		"uniform sampler2D bgl_RenderedTexture;\n"
		"uniform sampler2D bgl_DepthTexture;\n"
		"uniform float bgl_RenderedTextureWidth;\n"
		"uniform float bgl_RenderedTextureHeight;\n"
		"\n"
		"#define PI  (3.14159265)\n"
		"\n"
		"float width = bgl_RenderedTextureWidth; //texture width\n"
		"float height = bgl_RenderedTextureHeight; //texture height\n"
		"\n"
		"uniform float focalDepth;  //focal distance value in meters, but you may use autofocus option below\n"
		"uniform float focalLength; //focal length in mm\n"
		"uniform float fstop; //f-stop value\n"
		"uniform bool showFocus; //show debug focus point and focal range (red = focal point, green = focal range)\n"
		"\n"
		"uniform bool autofocus;\n"
		"\n"
		"\n"
		"uniform float znear; //camera clipping start\n"
		"uniform float zfar; //camera clipping end\n"
		"\n"
		"int samples = 3;        //samples on the first ring\n"
		"int rings = 3;            //ring count\n"
		"\n"
		"float CoC = 0.03;        //circle of confusion\n"
		"\n"
		"bool vignetting = true; //use optical lens vignetting\n"
		"float vignout = 1.3; //vignetting outer border\n"
		"float vignin = 0.0; //vignetting inner border\n"
		"float vignfade = 4.0; //f-stops till vignete fades\n"
		"\n"
		"vec2 focus = vec2(0.5,0.5); // autofocus point on screen (0.0,0.0 - left lower corner, 1.0,1.0 - upper right)\n"
		"float maxblur = 2.0; //clamp value of max blur (0.0 = no blur,1.0 default)\n"
		"\n"
		"float threshold = 0.5; //highlight threshold;\n"
		"float gain = 0.8; //highlight gain;\n"
		"\n"
		"float bias = 0.5; //bokeh edge bias\n"
		"float fringe = 0.0; //bokeh chromatic aberration/fringing\n"
		"\n"
		"//bool noise = true; //use noise instead of pattern for sample dithering\n"
		"float namount = 0.0001; //dither amount\n"
		"\n"
		"\n"
		"#FRAGMENT_INPUT vec4 TexCoord[1];\n"
		"#FRAGMENT_OUTPUT\n"
		"\n"
		"vec3 color(vec2 coords,float blur) //processing the sample\n"
		"{\n"
		"    vec3 col = vec3(0.0);\n"
		"\n"
		"    col.r = #TEXTURE_FUNC( bgl_RenderedTexture, coords + vec2( 0.0, 1.0) * fringe * blur).r;\n"
		"    col.g = #TEXTURE_FUNC( bgl_RenderedTexture, coords + vec2( -0.866, -0.5) * fringe * blur).g;\n"
		"    col.b = #TEXTURE_FUNC( bgl_RenderedTexture, coords + vec2( 0.866, -0.5) * fringe * blur).b;\n"
		"\n"
		"    vec3 lumcoeff = vec3( 0.299, 0.587, 0.114);\n"
		"    float lum = dot( col.rgb, lumcoeff);\n"
		"    float thresh = max( (lum - threshold)*gain, 0.0);\n"
		"    return col + mix( vec3(0.0), col, thresh*blur);\n"
		"}\n"
		"\n"
		"vec2 rand(vec2 coord) //generating noise/pattern texture for dithering\n"
		"{\n"
		"    float noiseX = ((fract( 1.0 - coord.s*(width/2.0))*0.25) + (fract(coord.t*(height/2.0))*0.75))*2.0 - 1.0;\n"
		"    float noiseY = ((fract( 1.0 - coord.s*(width/2.0))*0.75) + (fract(coord.t*(height/2.0))*0.25))*2.0 - 1.0;\n"
		"\n"
		"    //if (noise)\n"
		"    //{\n"
		"        noiseX = clamp(fract(sin(dot(coord , vec2(12.9898,78.233))) * 43758.5453),0.0,1.0)*2.0 - 1.0;\n"
		"        noiseY = clamp(fract(sin(dot(coord , vec2(12.9898,78.233)*2.0)) * 43758.5453),0.0,1.0)*2.0 - 1.0;\n"
		"    //}\n"
		"    return vec2( noiseX, noiseY);\n"
		"}\n"
		"\n"
		"vec3 debugFocus(vec3 col, float blur, float depth)\n"
		"{\n"
		"    float edge = 0.002*depth; //distance based edge smoothing\n"
		"    float m = clamp(smoothstep(0.0, edge, blur), 0.0, 1.0);\n"
		"    float e = clamp(smoothstep(1.0 - edge, 1.0, blur), 0.0, 1.0);\n"
		"\n"
		"    col = mix( col, vec3( 1.0, 0.5, 0.0), (1.0 - m)*0.6);\n"
		"    col = mix( col, vec3( 0.0, 0.5, 1.0), ((1.0 - e) - (1.0 - m))*0.2);\n"
		"\n"
		"    return col;\n"
		"}\n"
		"\n"
		"float linearize(float depth)\n"
		"{\n"
		"    return -zfar * znear / (depth * (zfar - znear) - zfar);\n"
		"}\n"
		"\n"
		"float vignette()\n"
		"{\n"
		"    float dist = distance(TexCoord[0].st, vec2(0.5,0.5));\n"
		"    dist = smoothstep(vignout + (fstop/vignfade), vignin + (fstop/vignfade), dist);\n"
		"    return clamp( dist, 0.0, 1.0);\n"
		"}\n"
		"\n"
		"void main()\n"
		"{\n"
		"    //scene depth calculation\n"
		"\n"
		"    float depth = linearize(#TEXTURE_FUNC(bgl_DepthTexture, TexCoord[0].st ).x);\n"
		"\n"
		"    float fDepth = focalDepth;\n"
		"\n"
		"    float blur = 0.0;\n"
		"\n"
		"    float f = focalLength;        //focal length in mm\n"
		"    float d = fDepth*1000.0;    //focal plane in mm\n"
		"    float o = depth*1000.0;        //depth in mm\n"
		"\n"
		"    float a = (o * f)/(o - f);\n"
		"    float b = (d * f)/(d - f);\n"
		"    float c = (d - f)/(d * fstop * CoC);\n"
		"\n"
		"    blur = abs(a - b)*c;\n"
		"\n"
		"    blur = clamp(blur, 0.0, 1.0);\n"
		"\n"
		"    // calculation of pattern for ditering\n"
		"\n"
		"    vec2 noise = rand(TexCoord[0].st)*namount*blur;\n"
		"\n"
		"    // getting blur x and y step factor\n"
		"\n"
		"    float w = (1.0/width)*blur*maxblur+noise.x;\n"
		"    float h = (1.0/height)*blur*maxblur+noise.y;\n"
		"\n"
		"    // calculation of final color\n"
		"\n"
		"    vec3 col = #TEXTURE_FUNC(bgl_RenderedTexture, TexCoord[0].st).rgb;\n"
		"    //float s = 1.0;\n"
		"    //int ringsamples;\n"
		"    //\n"
		"    //for (int i = 1; i <= rings; i += 1)\n"
		"    //{\n"
		"    //    ringsamples = i * samples;\n"
		"    //\n"
		"    //    for (int j = 0 ; j < ringsamples ; j += 1)\n"
		"    //    {\n"
		"    //        float step = PI*2.0 / float(ringsamples);\n"
		"    //        float pw = (cos(float(j)*step)*float(i));\n"
		"    //        float ph = (sin(float(j)*step)*float(i));\n"
		"    //        float p = 1.0;\n"
		"    //\n"
		"    //        col += color(TexCoord[0].st + vec2(pw*w,ph*h), blur) * mix(1.0, (float(i))/(float(rings)), bias)*p;\n"
		"    //        s += 1.0*mix(1.0, (float(i))/(float(rings)), bias)*p;\n"
		"    //    }\n"
		"    //}\n"
		"    //col /= s; //divide by sample count\n"
		"\n"
		"    // Unrolled sampling loop:\n"
		"    #SAMPLING_LOOP\n"
		"\n"
		"    // Generated Debug Focus code:\n"
		"    #DEBUG_FOCUS\n"
		"\n"
		"    // Generated Vignette code:\n"
		"    #VIGNETTE_CODE\n"
		"\n"
		"    #FRAGMENT_OUTCOLOR = vec4( col, 1.0);\n"
		"}\n"
		;
	}

	const void BokehShaderParameters::getMacros(std::map<std::string, std::string>& macros) const {

		if (debugEnable) {
			macros["#DEBUG_FOCUS"] = "col = debugFocus( col, blur, depth);";
		} else {
			macros["#DEBUG_FOCUS"] = "";
		}

		if (vignettingEnable) {
			macros["#VIGNETTE_CODE"] = "col *= vignette();";
		} else {
			macros["#VIGNETTE_CODE"] = "";
		}

		macros["#SAMPLING_LOOP"] = unrollSamplingLoop();
	}

	const bool BokehShaderParameters::lessThan(const ShaderParameters* right) const {
//		const BokehShaderParameters* Left = (const BokehShaderParameters*)left;
		const BokehShaderParameters* Right = (const BokehShaderParameters*)right;

		if(autoFocusEnable > Right->autoFocusEnable) return false;
		if(autoFocusEnable < Right->autoFocusEnable) return true;
		if(debugEnable > Right->debugEnable) return false;
		if(debugEnable < Right->debugEnable) return true;
		if(vignettingEnable > Right->vignettingEnable) return false;
		if(vignettingEnable < Right->vignettingEnable) return true;
		if(samples > Right->samples) return false;
		if(samples < Right->samples) return true;
		if(rings > Right->rings) return false;
		if(rings < Right->rings) return true;
		
		return false;
	}

	ShaderParameters* BokehShaderParameters::copy() const {
		BokehShaderParameters* result = new BokehShaderParameters;
		*result = *this;
		return result;
	}

	std::string BokehShaderParameters::unrollSamplingLoop() const {
		std::stringstream result("");

		result << std::setprecision(9) << std::fixed;

		result << "    float s = 1.0;\n";
		for (int i = 1; i <= rings; i++) {
			int ringsamples = i * samples;
			for (int j = 0; j < ringsamples; j++) {
				float step = MOGI_PI*2.0 / float(ringsamples);
				float pw = (cos(float(j)*step)*float(i));
				float ph = (sin(float(j)*step)*float(i));
				float p = 1.0;

				result << "    col += color(TexCoord[0].st + vec2(" << pw << "*w," << ph << "*h), blur) * mix(1.0, (float(" << i << "))/(float(" << rings << ")), bias)*" << p << ";\n";
				result << "    s += 1.0*mix(1.0, (float(" << i << "))/(float(" << rings << ")), bias)*" << p << ";\n";
			}
		}

		result << "    col /= s;\n";

		return result.str();
	}

#ifdef _cplusplus
}
#endif
