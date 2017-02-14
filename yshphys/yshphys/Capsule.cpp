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

dVec3 Capsule::SupportLocal(const dVec3& v) const
{
	double k = m_radius / sqrt(v.Dot(v));
	dVec3 support = v.Scale(k);
	support.z += m_halfHeight*MathUtils::sgn(v.z);
	return support;
}

Polygon Capsule::IntersectPlaneLocal(const dVec3& planeOrigin, const dVec3& planeNormal, const dVec3& x, const dVec3& y) const
{
	Polygon poly;

	if (abs(planeNormal.z) > 0.001) // the capsule is tilted by more than ~0.5 degrees
	{
		poly.AddVertex(dVec2(0.0, 0.0));
	}
	else
	{
		dVec3 A(0.0, 0.0, -m_halfHeight - planeOrigin.z);
		dVec3 B(0.0, 0.0, m_halfHeight - planeOrigin.z);

		poly.AddVertex(dVec2(A.Dot(x), A.Dot(y)));
		poly.AddVertex(dVec2(B.Dot(x), B.Dot(y)));
	}

	return poly;
}
