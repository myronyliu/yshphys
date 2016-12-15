#include "stdafx.h"
#include "Sphere.h"


Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}

void Sphere::SetRadius(double radius)
{
	m_radius = abs(radius);

	m_localOOBB.max.x = m_radius;
	m_localOOBB.max.y = m_radius;
	m_localOOBB.max.z = m_radius;

	m_localOOBB.min = -m_localOOBB.max;
}

dVec3 Sphere::SupportLocal(const dVec3& v) const
{
	double k = m_radius / sqrt(v.Dot(v));
	return v.Scale(k);
}

dVec3 Sphere::Support(const dVec3& x, const dQuat& q, const dVec3& v) const
{
	return x + Sphere::SupportLocal(v);
}
