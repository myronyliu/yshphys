#pragma once
#include "Quat.h"
#include "Vec3.h"
class Viewport
{
public:
	Viewport();
	virtual ~Viewport();

	fVec3 m_pos;
	fQuat m_rot; // NEGATIVE z-axis of the rotation matrix defines the viewing direction. y is up, x is right
	float m_fov; // y (following OpenGL's convention)
	float m_aspect; // x/y
	float m_near; // near plane closer than which objects are clipped
	float m_far; // far plane further than which objects are clipped
};

