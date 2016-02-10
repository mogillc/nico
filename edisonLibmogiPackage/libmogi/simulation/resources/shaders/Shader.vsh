#version 330

//
//  Shader.vsh
//  OpenGL 2
//
//  Created by Mr Roboto on 5/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
//#ifdef APPLE
//#define lowp
//#endif

attribute vec4 position;
attribute vec3 normal;
attribute vec4 color;
//attribute vec3 light;

varying vec4 colorVarying;
varying vec3 fposition;
varying vec3 fnormal;
//uniform vec3 lightPosition;

uniform mat3 normalMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
uniform vec3 lightPos;

//varying vec3 n, PointToLight;
//uniform mat4 modelViewMatrix;

void main()
{
	/*
    vec3 eyeNormal = normalize(normalMatrix * normal);

	vec3 vertPosition = vec3(position[0], position[1], position[3]);
	//vec4 tempLightPosition = modelViewProjectionMatrix*vec4(3, 3, 0, 0);
    //vec3 lightPosition = vec3(3, 3, 0);
	vec3 lightPosition = lightPos;
	//vec3 lightPosition = vec3( tempLightPosition[0], tempLightPosition[1], tempLightPosition[2]);
	
	//vec3 lightPosition = vec3(0.0, -3.0, -1);
	vec3 lightPosition2 = vec3(0.0, -3.0, -1);
	vec3 lightPosition3 = vec3(0.0, 0.0, 1.0);
    vec4 diffuseColorRed = vec4(1.0*color[0], .0*color[1], .0*color[2], 1.0*color[3]);
	vec4 diffuseColor = vec4(1.0*color[0], 1.0*color[1], 1.0*color[2], 1.0*color[3]);
	vec4 diffuseColorBlue = vec4(.0*color[0], .0*color[1], 1.0*color[2], 1.0*color[3]);
    
    //float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
	float nDotVP2 = max(0.0, dot(eyeNormal, normalize(lightPosition2))) * 0.0;
	float nDotVP3 = max(0.0, dot(eyeNormal, normalize(lightPosition3)))*0.1;
    //colorVarying = diffuseColorRed * nDotVP;
	//colorVarying += diffuseColorBlue * nDotVP2;
	//colorVarying += diffuseColor * nDotVP3;	// very dim ambient lighting (so I know things are alive
	*/
	colorVarying = color;
	gl_Position = modelViewProjectionMatrix * position;
/*
	vec4 g_gl_Position = modelViewProjectionMatrix * position;
	
	vec4 tempvertPosition = modelViewMatrix * position;
	vertPosition = vec3(tempvertPosition[0], tempvertPosition[1], tempvertPosition[2]);
	//vertPosition = vec3(g_gl_Position[0], g_gl_Position[1], g_gl_Position[2]);
	float lightIntensity = length(vertPosition - lightPosition);
	lightIntensity = 60.0 / (lightIntensity * lightIntensity);
	float nDotPointSource = max(0.0, lightIntensity * dot( eyeNormal, normalize( lightPosition - vertPosition)));
	colorVarying += diffuseColor * nDotPointSource;

	//colorVarying[0] = diffuseColor[0] * normalMatrix[0][0];
	//colorVarying[1] = diffuseColor[1] * normalMatrix[1][0];
	//colorVarying[2] = diffuseColor[2] * normalMatrix[2][0];
	//colorVarying[0] = diffuseColor[0] * normal[0];
	//colorVarying[1] = diffuseColor[1] * normal[1];
	//colorVarying[2] = diffuseColor[2] * normal[2];
	
    //gl_FrontColor = colorVarying;
    gl_Position = g_gl_Position;
	g_gl_Position = modelViewMatrix * position;
	fposition = vec3(g_gl_Position[0], g_gl_Position[1], g_gl_Position[2]);
	fnormal = eyeNormal;

//	lightPosition = vec3(2.0, 2.0, 20);
//	n = normalMatrix * normal;
//	PointToLight = ((modelViewMatrix * vec4(lightPosition,1.0)) - (modelViewMatrix * modelViewProjectionMatrix * position)).xyz;
 */
}
