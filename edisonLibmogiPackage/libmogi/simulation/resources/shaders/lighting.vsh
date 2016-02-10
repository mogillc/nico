#version 410

layout(location=0) in vec3 position;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 biasedModelViewProjectionMatrix;
uniform float uTanHalfFov;
uniform float uAspectRatio;

smooth out vec2 TexCoord;
//smooth out vec3 positionOut;
noperspective out vec3 viewRay;

void main()
{
	// Do nothing.
	vec3 positionOut = (modelViewProjectionMatrix*vec4(position,1.0)).xyz;
	TexCoord = (biasedModelViewProjectionMatrix*vec4(position,1.0)).st;

	viewRay = vec3(
					positionOut.x * uTanHalfFov * uAspectRatio,
					positionOut.y * uTanHalfFov,
					1.0
					);

	gl_Position = vec4(positionOut, 1.0);
}