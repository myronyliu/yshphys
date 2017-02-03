#version 400

in vec4 ex_fragPos;

uniform mat4 gView;

uniform float gNear;
uniform float gFar;

void main(void)
{
	float depth = -(gView * ex_fragPos).z;
	gl_FragDepth = (depth - gNear) / (gFar - gNear);
}