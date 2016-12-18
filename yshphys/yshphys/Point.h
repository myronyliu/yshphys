#pragma once
#include "Geometry.h"
class Point :
	public Geometry
{
public:
	Point();
	virtual ~Point();

	virtual SupportPolygon Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual SupportPolygon SupportLocal(const dVec3& v) const;
};

