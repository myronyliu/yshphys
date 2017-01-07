#pragma once
#include "Force.h"
class Force_Spring :
	public Force
{
public:
	Force_Spring();
	virtual ~Force_Spring();

	dVec3 anchor;

	double k;
	double b;

	double length;

protected:
	virtual dVec3 ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const;
};

