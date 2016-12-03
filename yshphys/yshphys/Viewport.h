#pragma once
#include "YshMath.h"

class Viewport
{
public:
	Viewport();
	virtual ~Viewport();

	fMat44 CreateViewMatrix() const;
	fMat44 CreateProjectionMatrix() const;

	void SetViewDir(const fVec3& viewDir, const fVec3& upDir);

	fVec3 m_pos;
	fQuat m_rot; // NEGATIVE z-axis of the rotation matrix defines the viewing direction. y is up, x is right (OpenGL convention)
	float m_fov; // field of view in the y direction (following OpenGL's convention)
	float m_aspect; // aspect ratio x/y
	float m_near; // near plane closer than which objects are clipped
	float m_far; // far plane further than which objects are clipped
};

