#include "stdafx.h"
#include "Shader_DeferredPointLight.h"


Shader_DeferredPointLight::Shader_DeferredPointLight()
{
	LoadShader("DeferredPointLighting_vert.glsl", "DeferredPointLighting_geom.glsl", "DeferredPointLighting_frag.glsl");
}


Shader_DeferredPointLight::~Shader_DeferredPointLight()
{
}
