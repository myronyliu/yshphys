#include "stdafx.h"
#include "LineSegment.h"


LineSegment::LineSegment()
{
}


LineSegment::~LineSegment()
{
}

dVec3 LineSegment::SupportLocal(const dVec3& v) const
{
	return dVec3(0.0, 0.0, m_halfLength*MathUtils::sgn(v.z));
}