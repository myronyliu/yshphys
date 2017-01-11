#pragma once
#include "Geometry.h"
class Point :
	public Geometry
{
public:
	Point();
	virtual ~Point();

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual dVec3 SupportLocal(const dVec3& v) const;
	
	virtual Polygon IntersectPlane(const dVec3& pos, const dQuat& rot, const dVec3& planeOrigin, const dQuat& planeOrientation) const;
	virtual Polygon IntersectPlaneLocal(const dVec3& planeOrigin, const dQuat& planeOrientation) const;

};

