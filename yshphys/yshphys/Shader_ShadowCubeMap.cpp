#include "stdafx.h"
#include "Shader_ShadowCubeMap.h"


Shader_ShadowCubeMap::Shader_ShadowCubeMap()
{
	Shader::LoadShader("ShadowCubeMapVertexShader.glsl", "ShadowCubeMapFragmentShader.glsl");
}


Shader_ShadowCubeMap::~Shader_ShadowCubeMap()
{
}
