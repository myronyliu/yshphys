#pragma once
#include "YshMath.h"
#include "Geometry.h"
#include "BVNode.h"

class RigidBody : public BVNodeContent
{
public:
	RigidBody();
	virtual ~RigidBody();
protected:

	double m_mass;
	fMat33 m_inertia; // in the local frame of itself
	Geometry* m_geometry;

	dVec3 m_aLin;
	dVec3 m_vLin;

	dVec3 m_aAng;
	dVec3 m_vAng;

	dVec3 m_pos; // relative to GLOBAL frame
	dQuat m_rot; // relative to GLOBAL frame
};

