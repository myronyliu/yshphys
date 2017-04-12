#pragma once
#include "PositionConstraint.h"
class PositionConstraint_Contact :
	public PositionConstraint
{
public:

	RigidBody* body[2];
	dVec3 x[2];
	dVec3 n[2]; // The impulse on body[0] points along n[0] (i.e. n[0] is ANTIparallel to the geometric normal)

	PositionConstraint_Contact();
	virtual ~PositionConstraint_Contact();

	virtual void Resolve();

protected:

	virtual void EvaluateC();
	virtual void EvaluateJMJ();
	virtual void EvaluateBiasFactor(double dt);

	J_1x6 m_J[2];
	J_1x6 m_JM[2];
	double m_JMJ;
	double m_C;
	double m_lambda;

	double m_b;
};

