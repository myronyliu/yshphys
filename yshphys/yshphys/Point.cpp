#include "stdafx.h"
#include "Point.h"


Point::Point()
{
}


Point::~Point()
{
}

dVec3 Point::Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const
{
	return pos;
}
dVec3 Point::SupportLocal(const dVec3& v) const
{
	return dVec3(0.0, 0.0, 0.0);
}
Polygon Point::IntersectPlane(const dVec3& pos, const dQuat& rot, const dVec3& planeOrigin, const dQuat& planeOrientation) const
{
	Polygon poly;
	poly.AddVertex(dVec2(0.0, 0.0));
	return poly;
}
Polygon Point::IntersectPlaneLocal(const dVec3& planeOrigin, const dQuat& planeOrientation) const
{
	Polygon poly;
	poly.AddVertex(dVec2(0.0, 0.0));
	return poly;
}
