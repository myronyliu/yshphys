#version 400

in vec4 ex_vertPosition;
in vec3 ex_vertNormal;
in vec3 ex_vertColor;

layout (location = 0) out vec3 out_fragPosition;
layout (location = 1) out vec3 out_fragNormal;
layout (location = 2) out vec3 out_fragColor;

void main(void)
{
	out_fragPosition = ex_vertPosition;
	out_fragColor = ex_vertColor;
	out_fragNormal = ex_vertNormal;
}