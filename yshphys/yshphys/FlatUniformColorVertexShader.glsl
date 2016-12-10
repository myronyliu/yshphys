#version 400

layout (location = 0) in vec3 in_position;

uniform vec3 color;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 ex_color;

void main(void)
{
	mat4 projectionViewModelMatrix = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = projectionViewModelMatrix * vec4(in_position, 1.0f);
	ex_color = color;
}
