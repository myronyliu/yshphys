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

SupportPolygon Capsule::SupportLocal(const dVec3& v) const
{
	SupportPolygon poly;

	double xySqr = v.x*v.x + v.y*v.y;
	double xy = sqrt(xySqr);

	if (v.z*v.z / xySqr < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
	{
		const double supportX = m_radius*v.x / xy;
		const double supportY = m_radius*v.y / xy;

		poly.nVertices = 2;
		poly.vertices[0] = dVec3(supportX, supportY, -m_halfHeight);
		poly.vertices[1] = dVec3(supportX, supportY, m_halfHeight);
	}
	else
	{
		double k = m_radius / sqrt(v.Dot(v));
		dVec3 support = v.Scale(k);

		double z = (-m_rot).Transform(v).z;
		support.z += MathUtils::sgn(v.z) * m_halfHeight;

		poly.nVertices = 1;
		poly.vertices[0] = support;
	}

	return poly;
}
