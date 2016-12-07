#include "stdafx.h"
#include "Capsule.h"


Capsule::Capsule()
{
}


Capsule::~Capsule()
{
}

void Capsule::SetHalfHeight(double halfHeight)
{
	m_halfHeight = abs(halfHeight);
	m_localOOBB.min.z = -m_halfHeight - m_radius;
	m_localOOBB.max.z = m_halfHeight + m_radius;
}

void Capsule::SetRadius(double radius)
{
	m_radius = abs(radius);

	m_localOOBB.max.x = m_radius;
	m_localOOBB.max.y = m_radius;
	m_localOOBB.max.z = m_halfHeight + m_radius;

	m_localOOBB.min = -m_localOOBB.max;
}

dVec3 Capsule::Support(const dVec3& v) const
{
	double k = m_radius / sqrt(v.Dot(v));
	dVec3 supportLocal = v.Scale(k);

	double z = (-m_rot).Transform(v).z;
	supportLocal.z += (double)((0.0 < z) - (z < 0.0)) * m_halfHeight;

	return m_pos + m_rot.Transform(supportLocal);
}