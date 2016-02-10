#version 410

uniform sampler2D uGBufferDiffuseTex;
uniform sampler2D uGBufferMaterialTex;
uniform sampler2D uGBufferGeometricTex;	// normals
uniform sampler2D uGBufferDepthTex;


smooth in vec2 TexCoord;
//smooth in vec3 positionOut;
noperspective in vec3 viewRay;

layout(location = 0) out vec4 FragColor;

void main()
{
	// Diffuse:
	vec4 kd = texture(uGBufferDiffuseTex, TexCoord);

	// Material:
	vec4 material = texture(uGBufferMaterialTex, TexCoord);
	float ks = material.r;
	float ke = material.g;
	float km = material.b * 0.5;

	// Geometry:
	vec4 geometric = texture(uGBufferGeometricTex, TexCoord);
	vec3 normal = geometric.xyz * 2.0 - 1.0;
	float ko = geometric.a; // local occlusion

	// depth doesn't need to be linear in this case
	vec3 position = viewRay * texture(uGBufferDepthTex, TexCoord).r;

	vec4 ld = vec4(1.0);//textureLod(uEnvTex, (uInverseViewMatrix * vec4(n, 0.0)).xyz, uEnvTexMaxLod);

	vec3 r = reflect(position, normal);
	float lod = pow(1.0 - ke, 0.5); // linearize, since mips aren't linearly spaced
	vec4 ls = vec4(1.0);//textureLod(uEnvTex, (uInverseViewMatrix * vec4(r, 0.0)).xyz, uEnvTexMaxLod * lod);

	vec4 intensity = mix(kd * ld + ks * ls, ks * kd * ls, km) * ko;

	FragColor = intensity;
}
