#include "stdafx.h"
#include "Shader_DepthPerspective.h"


Shader_DepthPerspective::Shader_DepthPerspective()
{
	Shader::LoadShader("DepthPerspective_vert.glsl", "DepthPerspective_frag.glsl");
}


Shader_DepthPerspective::~Shader_DepthPerspective()
{
}
