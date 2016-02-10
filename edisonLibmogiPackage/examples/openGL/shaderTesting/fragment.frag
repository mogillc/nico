//http://www.youtube.com/user/thecplusplusguy
//normalmapping (bump-mapping) fragment.frag file (fragment-shader)
#version 120

uniform sampler2D img;
uniform sampler2D normalmap;
uniform sampler2D heightmap;
uniform sampler2D specularitymap;
varying vec2 texcoord;

varying vec3 position;
varying vec3 normal;

uniform vec3 lightPos;

uniform vec3 mambient;	//gl_FrontMaterial
uniform vec3 mdiffuse;
uniform vec3 mspecular;
uniform float shininess;

uniform vec3 lambient;	//gl_LightSource[0]
uniform vec3 ldiffuse;
uniform vec3 lspecular;

varying vec3 tangentSurface2light;
varying vec3 tangentSurface2view;

varying vec3 lightPos2;

//varying mat3 NormalMatrix;

//varying vec3 eyeVec;

//const vec2 scaleBias = {0.04, 0.02};
void main()
{
	// Determine new texture coordinates based on height map:
	//vec2 scaleBias = 0.04, 0.02;
	float height = texture2D(heightmap, texcoord.st).r;
	float v = height * 0.04 - 0.02;
	vec3 eye = normalize(tangentSurface2view);
	eye.x = -eye.x;
	vec2 newCoords = texcoord + (eye.xy * v);

	//vec3 lightPos2=vec3(gl_ModelViewMatrix*vec4(lightPos,1.0));
	vec3 texcolor=vec3(texture2D(img,newCoords));
	float dist=length(position-lightPos2);
	float att=1.0/(1.0+0.01*dist+0.001*dist*dist);
	vec3 ambient=texcolor*lambient;
	
	vec3 surf2light=normalize(tangentSurface2light);
	vec3 norm=normalize((texture2D(normalmap,newCoords).xyz)*2.0-1.0);
	float dcont=max(0.0,dot(norm,surf2light));
	vec3 diffuse=dcont*(texcolor*ldiffuse);
	
	vec3 surf2view=normalize(tangentSurface2view);
	vec3 reflection=reflect(-surf2light,norm);
	
	float scont=pow(max(0.0,dot(surf2view,reflection)),shininess);
	vec3 specular=(scont*lspecular) * texture2D(specularitymap,newCoords).xyz;
	
	gl_FragColor= vec4((ambient+diffuse+specular)*att,1.0);
	//gl_FragColor=vec4(tangentSurface2view,1.0);
}