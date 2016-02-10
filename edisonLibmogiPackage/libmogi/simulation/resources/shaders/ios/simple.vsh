//#version 330
precision highp float;	// OpenGLES maybe

// Very simple shader for shadowmapping:

//in vec3 position;
attribute vec3 position;

uniform mat4 modelViewProjectionMatrix;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);
}
