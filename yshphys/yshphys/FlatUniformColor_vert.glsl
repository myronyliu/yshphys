#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

out vec3 ex_vertPosition;
out vec3 ex_vertNormal;
out vec3 ex_vertDiffuse;

uniform vec3 gDiffuse;
uniform mat4 gProjection;
uniform mat4 gView;
uniform mat4 gModel;

void main(void)
{
	gl_Position = gProjection * gView * gModel * vec4(in_position, 1.0f);
	ex_vertPosition = (gModel * vec4(in_position, 1.0f)).xyz;
	ex_vertNormal = mat3(gModel) * in_normal;
	ex_vertDiffuse = gDiffuse;
}
