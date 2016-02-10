//#version 330
precision highp float;	// OpenGLES maybe

//in vec3 position;
attribute vec3 position;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 biasedModelViewProjectionMatrix;

//out vec4 TexCoord[1];
varying vec4 TexCoord[1];

void main()
{
	// Do nothing.
	gl_Position=modelViewProjectionMatrix*vec4(position,1.0);
	TexCoord[0]=biasedModelViewProjectionMatrix*vec4(position,1.0);
	//gl_Position=modelViewProjectionMatrix*gl_Vertex;
	//gl_Position = gl_Vertex;
}