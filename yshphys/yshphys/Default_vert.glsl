#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

uniform vec3 pointLightPos;

out vec3 ex_color;
out vec3 ex_fragPos;

void main(void)
{
	mat4 viewModelMatrix = viewMatrix * modelMatrix;
	mat4 projectionViewModelMatrix = projectionMatrix * viewModelMatrix;

	ex_fragPos = (modelMatrix * vec4(in_position, 1.0f)).xyz;

	gl_Position = projectionViewModelMatrix * vec4(in_position, 1.0f);

	vec3 normal = mat3(modelMatrix) * in_normal;

	float cosTheta = dot(normalize(pointLightPos - ex_fragPos), normal);
	float colorScale = max(0.0f, cosTheta) + 0.25f;
	colorScale = min(1.0f, colorScale);

	ex_color = vec3(in_color) * colorScale;
}
