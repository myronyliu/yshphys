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

	dVec3 Support(const dVec3& v) const;

protected:
	dVec3 m_halfDim;
};

