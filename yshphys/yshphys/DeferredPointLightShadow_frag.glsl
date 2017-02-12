#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec4 out_fragColor;

uniform vec3 gLightPos;

uniform float gNear_light;
uniform float gFar_light;

uniform sampler2D gPositionTex;
uniform sampler2D gColorTex;

uniform samplerCube gShadowCubeMap;

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

	for (int i = 0; i < samples; ++i)
	{
		float alpha = texture(gShadowCubeMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
//		float alpha = texture(gShadowCubeMap, lightToFrag).r;
		float closestDepth = (1.0f - alpha) * gNear_light + alpha * gFar_light;

		if (currentDepth < gFar_light && currentDepth - bias > closestDepth)
		{
			shadow += 1.0f;
		}
	}
	shadow /= float(samples);  
	return 1.0f - shadow * 0.5f;
}

void main()
{
	vec3 fragPos = texture(gPositionTex, ex_texCoord).xyz;

	vec3 lightToFrag = fragPos - gLightPos;

	out_fragColor = texture(gColorTex, ex_texCoord) * CalcShadowFactor(lightToFrag);
//	out_fragColor.r = CalcShadowFactor(lightToFrag);
//	out_fragColor.g = 0.0f;
//	out_fragColor.b = 0.0f;
//	out_fragColor.a = 1.0f;
}
