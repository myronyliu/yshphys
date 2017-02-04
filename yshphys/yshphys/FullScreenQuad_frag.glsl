#version 400

in vec2 ex_texCoord;

out vec4 out_fragColor;

uniform sampler2D gImage;

void main()
{
	out_fragColor = texture(gImage, ex_texCoord);
}
