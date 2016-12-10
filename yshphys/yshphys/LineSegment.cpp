#include "stdafx.h"
#include "LineSegment.h"


LineSegment::LineSegment()
{
}


LineSegment::~LineSegment()
{
}

dVec3 LineSegment::Support(const dVec3& v) const
{
	const dVec3 vLocal = (-m_rot).Transform(v);
	const dVec3 supportLocal = dVec3(0.0, 0.0, m_halfLength*sgn(vLocal.z));
	return m_pos + m_rot.Transform(supportLocal);
}