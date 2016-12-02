#include "stdafx.h"
#include "Shader_Default.h"


Shader_Default::Shader_Default()
{
	Shader::LoadShader("DefaultVertexShader.glsl", "DefaultFragmentShader.glsl");
}


Shader_Default::~Shader_Default()
{
}
