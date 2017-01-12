#pragma once
#include "YshMath.h"

// TODO: Take Julio's recommendation for computing the closest distance between OOBBs so that
// we can bypass a lot of fine-grain computations for tilted objects with highly skewed aspect ratios

struct BoundingBox
{
	dVec3 min;
	dVec3 max;
	static double ClosestDistance(
		const BoundingBox& box0, const dVec3& pos0, const dQuat& rot0,
		const BoundingBox& box1, const dVec3& pos1, const dQuat& rot1);
};

struct AABB : public BoundingBox
{
	double ClosestDistance(const AABB& aabb) const;
	bool Overlaps(const AABB& aabb) const;
	AABB Aggregate(const AABB& aabb) const;
	double Area() const;
};