#include "stdafx.h"
#include "Viewport.h"

Viewport::Viewport() :
	m_fov(60.0f),
	m_aspect(1.5),
	m_near(0.25f),
	m_far(64.0f),
	m_pos(fVec3(0.0f, 0.0f, 0.0f)),
	m_rot(fQuat(0.0f, 0.0f, 0.0f, 1.0f))
{
}

Viewport::~Viewport()
{
}

fMat44 Viewport::CreateViewMatrix() const
{
	const fMat44 T_inv(fHomogeneousTransformation::CreateTranslation(-m_pos));
	const fMat44 R_inv(fHomogeneousTransformation::CreateRotation(-m_rot));
	return T_inv*R_inv;
}

fMat44 Viewport::CreateProjectionMatrix() const
{
	return fHomogeneousTransformation::CreateProjection(m_fov, m_aspect, m_near, m_far);
}