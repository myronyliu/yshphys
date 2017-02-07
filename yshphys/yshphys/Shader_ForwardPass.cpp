#include "stdafx.h"
#include "Shader_ForwardPass.h"


Shader_ForwardPass::Shader_ForwardPass()
{
	LoadShader("ForwardPass_vert.glsl", "ForwardPass_frag.glsl");
}


Shader_ForwardPass::~Shader_ForwardPass()
{
}
