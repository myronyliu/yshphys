#include "stdafx.h"
#include "Shader_DeferredPointLight.h"


Shader_DeferredPointLight::Shader_DeferredPointLight()
{
	LoadShader("DeferredLighting_vert.glsl", "DeferredLighting_geom.glsl", "DeferredLighting_frag.glsl");
}


Shader_DeferredPointLight::~Shader_DeferredPointLight()
{
}
