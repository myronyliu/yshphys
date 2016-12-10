#include "stdafx.h"
#include "Shader_FlatUniformColor.h"


Shader_FlatUniformColor::Shader_FlatUniformColor()
{
	Shader::LoadShader("FlatUniformColorVertexShader.glsl", "FlatUniformColorFragmentShader.glsl");
}


Shader_FlatUniformColor::~Shader_FlatUniformColor()
{
}
