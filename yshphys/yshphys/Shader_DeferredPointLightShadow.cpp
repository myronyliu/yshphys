#include "stdafx.h"
#include "Shader_DeferredPointLightShadow.h"


Shader_DeferredPointLightShadow::Shader_DeferredPointLightShadow()
{
	LoadShader("DeferredPointLightShadow_vert.glsl", "DeferredPointLightShadow_geom.glsl", "DeferredPointLightShadow_frag.glsl");
}


Shader_DeferredPointLightShadow::~Shader_DeferredPointLightShadow()
{
}
