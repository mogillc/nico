#version 330

// Very simple shader for shadowmapping:

in vec3 position;

uniform mat4 modelViewProjectionMatrix;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);
}
