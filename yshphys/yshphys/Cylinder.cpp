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

dVec3 Cylinder::Support(const dVec3& v) const
{
	dVec3 vLocal = (-m_rot).Transform(v);
	dVec3 supportLocal;
	double xySqr = vLocal.x*vLocal.x + vLocal.y*vLocal.y;

	if (xySqr > 0.0)
	{
		double xy = sqrt(xySqr);
		supportLocal.x = vLocal.x / (xySqr);
		supportLocal.y = vLocal.y / (xySqr);
	}
	else
	{
		supportLocal.x = 0.0;
		supportLocal.y = 0.0;
	}
	supportLocal.z = m_halfHeight * (double)((0.0 < vLocal.z) - (vLocal.z < 0.0));
	return m_pos + m_rot.Transform(supportLocal);
}