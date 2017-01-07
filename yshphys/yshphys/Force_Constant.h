#pragma once
#include "Force.h"
class Force_Constant :
	public Force
{
public:
	Force_Constant();
	virtual ~Force_Constant();

	dVec3 F;

protected:
	virtual dVec3 ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const;
};
