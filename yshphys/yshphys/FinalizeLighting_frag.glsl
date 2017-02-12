#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec3 out_fragColor;

uniform vec3 gAmbient;

uniform sampler2D gColorTex;
uniform sampler2D gLightingStencil;

void main()
{
	out_fragColor = texture(gColorTex, ex_texCoord).rgb;

	if (texture(gLightingStencil, ex_texCoord).rgb != 0.0f)
	{
		out_fragColor += gAmbient;
	}
	out_fragColor.r = min(1.0f, out_fragColor.r);
	out_fragColor.g = min(1.0f, out_fragColor.g);
	out_fragColor.b = min(1.0f, out_fragColor.b);
}
