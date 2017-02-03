#version 400

in vec3 ex_color;
in vec3 ex_fragPos;

out vec4 out_fragColor;

uniform vec3 pointLightPos;
uniform float pointLightFarPlane;

uniform samplerCube shadowCubeMap;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float CalcShadowFactor(vec3 lightToFrag)
{
	float shadow = 0.0f;
	float bias = 0.01f;
	int samples = 20;
	float currentDepth = length(lightToFrag);
	float diskRadius = 0.01f;

//	return texture(shadowCubeMap, lightToFrag).r;

	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowCubeMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
//		float closestDepth = texture(shadowCubeMap, lightToFrag).r;
		closestDepth *= pointLightFarPlane;

		if (currentDepth < pointLightFarPlane && currentDepth - bias > closestDepth)
		{
			shadow += 1.0f;
		}
	}
	shadow /= float(samples);  
	return 1.0f - shadow * 0.5f;
}

void main(void)
{
	vec3 lightToFrag = ex_fragPos - pointLightPos;
	float shadowFactor = CalcShadowFactor(lightToFrag);
	out_fragColor = vec4(ex_color * shadowFactor, 1.0f);
//	out_fragColor = vec4(shadowFactor, shadowFactor, shadowFactor, 1.0f);
}