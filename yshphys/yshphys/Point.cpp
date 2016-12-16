#include "stdafx.h"
#include "Point.h"


Point::Point()
{
}


Point::~Point()
{
}

dVec3 Point::Support(const dVec3& pos, const dQuat& rot, const dVec3& v, bool& degenerate) const
{
	degenerate = false;
	return pos;
}
dVec3 Point::SupportLocal(const dVec3& v, bool& degenerate) const
{
	degenerate = false;
	return dVec3(0.0, 0.0, 0.0);
}
