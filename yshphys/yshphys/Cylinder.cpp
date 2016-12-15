#include "stdafx.h"
#include "Cylinder.h"


Cylinder::Cylinder()
{
}


Cylinder::~Cylinder()
{
}

void Cylinder::SetHalfHeight(double halfHeight)
{
	m_halfHeight = abs(halfHeight);
	m_localOOBB.min.z = -m_halfHeight;
	m_localOOBB.max.z = m_halfHeight;
}

void Cylinder::SetRadius(double radius)
{
	m_radius = abs(radius);
	m_localOOBB.max.x = -m_radius;
	m_localOOBB.max.y = -m_radius;
	m_localOOBB.max.x = m_radius;
	m_localOOBB.max.y = m_radius;
}

dVec3 Cylinder::SupportLocal(const dVec3& v) const
{
	dVec3 support;
	double xySqr = v.x*v.x + v.y*v.y;

	if (xySqr > 0.0)
	{
		double xy = sqrt(xySqr);
		support.x = v.x / (xySqr);
		support.y = v.y / (xySqr);
	}
	else
	{
		support.x = 0.0;
		support.y = 0.0;
	}
	support.z = m_halfHeight * MathUtils::sgn(v.z);
	return support;
}