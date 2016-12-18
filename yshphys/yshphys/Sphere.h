#pragma once
#include "Geometry.h"
class Sphere :
	public Geometry
{
public:
	Sphere();
	virtual ~Sphere();

	void SetRadius(double m_radius);

	virtual SupportPolygon Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual SupportPolygon SupportLocal(const dVec3& v) const;

	virtual bool RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hitPt) const;

protected:
	double m_radius;
};

