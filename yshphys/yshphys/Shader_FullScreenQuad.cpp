#include "stdafx.h"
#include "Shader_FullScreenQuad.h"


Shader_FullScreenQuad::Shader_FullScreenQuad()
{
	LoadShader("FullScreenQuad_vert.glsl", "FullScreenQuad_geom.glsl", "FullScreenQuad_frag.glsl");
}


Shader_FullScreenQuad::~Shader_FullScreenQuad()
{
}
