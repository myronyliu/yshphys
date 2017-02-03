#version 400

layout (location = 0) in vec3 in_position;
// We don't actually need normals and color, but since they are already in the VAO, might as well load them
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 gProjection;
uniform mat4 gView;
uniform mat4 gModel;

out vec4 ex_fragPos;

void main(void)
{
    gl_Position = gProjection * gView * gModel * vec4(in_position, 1.0f);
	ex_fragPos = gModel * vec4(in_position, 1.0f);
}