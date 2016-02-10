#version 330

//uniform sampler2DShadow shadowMap;
uniform sampler2D shadowMap[10];

// Stuff copied from texture test:
uniform sampler2D normalMap[10];
uniform bool normalMapEnable;
uniform sampler2D heightMap[10];
uniform bool heightMapEnable;
uniform sampler2D specularityMap[10];
uniform bool specularityMapEnable;
uniform sampler2D colorMap[10];
uniform bool colorMapEnable;

//uniform float shininess;

uniform bool shadowEnable[10];
uniform float mapWidth[10];
uniform float mapHeight[10];

// Older shadow shader stuff:
uniform int nLights;
uniform vec3 lightPos[10];

uniform vec3 lightColor[10];


in vec3 lightPos2[10];
in vec4 lightVertexPosition[10];

in vec3 fposition;
in vec3 outColor;
in vec2 texcoord;

in mat3 TBN;

layout(location = 0) out vec4 FragColor;

void main()
{
	//int l = 0;
	//vec3 lightPos2=vec3(gl_ModelViewMatrix*vec4(lightPos,1.0));
	//float shadowValue = shadow2DProj( shadowMap, lightVertexPosition ).r;	// r == 1.0 not in shadow
	// r == 0.0 in shadow
	// Determines the shadow value from the shadowMap:


	vec3 totalColor = vec3(0.0,0.0,0.0);
	vec2 coordinates;

	// Determines the new texture coordinate to use for sampling:
	vec2 newCoords;
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


	for (int l = 0; l < nLights; l++) {

		float shadowValue=1.0;

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
						if( texture( shadowMap[l], texcoordNew).r >= lightVertexPosition2.z )
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
		//shadowValue = 1.0;





		vec3 lightLookAt = fposition-lightPos2[l];
		float dist = length(lightLookAt);	//distance from light-source to surface

		float att=1.0/(1.0+0.10*dist+0.01*dist*dist);	//attenuation (constant,linear,quadric)




		//vec3 surf2light = normalize( tangentSurface2light );
		//vec3 surf2camera = normalize( tangentSurface2view );

		vec3 surf2light = TBN * normalize( -lightLookAt );
		vec3 surf2camera = eye;// normalize( tangentSurface2view );


		//vec3 surf2light = normalize( lightPos - fposition);
		//vec3 surf2camera = normalize( -fposition );
		//vec3 surf2light = tangentSurface2light;
		//vec3 surf2camera = tangentSurface2view;

		float diffuseContribution = max( 0.0, dot( thisNormal, surf2light));
		vec3 reflectionNew = reflect( -surf2light, thisNormal );
		specularContribution = pow( max(0.0, dot(reflectionNew, surf2camera)), shininess);
		//float specularContribution = pow( max(0.0, dot(reflectionNew, surf2light)), shininess);

		if (specularityMapEnable) {
			specularContribution *= texture(specularityMap[0],newCoords).x;
		}

		//float clampedCosine = max(0.0, dot(-surf2light, light0.spotDirection));

		//gl_FragColor=vec4(outColor*ambient, 1.0) + shadowValue/2.0* (colorVarying + vec4((specular)*att,1.0));	//<- don't forget the paranthesis (ambient+diffuse+specular)



		vec3 ambient = texcolor * lightColor[l] * (0.1/float(nLights));// * mambient;	//the ambient light
		vec3 diffuse = diffuseContribution * texcolor * lightColor[l] * (0.9);///float(nLights));
		vec3 specular = specularContribution * lightColor[l];// * mspecular;

		//gl_FragColor=vec4(ambientColor*texcolor+(shadowValue+0.05)*diffuseContribution *diffuseColor*texcolor+ att*(shadowValue<0.5 ? vec3(0.0,0.0,0.0) : specularContribution*specularColor*shadowValue), 1.0);
		totalColor += ambient + (diffuse+specular)*shadowValue/(dist*dist)*att;
	}

	FragColor =vec4(totalColor, 1.0 );
	//gl_FragColor =texture( shadowMap[0], coordinates);
	//gl_FragColor = vec4( (ambient+(diffuse+specular)*shadowValue)*att, 1.0 );
	
}
