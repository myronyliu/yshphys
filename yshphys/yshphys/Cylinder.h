#pragma once
#include "Geometry.h"
class Cylinder :
	public Geometry
{
public:
	Cylinder();
	~Cylinder();

	Vec3 Support(const Vec3& v) const;

protected:
	double m_halfHeight;
	double m_radius;
};

