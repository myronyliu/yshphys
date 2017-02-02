#version 400

in vec3 ex_fragPos;

uniform vec3 pointLightPos;
uniform float pointLightFarPlane;

//out float out_fragColor;

void main(void)
{
	vec3 LightToVertex = ex_fragPos - pointLightPos;

	gl_FragDepth = length(LightToVertex) / pointLightFarPlane;
}