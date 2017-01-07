#pragma once
#include "YshMath.h"
#include "RigidBody.h"

class Force
{
public:
	Force();
	virtual ~Force();

	virtual void ComputeForceAndTorque(const RigidBody::Inertia& inertia, const RigidBody::State& state, dVec3& F, dVec3& T) const;

	dVec3 offset; // relative to the rigidbody's local frame. An offset of 0.0 always yields zero torque.

protected:
	virtual dVec3 ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const;
};

