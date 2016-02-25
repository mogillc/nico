#version 300 es

//precision highp float;

in vec3 position;
in vec3 normal;
in vec3 color;
in vec3 tangent;
in vec2 UV;

uniform mat4 modelViewProjectionMatrix;


uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform highp mat4 viewMatrix;

uniform bool normalMapEnable;
//uniform vec3 lightPos;

uniform int nLights;
uniform highp vec3 lightPos[2];
uniform highp mat4 lightModelViewProjectionMatrix[2];

out highp vec3 lightPos2[2];
out highp vec4 lightVertexPosition[2];	// for shadows


out vec3 fposition;
out vec3 outColor;
out vec2 texcoord;

out mat3 TBN;

void main()
{
	gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);


	fposition = vec3( modelViewMatrix * vec4( position, 1.0));
	outColor = color ;

	//outNormal = vec3(0.0, 0.0, 1.0);//normal;

	// Copied from texture test:
	for (int i = 0; i < nLights; i++) {
		lightVertexPosition[i] = lightModelViewProjectionMatrix[i] * vec4( position, 1.0);
		lightPos2[i] = vec3( viewMatrix * vec4( lightPos[i], 1.0));
	}

	texcoord = UV;

	vec3 tangent;
	vec3 n=normal;//normalize( normalMatrix * normal );
	vec3 v1=cross( n, vec3(0.0,0.0,1.0));
	vec3 v2=cross( n, vec3(0.0,1.0,0.0));
	if(length(v1)>length(v2))
		tangent=v1;
	else
		tangent=v2;

	//vec3 t=normalize( tangent );
	n=normalize( normalMatrix *normal);
	vec3 t=normalize( normalMatrix * tangent);
	vec3 b=cross(n,t);
	//mat3 mat = mat3( t.x,b.x,n.x, t.y,b.y,n.y, t.z,b.z,n.z);
	TBN = mat3( t.x,b.x,n.x, t.y,b.y,n.y, t.z,b.z,n.z);
	//TBN = mat3( t, b, n);

	//mat=mat3(t, b, n);

	//vec3 vector;
	//for (int i = 0; i < nLights; i++) {
	//vector=normalize(lightPos2[i]-fposition);
	//vec3 vector=normalize(lightPos-position);
	//tangentSurface2light[i] = (mat)*vector;
	//}


	//vector=normalize(-fposition);
	//vector=normalize(-position);
	//tangentSurface2view = (mat)*vector;


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
