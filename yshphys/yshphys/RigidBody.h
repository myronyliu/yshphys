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

	void Compute_qDot(const dQuat& q, const dVec3& L, dQuat& qDot) const; // no need to compute LDot; we know that it's equal to m_T

	Geometry* m_geometry;

	// CONSTANTS
	double m_m;
	double m_minv;
	dMat33 m_Ibody; // in the local frame of itself
	dMat33 m_Ibodyinv;

	// STATE VARIABLES
	dVec3 m_x; // position
	dQuat m_q; // orientation

	dVec3 m_P; // linear  momentum
	dVec3 m_L; // angular momentum

	// DERIVED STATE VARIABLES
	dMat33 m_Iinv;
	dVec3 m_v; // linear  velocity
	dVec3 m_w; // angular velocity

	// COMPUTED
	dVec3 m_F; // force
	dVec3 m_T; // torque

	// CACHED DATA
	AABB m_AABB;
};

