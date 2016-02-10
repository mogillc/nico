#version 150 compatibility

//
//  Shader.fsh
//  OpenGL 2
//
//  Created by Mr Roboto on 5/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//
/*
varying vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
	//gl_FragColor = vec4(0.0,0.0,1.0,1.0);
	//gl_FragColor = gl_Color;
}
//*/
/*
//varying lowp vec2 TexCoordOut;
varying highp vec3 n, PointToLight;

uniform sampler2D Texture;

void main(void) {
 //   gl_FragColor = texture2D(Texture, TexCoordOut);

    highp vec3 nn = normalize(n);
    highp vec3 L = normalize(PointToLight);

    lowp float NdotL = clamp(dot(n, L), -0.8, 1.0);
    gl_FragColor *= (NdotL+1.)/2.;
}
*/



//http://www.youtube.com/user/thecplusplusguy
//GLSL tutorial - per-fragment (per-pixel lighting)
//fragment.frag
#version 120
varying vec3 fposition;
varying vec3 fnormal;
varying vec4 colorVarying;
//attribute vec4 color;

uniform vec3 lightPos;
/*
uniform vec3 mambient;	//gl_FrontMaterial
uniform vec3 mdiffuse;
uniform vec3 mspecular;
uniform float shininess;

uniform vec3 lambient;	//gl_LightSource[0]
uniform vec3 ldiffuse;
uniform vec3 lspecular;
*/

void main()
{

//	vec3 lightPos = vec3(3.0,3.0,0.0);
/*
	vec3 mambient = vec3(0.2,0.2,0.2);	//gl_FrontMaterial
	vec3 mdiffuse = vec3(0.6,0.6,0.6);
	vec3 mspecular = vec3(1.0,1.0,1.0);
	float shininess = 32;
	
	vec3 lambient = vec3(0.2,0.2,0.2);	//gl_LightSource[0]
	vec3 ldiffuse = vec3(0.6,0.6,0.6);
	vec3 lspecular = vec3(1.0,1.0,1.0);

	float dist = length(fposition-lightPos);	//distance from light-source to surface
	float att=1.0/(1.0+0.1*dist+0.01*dist*dist);	//attenuation (constant,linear,quadric)
	vec3 ambient=mambient*lambient;	//the ambient light
	
	vec3 surf2light=normalize(lightPos-fposition);
	vec3 norm=normalize(fnormal);
	float dcont=max(0.0,dot(norm,surf2light));
	vec3 diffuse=dcont*(mdiffuse*ldiffuse);
	
	vec3 surf2view=normalize(-fposition);
	vec3 reflection=reflect(-surf2light,norm);
	
	float viewReflectDot = max(0.0, dot(surf2view,reflection));
	float scont;//=pow(float(viewReflectDot), float(shininess));
	if (shininess != 0.0) {
    	scont = pow( viewReflectDot, shininess);
	} else {
    	scont = 0.0;
  	}
	//scont = pow(scont, shininess);
	vec3 specular=scont*lspecular*mspecular;
*/
	gl_FragColor=colorVarying;//vec4(1.0, 1.0, 1.0, 1.0);//colorVarying + vec4((specular)*att,1.0);	//<- don't forget the paranthesis (ambient+diffuse+specular)
}
//*/