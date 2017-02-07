#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec4 out_fragColor;

uniform vec3 gLightPos;
uniform vec3 gLightInt;

uniform mat4 gViewInv;
uniform float gFOV;
uniform float gAspect;
uniform float gNear;
uniform float gFar;

uniform sampler2D gDepthTex;
uniform sampler2D gNormalTex;

uniform sampler2D gColorTex;

void main()
{
	float alpha = texture(gDepthTex, ex_texCoord).x;
	float xAng = (ex_texCoord.x - 0.5f) * gFOV * gAspect;
	float yAng = (ex_texCoord.y - 0.5f) * gFOV;
	float z = (1.0f - alpha) * gNear + alpha * gFar;
	float x = z * tan(xAng);
	float y = z * tan(yAng);
	vec3 fragPos = (inverse(gViewInv) * vec4(x, y, -z, 1.0f)).xyz;

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
