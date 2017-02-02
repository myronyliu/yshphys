#include "stdafx.h"
#include "Shader_FlatUniformColor.h"


Shader_FlatUniformColor::Shader_FlatUniformColor()
{
	Shader::LoadShader("FlatUniformColor_vert.glsl", "FlatUniformColor_frag.glsl");
}


Shader_FlatUniformColor::~Shader_FlatUniformColor()
{
}
