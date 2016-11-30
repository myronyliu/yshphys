#include "stdafx.h"
#include "Sphere.h"


Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}


dVec3 Sphere::Support(const dVec3& v) const
{
	double k = m_radius / sqrt(v.Dot(v));
	return m_pos + v.Scale(k);
}