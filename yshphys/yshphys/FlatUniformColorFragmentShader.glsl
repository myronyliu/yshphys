#version 400

in vec3 ex_color;

out vec4 fragColor;

void main(void)
{
	fragColor = vec4(ex_color, 1.0f);
}