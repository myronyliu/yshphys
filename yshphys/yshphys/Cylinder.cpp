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

Polygon Cylinder::IntersectPlaneLocal(const dVec3& planeOrigin, const dQuat& planeOrientation) const
{
	Polygon poly;

	const dMat33 R = dMat33(planeOrientation);

	if (abs(R(2, 2)) < 0.001) // the capsule is oriented like a tire rolling on the plane
	{
		dVec3 x = R.GetColumn(0);
		dVec3 y = R.GetColumn(1);

		dVec3 A(0.0, 0.0, -m_halfHeight - planeOrigin.z);
		dVec3 B(0.0, 0.0, m_halfHeight - planeOrigin.z);

		poly.AddVertex(dVec2(A.Dot(x), A.Dot(y)));
		poly.AddVertex(dVec2(B.Dot(x), B.Dot(y)));
	}
	else if (abs(R(2,2)) > 0.9999) // the capsule is oriented like an erect barrel on the plane
	{
		const double cx = planeOrigin.Dot(R.GetColumn(0));
		const double cy = planeOrigin.Dot(R.GetColumn(1));

		for (int i = 0; i < 8; ++i)
		{
			double phi = 2.0*dPI*double(i) / (double)8;
			poly.AddVertex(dVec2(m_radius*cos(phi) - cx, m_radius*sin(phi) - cy));
		}
	}
	else // the plane intersects at the cylinder's edge
	{
		poly.AddVertex(dVec2(0.0, 0.0));
	}

	return poly;
}