precision highp float;
//uniform sampler2DShadow shadowMap;
//uniform highp sampler2D shadowMap;
uniform float shadowXres;
uniform float shadowYres;

uniform sampler2D shadowMap[10];
uniform bool shadowEnable[10];
uniform float mapWidth[10];
uniform float mapHeight[10];

uniform sampler2D normalMap[10];
uniform bool normalMapEnable;
uniform sampler2D colorMap[10];
uniform bool colorMapEnable;

//uniform highp vec3 lightPos[1];
uniform int nLights;
uniform highp vec3 lightPos[10];
uniform highp vec3 lightColor[10];

varying vec3 lightPos2[10];
varying vec4 lightVertexPosition[10];



varying highp vec4 lightVertexPosition;

//varying vec4 colorVarying;
varying vec3 fposition;
//varying vec3 fnormal;

varying highp vec2 texcoord;

varying highp vec3 outNormal;
varying lowp vec3 outColor;

const lowp vec3 ambientColor=vec3(0.05,0.05,0.05);
const lowp vec3 diffuseColor=vec3(0.7,0.7,0.7);
const lowp vec3 specularColor=vec3(1.0,1.0,1.0);

void main()
{
	//float shadowValue = shadow2DProj( shadowMap, lightVertexPosition ).r;	// r == 1.0 not in shadow
								// r == 0.0 in shadow

	float shadowValue=0.0;
	highp vec4 lightVertexPosition2 = lightVertexPosition;
	lightVertexPosition2 /= lightVertexPosition2.w;
	float xIncrement = 1.0/shadowXres;
	float yIncrement = 1.0/shadowYres;
	float xMax = 1.5/shadowXres;
	float yMax = 1.5/shadowYres;
	float xBegin = -1.0/shadowXres;
	float yBegin = -1.0/shadowYres;
	float x, y;

	for( x = xBegin; x < xMax; x += xIncrement)
		for( y = yBegin; y <= yMax; y += yIncrement)
		{
			if( texture2D( shadowMap[0], lightVertexPosition2.xy + vec2(x,y)).r >= lightVertexPosition2.z )
				shadowValue += 1.0;
		}
	shadowValue *= 0.11111111;

//	gl_FragColor = vec4( shadowValue, .1, shadowValue, 1.0);



	highp float shininess = 32.0;
	highp vec3 thisNormal;// = normalize( outNormal );

	highp float specularContribution;
	if (normalMapEnable) {
		thisNormal = normalize( (texture2D( normalMap[0], texcoord).xyz)*2.0 - 1.0);
		thisNormal.y = -thisNormal.y;
	} else {
		thisNormal = normalize( outNormal );//vec3( 0.0, 0.0, 1.0);//
	}


	highp vec3 texcolor;
	if (colorMapEnable) {
		texcolor = vec3( texture2D( colorMap[0], texcoord) );
	} else {
		texcolor = outColor;
	}

	highp vec3 totalColor = vec3(0.0,0.0,0.0);


	for (int l = 0; l < nLights; l++) {

		if (shadowEnable[l])
		{
			vec4 lightVertexPosition2 = lightVertexPosition[l];
			lightVertexPosition2 /= lightVertexPosition2.w;
			shadowValue=0.0;

			float xIncrement = 1.0/mapWidth[l];
			float yIncrement = 1.0/mapHeight[l];

			for( float x=-1.0; x <= 1.0; x+=1.0)
				for( float y=-1.0; y <= 1.0; y+=1.0)
				{
					vec2 texcoordNew = lightVertexPosition2.xy + vec2(x*xIncrement,y*yIncrement);
					coordinates = texcoordNew;
					if ((texcoordNew.x > 1.0) || (texcoordNew.x < 0.0) || (texcoordNew.y > 1.0) || (texcoordNew.y < 0.0)) {
						//shadowValue += 1.0;
					} else {
						if( texture2D( shadowMap[l], texcoordNew).r >= lightVertexPosition2.z )
							shadowValue += 1.0;
					}
				}

			//for( float x=-1.0; x<1.0; x+=1.0)
			//	for( float y=-1.0; y<1.0; y+=1.0)
			//	{
			//		if( texture( shadowMap, lightVertexPosition2.xy + vec2(x,y)).r >= lightVertexPosition2.z )
			//			shadowValue += 1.0;
			//	}
			shadowValue/=9.0;
		}

		//lowp vec3 lambient = vec3(0.2,0.2,0.2);	//gl_LightSource[0]
		//lowp vec3 ldiffuse = vec3(0.6,0.6,0.6);
		//lowp vec3 lspecular = vec3(1.0,1.0,1.0);

		highp float dist = length(fposition-lightPos[l]);	//distance from light-source to surface
		highp float att=1.0/(1.0+0.1*dist+0.01*dist*dist);	//attenuation (constant,linear,quadric)
		//lowp vec3 ambient=mambient*lambient;	//the ambient light

		highp vec3 normal = normalize( thisNormal );
		highp vec3 surf2light = normalize( lightPos[l] - fposition);
		highp vec3 surf2camera = normalize( -fposition );
		highp vec3 reflectionNew = -reflect( surf2camera, normal );
		highp float diffuseContribution = max( 0.0, dot( normal, surf2light));
		highp float specularContribution = pow( max(0.0, dot(reflectionNew, surf2light)), shininess);

		highp vec3 ambient = texcolor * lightColor[l] * (0.1/float(nLights));// * mambient;	//the ambient light
		highp vec3 diffuse = diffuseContribution * texcolor * lightColor[l] * 3.0;///float(nLights));
		highp vec3 specular = specularContribution * lightColor[l] * 5.0;// * mspecular;

		totalColor += ambient + (diffuse+specular)*shadowValue/(dist*dist)*att;
	}

	//gl_FragColor=vec4(ambientColor*texcolor+(shadowValue+0.05)*diffuseContribution *diffuseColor*texcolor+ att*(shadowValue<0.5 ? vec3(0.0,0.0,0.0) : specularContribution*specularColor*shadowValue),1.0);

	gl_FragColor=vec4(totalColor,1.0);
}
