#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 ex_color;
out vec3 ex_fragPos;

void main(void)
{
	mat4 viewModelMatrix = viewMatrix * modelMatrix;
	mat4 projectionViewModelMatrix = projectionMatrix * viewModelMatrix;

	ex_fragPos = (modelMatrix * vec4(in_position, 1.0f)).xyz;

	gl_Position = projectionViewModelMatrix * vec4(in_position, 1.0f);

//	float colorScale = (mat3(viewModelMatrix) * in_normal).z;
	float cosTheta = (mat3(modelMatrix) * in_normal).z;
	float theta = acos(cosTheta);
	float colorScale = max(0.0f, cos(theta * 0.75f)) + 0.1f;
	colorScale = min(1.0f, colorScale);

	ex_color = vec3(in_color) * colorScale;
}
