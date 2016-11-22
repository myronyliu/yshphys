#include "stdafx.h"
#include "Sphere.h"


Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}


Vec3 Sphere::Support(const Vec3& v) const
{
	float k = m_radius / sqrtf(v.Dot(v));
	return m_pos + v.Scale(k);
}