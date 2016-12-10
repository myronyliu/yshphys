#include "stdafx.h"
#include "RigidBody.h"

#define AABB_QUANTIZATION 0.25

void QuantizeAABB(AABB& aabb)
{
	aabb.min.x = std::floor(aabb.min.x / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	aabb.min.y = std::floor(aabb.min.y / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	aabb.min.z = std::floor(aabb.min.z / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	aabb.max.x = std::ceil(aabb.max.x / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	aabb.max.y = std::ceil(aabb.max.y / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	aabb.max.z = std::ceil(aabb.max.z / AABB_QUANTIZATION) * AABB_QUANTIZATION;
}

RigidBody::RigidBody()
{
}

RigidBody::~RigidBody()
{
}

AABB RigidBody::GetAABB() const
{
	return m_AABB;
}

void RigidBody::UpdateAABB()
{
	const BoundingBox oobb = m_geometry->GetLocalOOBB();
	const dVec3 oobbCenter = (oobb.min + oobb.max).Scale(0.5); // center of the OOBB in geom's local frame
	const dVec3 oobbSpan = (oobb.max - oobb.min).Scale(0.5); // span of the OOBB

	const dVec3 aabbCenter = m_pos + m_rot.Transform(m_geometry->GetPosition() + oobbCenter);
	const dQuat geomRot = m_rot*m_geometry->GetRotation();

	const dVec3 aabbSpan = dMat33(geomRot).Abs().Transform(oobbSpan);

	m_AABB.min = aabbCenter - aabbSpan;
	m_AABB.max = aabbCenter + aabbSpan;

	QuantizeAABB(m_AABB);

	m_bvNode->SetAABB(m_AABB);
}