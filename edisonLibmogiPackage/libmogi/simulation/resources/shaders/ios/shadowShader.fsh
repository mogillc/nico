#version 300 es

precision highp float;
//#version 330

//uniform sampler2DShadow shadowMap;
uniform highp sampler2D shadowMap[2];

// Stuff copied from texture test:
uniform sampler2D normalMap[2];
uniform bool normalMapEnable;
uniform sampler2D heightMap[2];
uniform bool heightMapEnable;
uniform sampler2D specularityMap[2];
uniform bool specularityMapEnable;
uniform sampler2D colorMap[2];
uniform bool colorMapEnable;

//uniform float shininess;

uniform bool shadowEnable[2];
uniform float mapWidth[2];
uniform float mapHeight[2];

// Older shadow shader stuff:
uniform int nLights;	// There is a wierd issue with nLights...
uniform highp vec3 lightPos[2];

uniform lowp vec3 lightColor[2];


in highp vec3 lightPos2[2];
in highp vec4 lightVertexPosition[2];

in vec3 fposition;
in vec3 outColor;
in vec2 texcoord;

in mat3 TBN;

layout(location = 0) out vec4 FragColor;

void main()
{
//	gl_FragColor = vec4(1.0,0.5,0.25, 1.0);
//	return;
	//int l = 0;
	//vec3 lightPos2=vec3(gl_ModelViewMatrix*vec4(lightPos,1.0));
	//float shadowValue = shadow2DProj( shadowMap, lightVertexPosition ).r;	// r == 1.0 not varying shadow
	// r == 0.0 varying shadow
	// Determines the shadow value from the shadowMap:


	vec3 totalColor = vec3(0.0,0.0,0.0);
	vec2 coordinates;

	// Determines the new texture coordinate to use for sampling:
	vec2 newCoords;//= texcoord;
	//vec2 texcoord2 = texcoord;
	//texcoord2 /= texcoord2.w;
	vec3 tangentSurface2view = TBN * -fposition;
	vec3 eye = normalize(tangentSurface2view);
	if (heightMapEnable) {

		float height = texture(heightMap[0], texcoord).r;
		float v = height * 0.04 - 0.02;
		//vec3 eye = normalize(tangentSurface2view);
		//eye.y = -eye.y;
		//eye.x = -eye.x;
		newCoords = texcoord - (eye.xy * v);
	} else {
		newCoords = texcoord;//2.st;
	}

	//vec3 mambient = vec3(0.3,0.3,0.3);	//gl_FrontMaterial
	//vec3 mdiffuse = vec3(0.6,0.6,0.6);
	//vec3 mspecular = vec3( 1.0, 1.0, 1.0);
	float shininess = 32.0;

	//vec3 lambient = vec3(0.6,0.6,0.6);	//gl_LightSource[0]
	//vec3 ldiffuse = vec3(0.6,0.6,0.6);
	//vec3 lspecular = vec3(1.0,1.0,1.0);

	//vec3 lambient = vec3(0.6,0.492,0.419);	//gl_LightSource[0]
	//vec3 ldiffuse = vec3(0.6,0.492,0.419);
	//vec3 lspecular = vec3(1.0,1.0,1.0);		// Racist
	//vec3 lspecular = vec3(1.0,0.82,0.698);		// Sodium
	//vec3 lspecular = vec3(1.0,0.84,0.56);		// Tungsten 40W

	vec3 thisNormal;// = normalize( outNormal );

	float specularContribution;
	if (normalMapEnable) {
		thisNormal = normalize( (texture( normalMap[0], newCoords).xyz)*2.0 - 1.0);
		thisNormal.y = -thisNormal.y;
	} else {
		thisNormal = vec3( 0.0, 0.0, 1.0);//normalize( outNormal );
	}

	vec3 texcolor;
	if (colorMapEnable) {
		texcolor = vec3( texture( colorMap[0], newCoords) );
	} else {
		texcolor = outColor;
	}


	vec3 lightLookAt;

	for (int l = 0; l < 2; l++) {

		float shadowValue=1.0;

		if (shadowEnable[l] && (l == 0))
		{
			highp vec4 lightVertexPosition2 = lightVertexPosition[l];
			lightVertexPosition2 /= lightVertexPosition2.w;
			shadowValue=0.0;

//			float xIncrement = 1.0/mapWidth[l];
//			float yIncrement = 1.0/mapHeight[l];

			for( float x=-1.0; x <= 1.0; x+=1.0)
				for( float y=-1.0; y <= 1.0; y+=1.0)
				{
					vec2 texcoordNew = lightVertexPosition2.xy + vec2(x/mapWidth[l],y/mapHeight[l]);
					coordinates = texcoordNew;
					if ((texcoordNew.x > 1.0) || (texcoordNew.x < 0.0) || (texcoordNew.y > 1.0) || (texcoordNew.y < 0.0)) {
						//shadowValue += 1.0;
					} else {
						if( texture( shadowMap[0], texcoordNew).r >= lightVertexPosition2.z )	// TODO: Dynamic indexing of sampler types is not allowed
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
		} else {
			shadowValue = 0.75;
		}
		//shadowValue = 1.0;


//		return;

		lightLookAt = fposition-lightPos2[l];

		float dist = length(lightLookAt);	//distance from light-source to surface

		float att=1.0/(1.0+0.10*dist+0.1*dist*dist);	//attenuation (constant,linear,quadric)





		//vec3 surf2light = normalize( tangentSurface2light );
		//vec3 surf2camera = normalize( tangentSurface2view );

		vec3 surf2light = TBN * normalize( -lightLookAt );
		//vec3 surf2camera = eye;// normalize( tangentSurface2view );


		//vec3 surf2light = normalize( lightPos - fposition);
		//vec3 surf2camera = normalize( -fposition );
		//vec3 surf2light = tangentSurface2light;
		//vec3 surf2camera = tangentSurface2view;

		float diffuseContribution = max( 0.0, dot( thisNormal, surf2light));
		vec3 reflectionNew = reflect( -surf2light, thisNormal );
		specularContribution = pow( max(0.0, dot(reflectionNew, eye)), shininess);
//		specularContribution = pow( max(0.0, dot(reflectionNew, surf2camera)), shininess);
		specularContribution = pow( max(0.0, dot(reflectionNew, eye)), shininess);
		//float specularContribution = pow( max(0.0, dot(reflectionNew, surf2light)), shininess);

		if (specularityMapEnable) {
			specularContribution *= texture(specularityMap[0],newCoords).x;
		}

		//float clampedCosine = max(0.0, dot(-surf2light, light0.spotDirection));

		//gl_FragColor=vec4(outColor*ambient, 1.0) + shadowValue/2.0* (colorVarying + vec4((specular)*att,1.0));	//<- don't forget the paranthesis (ambient+diffuse+specular)



		vec3 ambient = texcolor * lightColor[l] * 0.05;// * (0.1/float(nLights));// * mambient;	//the ambient light


		vec3 diffuse = diffuseContribution * texcolor * lightColor[l];///float(nLights));
		vec3 specular = specularContribution * lightColor[l];// * mspecular;

		//gl_FragColor=vec4(ambientColor*texcolor+(shadowValue+0.05)*diffuseContribution *diffuseColor*texcolor+ att*(shadowValue<0.5 ? vec3(0.0,0.0,0.0) : specularContribution*specularColor*shadowValue), 1.0);
		totalColor += ambient + (diffuse+specular)*shadowValue/(dist*dist)*att;



	}
//	totalColor += texcolor * shadowValue;// / float(nLights);
//	gl_FragColor = vec4(totalColor, 1.0);// vec4(totalColor, 1.0 );
//	return;
	FragColor =vec4(totalColor*1.5, 1.0 );
//	gl_FragColor = vec4(totalColor, 1.0);// vec4(totalColor, 1.0 );
	//gl_FragColor =texture( shadowMap[0], coordinates);
	//gl_FragColor = vec4( (ambient+(diffuse+specular)*shadowValue)*att, 1.0 );
	
}
