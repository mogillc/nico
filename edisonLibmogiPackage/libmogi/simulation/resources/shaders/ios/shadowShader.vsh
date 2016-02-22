precision highp float;

attribute highp vec3 position;
attribute highp vec3 normal;
attribute highp vec4 color;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 lightModelViewProjectionMatrix;

uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
//uniform vec3 lightPos;

varying highp vec4 lightVertexPosition;

//varying vec4 colorVarying;
varying vec3 fposition;
//varying vec3 fnormal;

varying vec3 outNormal;
varying lowp vec3 outColor;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);
	lightVertexPosition = lightModelViewProjectionMatrix * vec4( position, 1.0);
	vec4 tempvertPosition = modelViewMatrix * vec4(position, 1.0);
	fposition = tempvertPosition.xyz;
	outColor = color.xyz ;
	outNormal = normalMatrix * normal;
/*
	vec3 eyeNormal = normalize(outNormal);

	vec3 vertPosition = position;
	vec3 lightPosition = lightPos;
	vec4 diffuseColor = color;
	float nDotVP3 = max(0.0, dot(eyeNormal, normalize(lightPos)))*0.1;
	
	colorVarying = diffuseColor * nDotVP3;	// very dim ambient lighting (so I know things are alive

	//vec4 g_gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);


	vertPosition = vec3(tempvertPosition[0], tempvertPosition[1], tempvertPosition[2]);
	//vertPosition = vec3(g_gl_Position[0], g_gl_Position[1], g_gl_Position[2]);
	float lightIntensity = length(vertPosition - lightPosition);
	lightIntensity = 60.0 / (lightIntensity * lightIntensity);
	float nDotPointSource = max(0.0, lightIntensity * dot( eyeNormal, normalize( lightPosition - vertPosition)));
	colorVarying += diffuseColor * nDotPointSource;


	
	fnormal = eyeNormal;
*/
	

}
