#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 ex_color;

void main(void)
{
	const mat4 projectionViewModelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = projectionViewModelMatrix * vec4(in_position, 1.0f);
	ex_color = vec3(in_color);
}
