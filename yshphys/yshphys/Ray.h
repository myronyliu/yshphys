#pragma once
#include "YshMath.h"
#include "BoundingBox.h"

class Ray
{
public:
	Ray();
	virtual ~Ray();

	dVec3 GetDirection() const;
	dVec3 GetOrigin() const;
	double GetLength() const;

	void SetDirection(const dVec3& direction);
	void SetOrigin(const dVec3& origin);
	void SetLength(double length);

	bool IntersectOOBB(const BoundingBox& oobb, const dVec3& pos, const dQuat& rot, double& tIn, double& tOut) const;
	bool IntersectAABB(const AABB& aabb, double& tIn, double& tOut) const;

protected:
	dVec3 m_direction;
	dVec3 m_dirInv; // componentwise inverse of m_direction. Cache it since we will use it frequently for ray-AABB tests
	dVec3 m_origin;
	double m_length;
};

