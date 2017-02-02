#include "stdafx.h"
#include "Shader_Default.h"


Shader_Default::Shader_Default()
{
	Shader::LoadShader("Default_vert.glsl", "Default_frag.glsl");
}


Shader_Default::~Shader_Default()
{
}
