#pragma once
#include "Geometry.h"
class Sphere :
	public Geometry
{
public:
	Sphere();
	virtual ~Sphere();

	virtual Vec3 Support(const Vec3& v) const;

protected:
	float m_radius;
};

