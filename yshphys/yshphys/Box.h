#pragma once
#include "Geometry.h"
class Box :
	public Geometry
{
public:
	Box();
	virtual ~Box();

	void GetDimensions(double& halfDimX, double& halfDimY, double& halfDimZ) const;
	void SetDimensions(double halfDimX, double halDimY, double halfDimZ);

	virtual dVec3 SupportLocal(const dVec3& v, bool& degenerate) const;

	virtual bool RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hitPt) const;

protected:
	dVec3 m_halfDim;
};

