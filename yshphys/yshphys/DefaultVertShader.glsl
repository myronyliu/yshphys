#version 450

in Vec3 in_position;
in Vec3 in_normal;
in Vec3 in_color;

out Vec3 out_color;

uniform mat4

void main()
{
	gl_Position = vec4( in_position.x, in_postion.y, in_position.z, 1 );
}
