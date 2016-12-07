#pragma once
#include "Geometry.h"
class Sphere :
	public Geometry
{
public:
	Sphere();
	virtual ~Sphere();

	void SetRadius(double m_radius);

	virtual dVec3 Support(const dVec3& v) const;

protected:
	double m_radius;
};

