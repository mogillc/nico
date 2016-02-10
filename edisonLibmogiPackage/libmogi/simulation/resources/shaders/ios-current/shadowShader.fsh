precision highp float;
//uniform sampler2DShadow shadowMap;
uniform highp sampler2D shadowMap;
uniform float shadowXres;
uniform float shadowYres;



uniform vec3 lightPos;

varying highp vec4 lightVertexPosition;

//varying vec4 colorVarying;
varying vec3 fposition;
//varying vec3 fnormal;

varying vec3 outNormal;
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
			if( texture2D( shadowMap, lightVertexPosition2.xy + vec2(x,y)).r >= lightVertexPosition2.z )
				shadowValue += 1.0;
		}
	shadowValue/=9.0;

//	gl_FragColor = vec4( shadowValue, .1, shadowValue, 1.0);



	lowp vec3 mambient = vec3(0.25,0.25,0.25);	//gl_FrontMaterial
	//lowp vec3 mdiffuse = vec3(0.6,0.6,0.6);
	//lowp vec3 mspecular = vec3(1.0,1.0,1.0);
	float shininess = 32.0;

	lowp vec3 lambient = vec3(0.2,0.2,0.2);	//gl_LightSource[0]
	//lowp vec3 ldiffuse = vec3(0.6,0.6,0.6);
	//lowp vec3 lspecular = vec3(1.0,1.0,1.0);

	float dist = length(fposition-lightPos);	//distance from light-source to surface
	float att=1.0/(1.0+0.1*dist+0.01*dist*dist);	//attenuation (constant,linear,quadric)
	lowp vec3 ambient=mambient*lambient;	//the ambient light

	vec3 normal = normalize( outNormal );
	vec3 surf2light = normalize( lightPos - fposition);
	vec3 surf2camera = normalize( -fposition );
	vec3 reflectionNew = -reflect( surf2camera, normal );
	float diffuseContribution = max( 0.0, dot( normal, surf2light));
	float specularContribution = pow( max(0.0, dot(reflectionNew, surf2light)), shininess);
/*
	vec3 norm=normalize( outNormal );
	float dcont=max(0.0,dot(norm,surf2light));
	vec3 diffuse=dcont*(mdiffuse*ldiffuse);

	vec3 surf2view=normalize(-fposition);
	vec3 reflection=reflect(-surf2light,norm);

	float viewReflectDot = max(0.0, dot(surf2view,reflection));
	float scont;//=pow(float(viewReflectDot), float(shininess));
	if (shininess != 0.0) {
    	scont = pow( viewReflectDot, shininess);
	} else {
    	scont = 0.0;
  	}
	//scont = pow(scont, shininess);
	vec3 specular=scont*lspecular*mspecular;
*/
	//gl_FragColor=vec4(outColor*ambient, 1.0) + shadowValue/2.0* (colorVarying + vec4((specular)*att,1.0));	//<- don't forget the paranthesis (ambient+diffuse+specular)
	gl_FragColor=vec4(ambientColor*outColor+(shadowValue+0.05)*diffuseContribution *diffuseColor*outColor+ att*(shadowValue<0.5 ? vec3(0.0,0.0,0.0) : specularContribution*specularColor*shadowValue),1.0);

}
