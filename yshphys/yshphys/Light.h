#pragma once
#include "YshMath.h"
#include "DepthCubeMap.h"

struct Light
{
	fVec3 intensity;
};

struct PointLight : public Light
{
	fVec3 position;

	DepthCubeMap shadowCubeMap;
};

struct DirectionalLight : public Light
{
	fVec3 direction;
};