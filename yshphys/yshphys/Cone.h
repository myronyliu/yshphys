#pragma once
#include "Geometry.h"
class Cone :
	public Geometry
{
public:
	Cone();
	virtual ~Cone();

	void SetHeight(double halfHeight);
	void SetRadius(double radius);

	virtual dVec3 SupportLocal(const dVec3& v) const;

	virtual Polygon IntersectPlaneLocal(const dVec3& planeOrigin, const dVec3& planeNormal, const dVec3& xAxis, const dVec3& yAxis) const;

protected:

	double m_height;
	double m_radius;
};

