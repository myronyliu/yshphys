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

dVec3 Box::Support(const dVec3& v) const
{
	dVec3 vLocal = (-m_rot).Transform(v); // rotate v into the local frame of box;
	dVec3 supportLocal
	(
		m_halfDim.x * (double)((0.0 < vLocal.x) - (vLocal.x < 0.0)),
		m_halfDim.y * (double)((0.0 < vLocal.y) - (vLocal.y < 0.0)),
		m_halfDim.z * (double)((0.0 < vLocal.z) - (vLocal.z < 0.0))
	);
	return m_pos + m_rot.Transform(supportLocal);
}