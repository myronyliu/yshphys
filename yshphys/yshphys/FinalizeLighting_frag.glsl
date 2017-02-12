#version 400

in vec2 ex_texCoord;

layout (location = 0) out vec3 out_fragColor;

uniform vec3 gAmbient;

uniform sampler2D gColorTex;

void main()
{
	out_fragColor = texture(gColorTex, ex_texCoord).rbg + gAmbient;
	out_fragColor.r = min(1.0f, out_fragColor.r);
	out_fragColor.g = min(1.0f, out_fragColor.g);
	out_fragColor.b = min(1.0f, out_fragColor.b);
}
