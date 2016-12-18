#include "stdafx.h"
#include "LineSegment.h"


LineSegment::LineSegment()
{
}


LineSegment::~LineSegment()
{
}

SupportPolygon LineSegment::SupportLocal(const dVec3& v) const
{
	SupportPolygon poly;
	if (v.z*v.z / (v.x*v.x + v.y*v.y) < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
	{
		poly.nVertices = 2;
		poly.vertices[0] = dVec3(0.0, 0.0, -m_halfLength);
		poly.vertices[1] = dVec3(0.0, 0.0, m_halfLength);
	}
	else
	{
		poly.nVertices = 1;
		poly.vertices[0] = dVec3(0.0, 0.0, m_halfLength*MathUtils::sgn(v.z));
	}
	return poly;
}