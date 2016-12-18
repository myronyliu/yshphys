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

SupportPolygon Cylinder::SupportLocal(const dVec3& v) const
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
	else if (xySqr / v.z*v.z < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
	{
		poly.nVertices = MAX_SUPPORT_POLYGON_VERTICES;
		const double supportZ = m_halfHeight * MathUtils::sgn(v.z);

		for (int i = 0; i < MAX_SUPPORT_POLYGON_VERTICES; ++i)
		{
			double theta = (double)i*2.0*dPI / MAX_SUPPORT_POLYGON_VERTICES;
			poly.vertices[i].x = m_radius*cos(theta);
			poly.vertices[i].y = m_radius*sin(theta);
			poly.vertices[i].z = supportZ;
		}
	}
	else
	{
		const double supportX = m_radius*v.x / xy;
		const double supportY = m_radius*v.y / xy;

		poly.nVertices = 1;
		poly.vertices[0].x = m_radius*v.x / xy;
		poly.vertices[0].y = m_radius*v.y / xy;
		poly.vertices[0].z = m_halfHeight*MathUtils::sgn(v.z);
	}
	return poly;
}