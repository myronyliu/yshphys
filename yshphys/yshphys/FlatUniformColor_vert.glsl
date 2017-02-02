#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform vec3 color;
uniform int useNormals;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 ex_color;

void main(void)
{
	mat4 viewModelMatrix = viewMatrix * modelMatrix;
	mat4 projectionViewModelMatrix = projectionMatrix * viewModelMatrix;
	gl_Position = projectionViewModelMatrix * vec4(in_position, 1.0f);
	ex_color = color;
	if (useNormals == 1)
	{
		float cosTheta = (mat3(viewModelMatrix) * in_normal).z;
		float theta = acos(cosTheta);
//		float colorScale = sqrt((1.0f + cosTheta) * 0.5f);
		float colorScale = cos(theta * 0.7f);
		ex_color *= colorScale;
	}
}
