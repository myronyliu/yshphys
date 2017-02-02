#version 400

in vec3 ex_color;
in vec3 ex_fragPos;

out vec4 fragColor;

uniform vec3 pointLightPos;
uniform samplerCube shadowCubeMap;

float CalcShadowFactor(vec3 LightDirection)
{
	float SampledDistance = texture(shadowCubeMap, LightDirection).r;

	float Distance = length(LightDirection);

	if (Distance < SampledDistance + 0.01f)
	{
		return 1.0f; // Inside the light
	}
    else
	{
		return 0.5f; // Inside the shadow
	}
}

void main(void)
{
	vec3 lightDirection = ex_fragPos - pointLightPos;
	float shadowFactor = CalcShadowFactor(lightDirection);
	fragColor = vec4(ex_color * shadowFactor, 1.0f);
}