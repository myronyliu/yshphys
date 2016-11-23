#pragma once
#include "Vec3.h"
#include "Quat.h"

class MathUtils
{
public:
	MathUtils();
	virtual ~MathUtils();

	static Vec3 ClosestPointOnLineToPoint(const Vec3& a, const Vec3& b, const Vec3& pt);
	static Vec3 ClosestPointOnTriangleToPoint(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& pt);
	static Vec3 ClosestPointOnTetrahedronToPoint(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d, const Vec3& pt);
};

