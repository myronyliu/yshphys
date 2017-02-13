#include "stdafx.h"
#include "Force_Spring.h"


Force_Spring::Force_Spring()
{
}


Force_Spring::~Force_Spring()
{
}

dVec3 Force_Spring::ComputeForce(const RigidBody::Inertia& inertia, const RigidBody::State& state) const
{
	const dQuat& q = state.q;
	const dVec3& L = state.L;
	const dVec3& x = state.x;
	const dVec3& P = state.P;

	const dVec3 tether = x + q.Transform(offset);

	const dVec3 v = P.Scale(inertia.minv);

	dVec3 Fs, Fd;

	const dVec3 dx = anchor - tether;
	const double dx_dx = dx.Dot(dx);

	if (dx_dx > FLT_EPSILON)
	{
		Fs = dx.Scale(k*(1.0 - length / sqrt(dx_dx)));
		Fd = -dx.Scale(b*v.Dot(dx) / dx_dx);
	}
	else
	{
		Fs = dVec3(0.0, 0.0, 0.0);
		Fd = -v.Scale(b);
	}

	return Fs + Fd;
}
