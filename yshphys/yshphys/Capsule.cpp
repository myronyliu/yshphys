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

dVec3 Capsule::SupportLocal(const dVec3& v, bool& degenerate) const
{
	degenerate = v.z == 0.0;

	double k = m_radius / sqrt(v.Dot(v));
	dVec3 support = v.Scale(k);

	double z = (-m_rot).Transform(v).z;
	support.z += MathUtils::sgn(v.z) * m_halfHeight;

	return support;
}
