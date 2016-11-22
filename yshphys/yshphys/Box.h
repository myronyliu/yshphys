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

	Vec3 Support(const Vec3& v) const;

protected:
	double m_halfDim[3];
};

