#include "stdafx.h"
#include "Box.h"
#include "Ray.h"


Box::Box()
{
}


Box::~Box()
{
}

void Box::GetDimensions(double& halfDimX, double& halfDimY, double& halfDimZ) const
{
	halfDimX = m_halfDim.x;
	halfDimY = m_halfDim.y;
	halfDimZ = m_halfDim.z;
}

void Box::SetDimensions(double halfDimX, double halfDimY, double halfDimZ)
{
	m_halfDim.x = abs(halfDimX);
	m_halfDim.y = abs(halfDimY);
	m_halfDim.z = abs(halfDimZ);

	m_localOOBB.min = -m_halfDim;
	m_localOOBB.max = m_halfDim;
}

dVec3 Box::SupportLocal(const dVec3& v) const
{
	return dVec3(
		m_halfDim.x * MathUtils::sgn(v.x),
		m_halfDim.y * MathUtils::sgn(v.y),
		m_halfDim.z * MathUtils::sgn(v.z));
}

bool Box::RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hit) const
{
	Ray r;
	r.SetOrigin((-rot).Transform((ray.GetOrigin() - pos)));
	r.SetDirection((-rot).Transform(ray.GetDirection()));
	AABB aabb;
	aabb.min = -m_halfDim;
	aabb.max = m_halfDim;

	double tMin, tMax;
	if (r.IntersectAABB(aabb, tMin, tMax))
	{
		hit = ray.GetOrigin() + ray.GetDirection().Scale(tMin);
		return true;
	}
	else
	{
		return false;
	}
}