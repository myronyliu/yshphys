#include "stdafx.h"
#include "Ray.h"


Ray::Ray() :
	m_direction(0.0, 0.0, 0.0),
	m_dirInv(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()),
	m_origin(0.0, 0.0, 0.0),
	m_length(88888888.88888888)
{
}
Ray::~Ray()
{
}
dVec3 Ray::GetDirection() const
{
	return m_direction;
}
dVec3 Ray::GetOrigin() const
{
	return m_origin;
}
double Ray::GetLength() const
{
	return m_length;
}
void Ray::SetDirection(const dVec3& direction)
{
	m_direction = direction;
	m_dirInv.x = 1.0 / direction.x;
	m_dirInv.y = 1.0 / direction.y;
	m_dirInv.z = 1.0 / direction.z;
}
void Ray::SetOrigin(const dVec3& origin)
{
	m_origin = origin;
}
void Ray::SetLength(double length)
{
	m_length = length;
}

bool Ray::IntersectOOBB(const BoundingBox& oobb, const dVec3& pos, const dQuat& rot, double& tMin, double& tMax) const
{
	Ray r = *this;
	r.SetDirection((-rot).Transform(m_direction));
	r.SetOrigin((-rot).Transform(m_origin - pos));

	AABB aabb;
	aabb.min = oobb.min;
	aabb.max = oobb.max;

	return r.IntersectAABB(aabb, tMin, tMax);
}

bool Ray::IntersectAABB(const AABB& aabb, double& tMin, double& tMax) const
{
	double tx0 = (aabb.min.x - m_origin.x) * m_dirInv.x;
	double tx1 = (aabb.max.x - m_origin.x) * m_dirInv.x;

	tMin = std::min(tx0, tx1);
	tMax = std::max(tx0, tx1);

	double ty0 = (aabb.min.y - m_origin.y) * m_dirInv.y;
	double ty1 = (aabb.max.y - m_origin.y) * m_dirInv.y;

	tMin = std::max(tMin, std::min(ty0, ty1));
	tMax = std::min(tMax, std::max(ty0, ty1));

	double tz0 = (aabb.min.z - m_origin.z) * m_dirInv.z;
	double tz1 = (aabb.max.z - m_origin.z) * m_dirInv.z;

	tMin = std::max(tMin, std::min(tz0, tz1));
	tMax = std::min(tMax, std::max(tz0, tz1));

	return (tMin < tMax) && (tMin < m_length);
}