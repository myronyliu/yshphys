#pragma once
#include "Vec3.h"
#include "Quat.h"

class MathUtils
{
public:
	MathUtils();
	virtual ~MathUtils();

	static Vec3 ClosestPointOnLineToPoint
	(
		const Vec3& a, bool& featureContainsA,
		const Vec3& b, bool& featureContainsB,
		const Vec3& pt
	);
	static Vec3 ClosestPointOnTriangleToPoint
	(
		const Vec3& a, bool& featureContainsA,
		const Vec3& b, bool& featureContainsB,
		const Vec3& c, bool& featureContainsC,
		const Vec3& pt
	);
	static Vec3 ClosestPointOnTetrahedronToPoint
	(
		const Vec3& a, bool& featureContainsA,
		const Vec3& b, bool& featureContainsB,
		const Vec3& c, bool& featureContainsC,
		const Vec3& d, bool& featureContainsD,
		const Vec3& pt
	);
};

