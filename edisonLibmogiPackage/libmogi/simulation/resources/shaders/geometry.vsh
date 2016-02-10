#version 410

// Very simple shader for shadowmapping:

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec2 UV;

uniform mat4 modelViewProjectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
//uniform mat4 viewMatrix;
smooth out vec3 Normal0;
smooth out vec3 outColor;
smooth out vec2 TexCoord0;
smooth out vec3 fposition;

smooth out mat3 TBN;

void main() {
	fposition = vec3( modelViewMatrix * vec4( position, 1.0));
	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);
	Normal0 = (modelViewMatrix*vec4(normal, 0.0)).xyz;// * 0.5 + 0.5;
	//Normal0 = normalMatrix*normal;// * 0.5 + 0.5;

	outColor = color;
	TexCoord0 = UV;


	vec3 n = normalize( (modelViewMatrix * vec4(normal, 0.0)).xyz);
	vec3 t = normalize( (modelViewMatrix * vec4(tangent, 0.0)).xyz);
	//n=normalize(  normalMatrix*normal );
	//vec3 t=normalize(  normalMatrix*tangent);
	vec3 b = cross( t, n);
	TBN = mat3( t, b, n);
}
