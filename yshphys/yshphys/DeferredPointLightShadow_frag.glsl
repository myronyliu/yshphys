#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec4 out_fragColor;

uniform vec3 gLightPos;

uniform mat4 gViewInv;
uniform float gFOV;
uniform float gAspect;
uniform float gNear_eye;
uniform float gFar_eye;

uniform float gNear_light;
uniform float gFar_light;

uniform sampler2D gDepthTex;
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

	return texture(gShadowCubeMap, lightToFrag).r;

	for (int i = 0; i < samples; ++i)
	{
//		float closestDepth = texture(gShadowCubeMap, lightToFrag + sampleOffsetDirections[i] * diskRadius).r;
		float alpha = texture(gShadowCubeMap, lightToFrag).r;
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
	float alpha = texture(gDepthTex, ex_texCoord).x;
	if (alpha >= 1.0f)
	{
		out_fragColor = texture(gColorTex, ex_texCoord);
	}
	else
	{
		float xAng = (ex_texCoord.x - 0.5f) * gFOV * gAspect;
		float yAng = (ex_texCoord.y - 0.5f) * gFOV;
		float z = (1.0f - alpha) * gNear_eye + alpha * gFar_eye;
		float x = z * tan(xAng);
		float y = z * tan(yAng);
		vec3 fragPos = (inverse(gViewInv) * vec4(x, y, -z, 1.0f)).xyz;

		vec3 lightToFrag = fragPos - gLightPos;

	//	out_fragColor = texture(gColorTex, ex_texCoord) * CalcShadowFactor(lightToFrag);
		out_fragColor.r = CalcShadowFactor(lightToFrag);
		out_fragColor.g = 0.0f;
		out_fragColor.b = 0.0f;
		out_fragColor.a = 1.0f;
	}
}
