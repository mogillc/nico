//
//  Shader.vsh
//  OpenGL 2
//
//  Created by Mr Roboto on 5/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

attribute highp vec4 position;
attribute highp vec3 normal;
attribute vec4 color;
attribute vec3 tangent;
attribute vec2 UV;
//attribute vec3 light;

//varying lowp vec4 colorVarying;
uniform vec3 lightPosition;

uniform highp mat4 modelViewProjectionMatrix;
uniform highp mat3 normalMatrix;

uniform mat4 lightModelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

//varying highp vec4 lightVertexPosition;

varying highp vec3 fposition;

varying vec2 texcoord;
varying highp vec3 outNormal;
varying lowp vec3 outColor;

void main()
{
    //vec3 eyeNormal = normalize(normalMatrix * normal);
    //vec3 lightPosition = vec3(2.0, 2.0, 20);
    //vec4 diffuseColor = vec4(1.0*color[0], 1.0*color[1], 1.0*color[2], 1.0*color[3]);
    
    //float nDotVP = max(0.0, dot(eyeNormal, normalize(lightPosition)));
                 
    //colorVarying = diffuseColor * nDotVP;
    
    gl_Position = modelViewProjectionMatrix * position;

	//lightVertexPosition = lightModelViewProjectionMatrix *   position ;
	//highp vec4 tempvertPosition = modelViewMatrix * position ;
	//fposition = tempvertPosition.xyz;
	fposition = (modelViewMatrix * position).xyz;
	outColor = color.xyz ;
	outNormal = normalMatrix * normal;

	texcoord = UV;


//	lightPosition = vec3(2.0, 2.0, 20);
//	n = normalMatrix * normal;
//	PointToLight = ((modelViewMatrix * vec4(lightPosition,1.0)) - (modelViewMatrix * modelViewProjectionMatrix * position)).xyz;
}
