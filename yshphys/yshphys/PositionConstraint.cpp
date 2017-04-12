#include "stdafx.h"
#include "PositionConstraint.h"


PositionConstraint::PositionConstraint()
{
}


PositionConstraint::~PositionConstraint()
{
}

void PositionConstraint::BuildFixedTerms(double dt)
{
	EvaluateC();
	EvaluateJMJ();
	EvaluateBiasFactor(dt);
}
