// Simple fragment shader for shadowmapping:

varying highp vec3 fposition;
varying highp vec4 outcolor;

void main() {
	gl_FragColor=outcolor;//vec4( fposition.x, fposition.y, 0.5, 1.0);
}
