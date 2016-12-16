#include "stdafx.h"
#include "LineSegment.h"


LineSegment::LineSegment()
{
}


LineSegment::~LineSegment()
{
}

dVec3 LineSegment::SupportLocal(const dVec3& v, bool& degenerate) const
{
	degenerate = v.z == 0.0;
	return dVec3(0.0, 0.0, m_halfLength*MathUtils::sgn(v.z));
}