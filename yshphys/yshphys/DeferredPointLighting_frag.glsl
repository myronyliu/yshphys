#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec4 out_fragColor;

uniform vec3 gLightPos;
uniform vec3 gLightInt;

uniform sampler2D gPositionTex;
uniform sampler2D gNormalTex;

uniform sampler2D gColorTex;

void main()
{
	vec3 fragPos = texture(gPositionTex, ex_texCoord).xyz;

	vec3 vFragToLight = gLightPos - fragPos;
	float dFragToLight = length(vFragToLight);

	vec3 ambient = vec3(0.25f, 0.25f, 0.25f);

	vec3 normal = texture(gNormalTex, ex_texCoord).xyz;
	float cosTheta = dot(vFragToLight / dFragToLight, normal);
	vec3 colorScale = gLightInt * max(0.0f, cosTheta) / dFragToLight + ambient;
	colorScale.x = min(1.0f, colorScale.x);
	colorScale.y = min(1.0f, colorScale.y);
	colorScale.z = min(1.0f, colorScale.z);


	out_fragColor = texture(gColorTex, ex_texCoord) * vec4(colorScale, 1.0f);
}
