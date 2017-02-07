#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 gProjection;
uniform mat4 gView;
uniform mat4 gModel;

out vec4 ex_vertPosition;
out vec3 ex_vertNormal;
out vec3 ex_vertColor;

void main(void)
{
    gl_Position = gProjection * gView * gModel * vec4(in_position, 1.0f);
	ex_vertPosition = gModel * vec4(in_position, 1.0f);
	ex_vertNormal = mat3(gModel) * in_normal;
	ex_vertColor = in_color;
}