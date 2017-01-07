#include "stdafx.h"
#include "Force_Constant.h"


Force_Constant::Force_Constant()
{
}


Force_Constant::~Force_Constant()
{
}

dVec3 Force_Constant::ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const
{
	return F;
}
