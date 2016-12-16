#pragma once
#include "Geometry.h"
class Sphere :
	public Geometry
{
public:
	Sphere();
	virtual ~Sphere();

	void SetRadius(double m_radius);

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v, bool& degenerate) const;
	virtual dVec3 SupportLocal(const dVec3& v, bool& degenerate) const;

	virtual bool RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hitPt) const;

protected:
	double m_radius;
};

