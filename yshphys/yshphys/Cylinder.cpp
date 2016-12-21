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
	double xySqr = v.x*v.x + v.y*v.y;
	double xy = sqrt(xySqr);

	return dVec3(
		m_radius*v.x / xy,
		m_radius*v.y / xy,
		m_halfHeight*MathUtils::sgn(v.z)
	);
}