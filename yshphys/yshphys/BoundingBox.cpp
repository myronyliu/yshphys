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