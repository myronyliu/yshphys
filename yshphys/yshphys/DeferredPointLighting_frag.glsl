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

uniform sampler2D gLightingStencil;

void main()
{
	if (texture(gLightingStencil, ex_texCoord).r == 0.0f)
	{
		out_fragColor = texture(gDiffuseTex, ex_texCoord).rgb;
	}
	else
	{
		vec3 fragPos = texture(gPositionTex, ex_texCoord).xyz;
		vec3 N = texture(gNormalTex, ex_texCoord).xyz;

		vec3 vFragToLight = gLightPos - fragPos;
		float dFragToLight = length(vFragToLight);
		vec3 L = vFragToLight / dFragToLight;

		float cosTheta = dot(L, N);

		out_fragColor = texture(gColorTex, ex_texCoord).rgb;

		if (cosTheta > 0.0f)
		{
			vec3 kDiffuse = gLightInt * cosTheta / dFragToLight;
			vec3 diffuse = texture(gDiffuseTex, ex_texCoord).rgb * kDiffuse;
			out_fragColor += diffuse;
		}

		// Blinn Phong
		float phongExp = 4.0f;
		vec3 V = normalize(gEyePos - fragPos);
		vec3 H = normalize(L + V);
		float HN = dot(H, N);

		if (HN > 0.0f)
		{
			vec3 kSpecular = gLightInt * pow(max(dot(H, N), 0.0f), phongExp);
			vec3 specular = texture(gSpecularTex, ex_texCoord).rgb * kSpecular;
			out_fragColor += specular;
		}
	}
}
