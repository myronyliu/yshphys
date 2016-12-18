#pragma once
#include "YshMath.h"

#define MAX_SUPPORT_POLYGON_VERTICES 16

#define ZERO_ANGLE_THRESH 0.001

struct SupportPolygon
{
	int nVertices;
	dVec3 vertices[MAX_SUPPORT_POLYGON_VERTICES];

	static double ComputeSeparation(const SupportPolygon& poly0, dVec3& pt0, const SupportPolygon& poly1, dVec3& pt1);
};
