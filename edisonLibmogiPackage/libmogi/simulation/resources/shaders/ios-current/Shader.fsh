//
//  Shader.fsh
//  OpenGL 2
//
//  Created by Mr Roboto on 5/29/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

uniform sampler2D normalMap[10];
uniform bool normalMapEnable;
uniform sampler2D colorMap[10];
uniform bool colorMapEnable;

//uniform highp vec3 lightPos[1];
uniform int nLights;
uniform highp vec3 lightPos[10];
uniform highp vec3 lightColor[10];

//varying lowp vec4 colorVarying;

varying highp vec4 lightVertexPosition;

varying highp vec3 fposition;

varying highp vec3 outNormal;
varying lowp vec3 outColor;

varying highp vec2 texcoord;

const lowp vec3 ambientColor=vec3(0.05,0.05,0.05);
const lowp vec3 diffuseColor=vec3(0.7,0.7,0.7);
const lowp vec3 specularColor=vec3(1.0,1.0,1.0);

void main()
{
//	const highp vec3 lightPos = vec3(-5.0, 5.0, 4.5);

	highp float shadowValue=1.0;

	//lowp vec3 mambient = vec3(0.2,0.2,0.2);	//gl_FrontMaterial
	//lowp vec3 mdiffuse = vec3(0.6,0.6,0.6);
	//lowp vec3 mspecular = vec3(1.0,1.0,1.0);
	highp float shininess = 32.0;

	highp vec3 thisNormal;// = normalize( outNormal );

	highp float specularContribution;
	if (normalMapEnable) {
		thisNormal = normalize( (texture2D( normalMap[0], texcoord).xyz)*2.0 - 1.0);
		thisNormal.y = -thisNormal.y;
	} else {
		thisNormal = normalize( outNormal );//vec3( 0.0, 0.0, 1.0);//
	}


	highp vec3 texcolor;
	if (colorMapEnable) {
		texcolor = vec3( texture2D( colorMap[0], texcoord) );
	} else {
		texcolor = outColor;
	}

	highp vec3 totalColor = vec3(0.0,0.0,0.0);


	for (int l = 0; l < nLights; l++) {
	//lowp vec3 lambient = vec3(0.2,0.2,0.2);	//gl_LightSource[0]
	//lowp vec3 ldiffuse = vec3(0.6,0.6,0.6);
	//lowp vec3 lspecular = vec3(1.0,1.0,1.0);

	highp float dist = length(fposition-lightPos[l]);	//distance from light-source to surface
	highp float att=1.0/(1.0+0.1*dist+0.01*dist*dist);	//attenuation (constant,linear,quadric)
	//lowp vec3 ambient=mambient*lambient;	//the ambient light

	highp vec3 normal = normalize( thisNormal );
	highp vec3 surf2light = normalize( lightPos[l] - fposition);
	highp vec3 surf2camera = normalize( -fposition );
	highp vec3 reflectionNew = -reflect( surf2camera, normal );
	highp float diffuseContribution = max( 0.0, dot( normal, surf2light));
	highp float specularContribution = pow( max(0.0, dot(reflectionNew, surf2light)), shininess);

		highp vec3 ambient = texcolor * lightColor[l] * (0.1/float(nLights));// * mambient;	//the ambient light
		highp vec3 diffuse = diffuseContribution * texcolor * lightColor[l] * 3.0;///float(nLights));
		highp vec3 specular = specularContribution * lightColor[l] * 5.0;// * mspecular;

		totalColor += ambient + (diffuse+specular)*shadowValue/(dist*dist)*att;
	}

	//gl_FragColor=vec4(ambientColor*texcolor+(shadowValue+0.05)*diffuseContribution *diffuseColor*texcolor+ att*(shadowValue<0.5 ? vec3(0.0,0.0,0.0) : specularContribution*specularColor*shadowValue),1.0);

	gl_FragColor=vec4(totalColor,1.0);
	
    //gl_FragColor = vec4( outColor * 0.9 + outNormal, 1.0);
}

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