#version 400

in vec3 ex_fragPos;

uniform vec3 pointLightPos;

out float out_fragColor;

void main(void)
{
	vec3 LightToVertex = ex_fragPos - pointLightPos;

	out_fragColor = length(LightToVertex);
}