#version 400

in vec3 ex_vertPosition;
in vec3 ex_vertNormal;
in vec3 ex_vertDiffuse;

layout (location = 0) out vec3 out_fragPosition;
layout (location = 1) out vec3 out_fragNormal;
layout (location = 2) out vec3 out_fragDiffuse;
layout (location = 4) out float out_lightingStencil;

uniform int gLit;

void main(void)
{
	out_fragPosition = ex_vertPosition;
	out_fragNormal = ex_vertNormal;
	out_fragDiffuse = ex_vertDiffuse;
	out_lightingStencil = float(gLit);
}