#version 400

in vec4 ex_vertPosition;
in vec3 ex_vertNormal;
in vec3 ex_vertColor;

uniform mat4 gView;

uniform float gNear;
uniform float gFar;

layout (location = 0) out vec3 out_fragColor;
layout (location = 1) out vec3 out_fragNormal;

void main(void)
{
	out_fragColor = ex_vertColor;
	out_fragNormal = ex_vertNormal;

	float depth = -(gView * ex_vertPosition).z;
	gl_FragDepth = (depth - gNear) / (gFar - gNear);
}