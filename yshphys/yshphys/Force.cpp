#include "stdafx.h"
#include "Force.h"


Force::Force()
{
}


Force::~Force()
{
}

dVec3 Force::ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const
{
	return dVec3(0.0, 0.0, 0.0);
}
void Force::ComputeForceAndTorque(const RigidBody::Inertia& inertia, const RigidBody::State& state, dVec3& F, dVec3& T) const
{
	F = ComputeForce(inertia, state);
	T = state.q.Transform(offset).Cross(F);
}
