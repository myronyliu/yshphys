#pragma once
#include "Geometry.h"
class Point :
	public Geometry
{
public:
	Point();
	virtual ~Point();

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v, bool& degenerate) const;
	virtual dVec3 SupportLocal(const dVec3& v, bool& degenerate) const;
};

