#pragma once
#include "YshMath.h"
#include "ShadowCubeMap.h"

struct Light
{
	fVec3 intensity;
};

struct PointLight : public Light
{
	fVec3 position;

	ShadowCubeMap shadowCubeMap;
};

struct DirectionalLight : public Light
{
	fVec3 direction;
};