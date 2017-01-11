#pragma once
#include "YshMath.h"
#include "Geometry.h"
#include "PhysicsObject.h"
#include "Contact.h"

class Force;
class Island;

// See http://www.cs.cmu.edu/~baraff/sigcourse/notesd1.pdf
// and http://www.cs.cmu.edu/~baraff/sigcourse/notesd2.pdf


struct DampedOscillatorCoefficients
{
	float k;
	float b;
};

struct CollisionGeometry
{
	Geometry* geom;
	dVec3 pos;
	dQuat rot;
};

class RigidBody : public PhysicsObject
{
public:

	struct State
	{
		dVec3 P; // linear  momentum
		dVec3 L; // angular momentum
		dVec3 x; // position
		dQuat q; // orientation
	};
	struct Inertia
	{
		// CONSTANTS
		double m;
		double minv;
		dMat33 Ibody; // in the local frame of itself
		dMat33 Ibodyinv;
	};

	RigidBody();
	virtual ~RigidBody();

	double GetMass() const;
	double GetInverseMass() const;
	dMat33 GetInverseInertia() const;

	dVec3 GetPosition() const;
	dQuat GetRotation() const;
	dVec3 GetLinearVelocity() const;
	dVec3 GetAngularVelocity() const;
	Geometry* GetGeometry() const;
	void GetGeometryLocalTransform(dVec3& pos, dQuat& rot) const;
	void GetGeometryGlobalTransform(dVec3& pos, dQuat& rot) const;

	void SetPosition(const dVec3& x);
	void SetRotation(const dQuat& q);
	void SetGeometry(Geometry* geometry, const dVec3& relativePos, const dQuat& relativeRot);
	void SetMass(double m);
	void SetInertia(const dMat33& Ibody);
	void SetInertia(const dQuat& principleAxes, const dVec3& inertia);

	void ApplyBruteForce(Force* force);
	void ApplyForce(const dVec3& force, const dVec3& worldPos);
	void ApplyImpulse(const dVec3& impulse, const dVec3& worldPos);

	void SetIsland(Island* island)
	{
		m_island = island;
	}
	Island* GetIsland() const
	{
		return m_island;
	}

	virtual void UpdateAABB();

	virtual void Step(double dt);
protected:

	CollisionGeometry m_geometry;

	RigidBody::State m_state;
	RigidBody::Inertia m_inertia;
	
	dVec3 m_dP; // linear impulse
	dVec3 m_dL; // angular impulse

	dVec3 m_F;
	dVec3 m_T;

	// DERIVED STATE VARIABLES
	dMat33 m_Iinv;
	dVec3 m_v; // linear  velocity
	dVec3 m_w; // angular velocity

	void UpdateDependentStateVariables()
	{
		dMat33 R(m_state.q);
		m_Iinv = R*m_inertia.Ibodyinv*R.Transpose();
		m_w = m_Iinv.Transform(m_state.L);
		m_v = m_state.P.Scale(m_inertia.minv);
	}

	Force* m_forces[64];
	int m_nForces;

	Island* m_island;

	void Compute_xDot(const dVec3& P, dVec3& xDot) const;
	void Compute_qDot(const dQuat& q, const dVec3& L, dQuat& qDot) const;

	void ResolveImpulses();
	void ResolveForces(double dt);
};

