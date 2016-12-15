#pragma once
#include "Geometry.h"
class Sphere :
	public Geometry
{
public:
	Sphere();
	virtual ~Sphere();

	void SetRadius(double m_radius);

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual dVec3 SupportLocal(const dVec3& v) const;

protected:
	double m_radius;
};

