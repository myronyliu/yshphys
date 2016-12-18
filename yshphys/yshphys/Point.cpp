#include "stdafx.h"
#include "Point.h"


Point::Point()
{
}


Point::~Point()
{
}

SupportPolygon Point::Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const
{
	SupportPolygon poly;
	poly.nVertices = 1;
	poly.vertices[0] = pos;
	return poly;
}
SupportPolygon Point::SupportLocal(const dVec3& v) const
{
	SupportPolygon poly;
	poly.nVertices = 1;
	poly.vertices[0] = dVec3(0.0, 0.0, 0.0);
	return poly;
}
