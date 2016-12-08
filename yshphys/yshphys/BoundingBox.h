#pragma once
#include "YshMath.h"

struct BoundingBox
{
	dVec3 min;
	dVec3 max;
};

struct AABB : public BoundingBox
{
	bool Overlaps(const AABB& aabb) const;
	AABB Aggregate(const AABB& aabb) const;
	double Area() const;
};