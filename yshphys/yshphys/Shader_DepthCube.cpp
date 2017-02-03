#include "stdafx.h"
#include "Shader_DepthCube.h"


Shader_DepthCube::Shader_DepthCube()
{
	Shader::LoadShader("DepthCube_vert.glsl", "DepthCube_geom.glsl", "DepthCube_frag.glsl");
}


Shader_DepthCube::~Shader_DepthCube()
{
}
