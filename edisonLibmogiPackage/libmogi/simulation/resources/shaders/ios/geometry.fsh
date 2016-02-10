//#version 410
precision highp float;

// Simple fragment shader for shadowmapping:
uniform sampler2D normalMap[10];
uniform bool normalMapEnable;
uniform sampler2D heightMap[10];
uniform bool heightMapEnable;
uniform sampler2D specularityMap[10];
uniform bool specularityMapEnable;
uniform sampler2D colorMap[10];
uniform bool colorMapEnable;

//	surface properties:
//uniform vec3 uDiffuseColor = vec3(1.0);
//uniform float uSpecularLevel = 1.0;
//uniform float uSpecularExponent = 1.0;
//uniform float uMetallicLevel = 1.0;
//uniform vec2 uDetailTexScale = vec2(1.0);

vec3 uDiffuseColor = vec3(1.0);	// Bleh, defaults not allowed in GLES glsl I guess?
float uSpecularLevel = 1.0;
float uSpecularExponent = 1.0;
float uMetallicLevel = 1.0;
vec2 uDetailTexScale = vec2(1.0);

//smooth in vec3 Normal0;
//smooth in vec3 outColor;
//smooth in vec2 TexCoord0;
//smooth in mat3 TBN;
//smooth in vec3 fposition;

varying vec3 Normal0;
varying vec3 outColor;
varying vec2 TexCoord0;
varying mat3 TBN;
varying vec3 fposition;

//layout(location = 0) out vec4 FragColor; // TODO: look into deferred rendering methods
//layout(location = 1) out vec4 NormalOut; // TODO: look into deferred rendering methods
//layout(location = 2) out vec4 MaterialOut; // TODO: look into deferred rendering methods
vec4 NormalOut; // TODO: look into deferred rendering methods
vec4 MaterialOut; // TODO: look into deferred rendering methods

void main() {
	// Height map should be determined first for new texture coordinates:
	vec2 newCoords;
	vec3 tangentSurface2view = TBN * -fposition;
	vec3 eye = normalize(tangentSurface2view);
	if (heightMapEnable) {
		//float height = texture(heightMap[0], TexCoord0).r;
		float height = texture2D(heightMap[0], TexCoord0).r; // TODO check this
		float v = height * 0.04 - 0.02;

		newCoords = TexCoord0 - (eye.xy * v);
	} else {
		newCoords = TexCoord0;//2.st;
	}

	// Diffuse:
	if (colorMapEnable) {
		//FragColor = texture( colorMap[0], newCoords);
		gl_FragColor = texture2D( colorMap[0], newCoords); // TODO: look into deferred rendering methods
	} else {
		//FragColor= vec4( outColor, 1.0);
		gl_FragColor= vec4( outColor, 1.0); // TODO: look into deferred rendering methods
	}

	// Normals:
	vec3 normalResult;
	if(normalMapEnable) {
		//normalResult = (texture( normalMap[0], newCoords)).xyz;
		normalResult = (texture2D( normalMap[0], newCoords)).xyz; // TODO check this
		normalResult = TBN * normalize( (normalResult * 2.0 - 1.0));
	} else {
		normalResult = normalize(  (Normal0));// normalize(Normal0);
	}
	NormalOut.rgb = normalResult * 0.5 + 0.5;
	NormalOut.a = 1.0;


	// Specular:
	vec4 specular = vec4(uSpecularLevel, uSpecularExponent, uMetallicLevel, 0.0);
	if (specularityMapEnable) {
		//specular *= texture(specularityMap[0], newCoords) ;
		specular *= texture2D(specularityMap[0], newCoords) ; // TODO check this
	}

	float ks = specular.r;
	float ke = specular.g;
	float km = specular.b;
	MaterialOut = vec4(ks, ke, km, 1.0);
}
