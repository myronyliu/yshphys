#include "stdafx.h"
#include "Shader_FinalizeLighting.h"


Shader_FinalizeLighting::Shader_FinalizeLighting()
{
	LoadShader("FinalizeLighting_vert.glsl", "FinalizeLighting_geom.glsl", "FinalizeLighting_frag.glsl");
}


Shader_FinalizeLighting::~Shader_FinalizeLighting()
{
}
