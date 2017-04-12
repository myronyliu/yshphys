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
	m_localOOBB.min.x = -m_radius;
	m_localOOBB.min.y = -m_radius;
	m_localOOBB.max.x = m_radius;
	m_localOOBB.max.y = m_radius;
}

dVec3 Cylinder::SupportLocal(const dVec3& v) const
{
	const double phi = atan2(v.y, v.x);

	return dVec3(
		m_radius*cos(phi),
		m_radius*sin(phi),
		m_halfHeight*MathUtils::sgn(v.z));
}

Polygon Cylinder::IntersectPlaneLocal(const dVec3& planeOrigin, const dVec3& planeNormal, const dVec3& x, const dVec3& y) const
{
	Polygon poly;

	if (abs(planeNormal.z) < 0.001) // the capsule is oriented like a tire rolling on the plane
	{
		dVec3 A(0.0, 0.0, -m_halfHeight - planeOrigin.z);
		dVec3 B(0.0, 0.0, m_halfHeight - planeOrigin.z);

		poly.AddVertex(dVec2(A.Dot(x), A.Dot(y)));
		poly.AddVertex(dVec2(B.Dot(x), B.Dot(y)));
	}
	else if (abs(planeNormal.z) > 0.9999) // the capsule is oriented like an erect barrel on the plane
	{
		const double cx = planeOrigin.Dot(x);
		const double cy = planeOrigin.Dot(y);

		const int nPts = 8;

		for (int i = 0; i < nPts; ++i)
		{
			double phi = 2.0*dPI*double(i) / (double)nPts;
			poly.AddVertex(dVec2(m_radius*cos(phi) - cx, m_radius*sin(phi) - cy));
		}
	}
	else // the plane intersects at the cylinder's edge
	{
		poly.AddVertex(dVec2(0.0, 0.0));
	}

	return poly;
}