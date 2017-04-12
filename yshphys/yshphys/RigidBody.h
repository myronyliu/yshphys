#pragma once
#include "YshMath.h"
#include "Geometry.h"
#include "PhysicsObject.h"
#include "PositionConstraint_Contact.h"

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

	bool IsStatic() const;

	dVec3 GetPosition() const;
	dQuat GetRotation() const;
	dVec3 GetLinearVelocity() const;
	dVec3 GetAngularVelocity() const;
	Geometry* GetGeometry() const;
	void GetGeometryLocalTransform(dVec3& pos, dQuat& rot) const;
	void GetGeometryGlobalTransform(dVec3& pos, dQuat& rot) const;

	Material::Type GetMaterial(const dVec3& worldPos) const;

	void SetPosition(const dVec3& x);
	void SetRotation(const dQuat& q);
	void SetGeometry(Geometry* geometry, const dVec3& relativePos, const dQuat& relativeRot);
	void SetMass(double m);
	void SetInertia(const dMat33& Ibody);
	void SetInertia(const dQuat& principleAxes, const dVec3& inertia);

	void ApplyBruteForce(Force* force);

	void ApplyForce(const dVec3& force, const dVec3& worldPos);
	void ApplyImpulse(const dVec3& impulse, const dVec3& worldPos);

	void ApplyLinImpulse(const dVec3& linImpulse) { m_dP = m_dP + linImpulse; }
	void ApplyAngImpulse(const dVec3& angImpulse) { m_dL = m_dL + angImpulse; }

	void ApplyLinImpulse_Immediate(const dVec3& linImpulse) { m_state.P = m_state.P + linImpulse; }
	void ApplyAngImpulse_Immediate(const dVec3& angImpulse) { m_state.L = m_state.L + angImpulse; }

	void ApplyForceAtCOM(const dVec3& force) { m_F = m_F + force; }
	void ApplyImpulseAtCOM(const dVec3& impulse) { m_dP = m_dP + impulse; }

	dVec3 GetForce() const { return m_F; }
	dVec3 GetTorque() const { return m_T; }

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
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < i; ++j)
			{
				const double x = (m_Iinv(i, j) + m_Iinv(j, i))*0.5;
				m_Iinv(i, j) = x;
				m_Iinv(j, i) = x;
			}
		}
		m_w = m_Iinv.Transform(m_state.L);
		m_v = m_state.P.Scale(m_inertia.minv);

		assert(m_Iinv.Transpose() == m_Iinv);
	}

	Force* m_forces[64];
	int m_nForces;

	// m_island should be null for static objects, since static objects should be able to appear in multiple islands
	Island* m_island;

	void Compute_xDot(const dVec3& P, dVec3& xDot) const;
	void Compute_qDot(const dQuat& q, const dVec3& L, dQuat& qDot) const;

	void ResolveImpulses();
	void ResolveForces(double dt);

	void Damp(double dt);
};

