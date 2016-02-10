// Very simple shader for shadowmapping:

attribute vec3 position;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

attribute vec4 color;

varying highp vec3 fposition;
varying lowp vec4 outcolor;

void main() {
	gl_Position = modelViewProjectionMatrix * vec4( position, 1.0);
	fposition = (modelViewMatrix * vec4(position, 1.0)).xyz;
	outcolor = color;
}
