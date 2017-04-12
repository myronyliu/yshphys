#pragma once
#include "YshMath.h"
//#include "RigidBody.h"

class RigidBody;

struct J_1x6
{
	dVec3 J_v;
	dVec3 J_w;

	RigidBody* body;
};

class PositionConstraint
{
public:
	PositionConstraint();
	virtual ~PositionConstraint();

	virtual void BuildFixedTerms(double dt);
	// Evaluates Jv + b, where v = [v_0, w_0, ..., v_n, w_n] and b is the bias term (e.g. resitution, Baumgarte stabilization)
	// Subsequently solves for the delta Impulse to be applied, clamping the accumulated impulse if necessary.
	virtual void Resolve() = 0;

protected:

	// Evaluates the constraint function(s) C_i(x_0, q_0, ..., x_n, q_n)
	virtual void EvaluateC() = 0;

	// Evaluates...
	// 1.  J = del C_i / del (x, q)_j
	// 2.  MJ = M_inv J_trans (which has the same sparse form as J)
	// 3.  JMJ = J MJ
	virtual void EvaluateJMJ() = 0;

	virtual void EvaluateBiasFactor(double dt) = 0;
};

