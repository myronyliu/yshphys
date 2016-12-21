#pragma once
#include "YshMath.h"
#include "Geometry.h"
#include "PhysicsObject.h"

// See http://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf
// and http://www.cs.cmu.edu/~baraff/sigcourse/notesd2.pdf


struct DampedOscillatorCoefficients
{
	float k;
	float b;
};

struct CollisionGeometry
{
	Geometry* m_geometry;
	dVec3 m_pos;
	dVec3 m_rot;
};

class RigidBody : public PhysicsObject
{
public:
	RigidBody();
	virtual ~RigidBody();

	double GetMass(double& inverseMass) const;

	dVec3 GetPosition() const;
	dQuat GetRotation() const;
	dVec3 GetLinearVelocity() const;
	dVec3 GetAngularVelocity() const;
	Geometry* GetGeometry() const;

	void SetPosition(const dVec3& x);
	void SetRotation(const dQuat& q);
	void SetGeometry(Geometry* geometry);
	void SetMass(double m);
	void SetInertia(dMat33 Ibody);

	void ApplyForce(const dVec3& F, const dVec3& applicationPt);
	void ApplyForceAtCenterOfMass(const dVec3& F);

	void ApplySpringPenalty(const dVec3& worldAnchor, const dVec3& localBodyAnchor, double kOverM, double bOverM);

	virtual void UpdateAABB();

	virtual void Step(double dt);

	dVec3 m_P; // linear  momentum
	dVec3 m_L; // angular momentum
	dVec3 m_dP; // linear impulse
	dVec3 m_dL; // angular impulse
protected:

	void Compute_qDot(const dQuat& q, const dVec3& L, dQuat& qDot) const; // no need to compute LDot; we know that it's equal to m_T
	void Compute_stateDot(
		const dVec3& x, const dVec3& P, const dQuat& q, const dVec3& L,
		dVec3& x_dot, dVec3& P_dot, dQuat& q_dot, dVec3& L_dot);

	Geometry* m_geometry;

	// CONSTANTS
	double m_m;
	double m_minv;
	dMat33 m_Ibody; // in the local frame of itself
	dMat33 m_Ibodyinv;

	// STATE VARIABLES
	dVec3 m_x; // position
	dQuat m_q; // orientation

//	dVec3 m_P; // linear  momentum
//	dVec3 m_L; // angular momentum

	// DERIVED STATE VARIABLES
	dMat33 m_Iinv;
	dVec3 m_v; // linear  velocity
	dVec3 m_w; // angular velocity

	// COMPUTED
	dVec3 m_F; // force
	dVec3 m_T; // torque

	dVec3 m_Fprev;
	dVec3 m_Tprev;

//	dVec3 m_dP; // linear impulse
//	dVec3 m_dL; // angular impulse

	struct SpringPenalty
	{
		dVec3 worldAnchor;
		dVec3 localBodyAnchor;
		double kOverM;
		double bOverM;
	};
	
	SpringPenalty m_springPenalties[16];
	int m_nSpringPenalties;
};

