#version 400

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec3 in_color;

uniform mat4 gProjection;
uniform mat4 gView;
uniform mat4 gModel;

uniform vec3 pointLightPos;
uniform vec3 pointLightInt;

out vec3 ex_color;
out vec3 ex_fragPos;

void main(void)
{
	mat4 PVM = gProjection * gView * gModel;

	ex_fragPos = (gModel * vec4(in_position, 1.0f)).xyz;

	gl_Position = PVM * vec4(in_position, 1.0f);

	vec3 normal = mat3(gModel) * in_normal;

	vec3 vFragToLight = pointLightPos - ex_fragPos;
	float dFragToLight = length(vFragToLight);

	vec3 ambient = vec3(0.25f, 0.25f, 0.25f);

	float cosTheta = dot(vFragToLight / dFragToLight, normal);
	vec3 colorScale = pointLightInt * max(0.0f, cosTheta) / dFragToLight + ambient;
	colorScale.x = min(1.0f, colorScale.x);
	colorScale.y = min(1.0f, colorScale.y);
	colorScale.z = min(1.0f, colorScale.z);

	ex_color = vec3(in_color) * colorScale;
}
