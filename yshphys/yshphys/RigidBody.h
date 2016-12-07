#pragma once
#include "YshMath.h"
#include "Geometry.h"

class RigidBody
{
public:
	RigidBody();
	virtual ~RigidBody();
protected:
	double m_mass;
	fMat33 m_inertia; // in the local frame of itself
	Geometry* m_geometry;

	dVec3 m_pos; // relative to GLOBAL frame
	dQuat m_rot; // relative to GLOBAL frame
};

