#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec3 out_fragColor;

uniform vec3 gEyePos;

uniform vec3 gLightPos;
uniform vec3 gLightInt;

uniform sampler2D gColorTex;

uniform sampler2D gPositionTex;
uniform sampler2D gNormalTex;

uniform sampler2D gDiffuseTex;
uniform sampler2D gSpecularTex;

void main()
{
	vec3 fragPos = texture(gPositionTex, ex_texCoord).xyz;

	vec3 vFragToLight = gLightPos - fragPos;
	float dFragToLight = length(vFragToLight);

	vec3 ambient = vec3(0.25f, 0.25f, 0.25f);

	vec3 normal = texture(gNormalTex, ex_texCoord).xyz;
	float cosTheta = dot(vFragToLight / dFragToLight, normal);
	vec3 kDiffuse = gLightInt * max(0.0f, cosTheta) / dFragToLight;

	vec3 diffuse = texture(gDiffuseTex, ex_texCoord).rgb * kDiffuse;

	// Blinn Phong
	float phongExp = 0.8f;
	vec3 L = normalize(gLightPos - fragPos);
	vec3 V = normalize(gEyePos - fragPos);
	vec3 H = normalize(L + V);
	vec3 kSpecular = gLightInt * pow(max(dot(H, normal), 0.0f), phongExp);
	vec3 specular = texture(gSpecularTex, ex_texCoord).rgb * kSpecular;

	out_fragColor = texture(gColorTex, ex_texCoord).rbg + diffuse + specular;
}
