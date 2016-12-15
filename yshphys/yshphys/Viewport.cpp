#include "stdafx.h"
#include "Viewport.h"

Viewport::Viewport() :
	m_fov(fPI*0.33f),
	m_aspect(1.33f),
	m_near(0.25f),
	m_far(64.0f),
	m_pos(0.0f, 0.0f, 0.0f),
	m_rot(fVec3(1.0f, 0.0f, 0.0f), fPI * 0.5f)
{
}

Viewport::~Viewport()
{
}

void Viewport::SetPos(const fVec3& pos)
{
	m_pos = pos;
}

fMat44 Viewport::CreateViewMatrix() const
{
	const fMat44 R_inv(fHomogeneousTransformation::CreateRotation(-m_rot));
	const fMat44 T_inv(fHomogeneousTransformation::CreateTranslation(-m_pos));
	return R_inv*T_inv;
}

fMat44 Viewport::CreateProjectionMatrix() const
{
	return fHomogeneousTransformation::CreateProjection(m_fov, m_aspect, m_near, m_far);
}

fVec3 Viewport::GetViewDir() const
{
	const fVec3 v = -fMat33(m_rot).GetColumn(2);
	return v.Scale(1.0f / sqrt(v.Dot(v)));
}

void Viewport::SetViewDir(const fVec3& viewDir, const fVec3& upDir)
{
	fVec3 z(-viewDir);
	fVec3 x(upDir.Cross(z));
	fVec3 y(z.Cross(x));

	x = x.Scale(1.0f / sqrt(x.Dot(x)));
	y = y.Scale(1.0f / sqrt(y.Dot(y)));
	z = z.Scale(1.0f / sqrt(z.Dot(z)));

	fMat33 R;
	R.SetColumn(0, x);
	R.SetColumn(1, y);
	R.SetColumn(2, z);
	m_rot = fQuat(R);
}

Ray Viewport::UnProject(int pixelX, int pixelY, int windowSpanX, int windowSpanY) const
{
	float x = m_aspect * (pixelX - 0.5f*windowSpanX) / (0.5f*windowSpanX);
	float y = (0.5f*windowSpanY - pixelY) / (0.5f*windowSpanY);
	float z = -1.0f / tan(0.5f*m_fov);

	fVec3 v(m_rot.Transform(fVec4(x, y, z, 1.0f)));
	v = v.Scale(1.0f / sqrt(v.Dot(v)));

	Ray ray;

	ray.SetDirection(v);
	ray.SetOrigin(m_pos);

	return ray;
}
