#pragma once
#include "YshMath.h"
#include "Geometry.h"
#include "BVNode.h"

// See http://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf
// and http://www.cs.cmu.edu/~baraff/sigcourse/notesd2.pdf

class RigidBody : public BVNodeContent
{
public:
	RigidBody();
	virtual ~RigidBody();

	AABB GetAABB() const;
	void UpdateAABB();

	void Step(double dt);

protected:
	Geometry* m_geometry;

	// CONSTANTS
	double m_m;
	fMat33 m_Ibody; // in the local frame of itself
	fMat33 m_Ibodyinv;

	// STATE VARIABLES
	dVec3 m_x; // position
	dQuat m_q; // orientation

	dVec3 m_P; // linear  momentum
	dVec3 m_L; // angular momentum

	// DERIVED STATE VARIABLES
	fMat33 m_Iinv;
	dVec3 m_v; // linear  velocity
	dVec3 m_w; // angular velocity

	// COMPUTED
	dVec3 m_F; // force
	dVec3 m_T; // torque

	// CACHED DATA
	AABB m_AABB;
};

