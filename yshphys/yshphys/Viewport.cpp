#include "stdafx.h"
#include "Viewport.h"

Viewport::Viewport() :
	m_fov(fPI*0.33f),
	m_aspect(1.33f),
	m_near(0.25f),
	m_far(64.0f),
	m_pos(0.0f, 0.0f, 0.0f),
	m_rot(0.0f, 0.0f, 0.0f, 1.0f)
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

void Viewport::SetViewDir(const fVec3& viewDir, const fVec3& upDir)
{
	fVec3 z(-viewDir);
	fVec3 x(upDir.Cross(z));
	fVec3 y(z.Cross(x));

	x.Scale(1.0f / sqrt(x.Dot(x)));
	y.Scale(1.0f / sqrt(y.Dot(y)));
	z.Scale(1.0f / sqrt(z.Dot(z)));

	fMat33 R;
	R.SetColumn(0, x);
	R.SetColumn(1, y);
	R.SetColumn(2, z);
	m_rot = fQuat(R);
}