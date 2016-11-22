#include "stdafx.h"
#include "Box.h"


Box::Box()
{
}


Box::~Box()
{
}

void Box::GetDimensions(double& halfDimX, double& halfDimY, double& halfDimZ) const
{
	halfDimX = m_halfDim[0];
	halfDimY = m_halfDim[1];
	halfDimZ = m_halfDim[2];
}

void Box::SetDimensions(double halfDimX, double halfDimY, double halfDimZ)
{
	m_halfDim[0] = halfDimX;
	m_halfDim[1] = halfDimY;
	m_halfDim[2] = halfDimZ;
}

Vec3 Box::Support(const Vec3& v) const
{
	Vec3 vLocal = (-m_ori).Transform(v); // rotate v into the local frame of box;
	Vec3 supportLocal
	(
		m_halfDim[0] * (double)((0.0 < vLocal.x) - (vLocal.x < 0.0)),
		m_halfDim[1] * (double)((0.0 < vLocal.y) - (vLocal.y < 0.0)),
		m_halfDim[2] * (double)((0.0 < vLocal.z) - (vLocal.z < 0.0))
	);
	return m_pos + m_ori.Transform(supportLocal);
}