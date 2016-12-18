#include "stdafx.h"
#include "Sphere.h"
#include "Ray.h"


Sphere::Sphere()
{
}


Sphere::~Sphere()
{
}

void Sphere::SetRadius(double radius)
{
	m_radius = abs(radius);

	m_localOOBB.max.x = m_radius;
	m_localOOBB.max.y = m_radius;
	m_localOOBB.max.z = m_radius;

	m_localOOBB.min = -m_localOOBB.max;
}

SupportPolygon Sphere::SupportLocal(const dVec3& v) const
{
	SupportPolygon poly;
	poly.nVertices = 1;
	poly.vertices[0] = v.Scale(m_radius / sqrt(v.Dot(v)));
	return poly;
}

SupportPolygon Sphere::Support(const dVec3& x, const dQuat& q, const dVec3& v) const
{
	SupportPolygon poly;
	poly.nVertices = 1;
	poly.vertices[0] = x + v.Scale(m_radius / sqrt(v.Dot(v)));
	return poly;
}

bool Sphere::RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hit) const
{
	const dVec3& rayDir = ray.GetDirection();
	const dVec3 center2origin = ray.GetOrigin() - pos;
	const dVec3 center2ray = center2origin - rayDir.Scale(center2origin.Dot(rayDir) / rayDir.Dot(rayDir));
	const double dSqr = center2ray.Dot(center2ray);
	const double rSqr = m_radius * m_radius;
	const double disc = rSqr - dSqr;

	if (disc < 0.0)
	{
		return false;
	}
	else if (disc == 0.0)
	{
		hit = pos + center2ray.Scale(m_radius / sqrt(dSqr));
		return true;
	}
	else
	{
		hit = pos + center2ray - rayDir.Scale(sqrt(disc / rayDir.Dot(rayDir)));
		return true;
	}
}
