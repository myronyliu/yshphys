#pragma once
#include "Geometry.h"
class Cylinder :
	public Geometry
{
public:
	Cylinder();
	~Cylinder();

	dVec3 Support(const dVec3& v) const;

protected:
	double m_halfHeight;
	double m_radius;
};

