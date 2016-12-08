#include "stdafx.h"
#include "BoundingBox.h"

bool AABB::Overlaps(const AABB& aabb) const
{
	if (max.x < aabb.min.x) { return false; }
	if (max.y < aabb.min.y) { return false; }
	if (max.z < aabb.min.z) { return false; }
	if (min.x > aabb.max.x) { return false; }
	if (min.y > aabb.max.y) { return false; }
	if (min.z > aabb.max.z) { return false; }
	return true;
}

AABB AABB::Aggregate(const AABB& aabb) const
{
	AABB aggregate;

	aggregate.min.x = (min.x < aabb.min.x) ? min.x : aabb.min.x;
	aggregate.min.y = (min.y < aabb.min.y) ? min.y : aabb.min.y;
	aggregate.min.z = (min.z < aabb.min.z) ? min.z : aabb.min.z;

	aggregate.max.x = (max.x > aabb.max.x) ? max.x : aabb.max.x;
	aggregate.max.y = (max.y > aabb.max.y) ? max.y : aabb.max.y;
	aggregate.max.z = (max.z > aabb.max.z) ? max.z : aabb.max.z;

	return aggregate;
}

double AABB::Area() const
{
	const double dx(max.x - min.x);
	const double dy(max.y - min.y);
	const double dz(max.z - min.z);

	return 2.0 * (dx*dy + dy*dz + dz*dx);
}