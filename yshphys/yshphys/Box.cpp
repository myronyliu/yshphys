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

dVec3 Box::SupportLocal(const dVec3& v) const
{
	return dVec3
	(
		m_halfDim.x * MathUtils::sgn(v.x),
		m_halfDim.y * MathUtils::sgn(v.y),
		m_halfDim.z * MathUtils::sgn(v.z)
	);
}