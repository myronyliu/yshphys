#include "stdafx.h"
#include "Shader_ShadowCubeMap.h"


Shader_ShadowCubeMap::Shader_ShadowCubeMap()
{
	Shader::LoadShader("ShadowCubeMap_vert.glsl", "ShadowCubeMap_geom.glsl", "ShadowCubeMap_frag.glsl");
}


Shader_ShadowCubeMap::~Shader_ShadowCubeMap()
{
}
