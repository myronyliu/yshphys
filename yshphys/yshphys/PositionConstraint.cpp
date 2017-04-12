#include "stdafx.h"
#include "PositionConstraint.h"


PositionConstraint::PositionConstraint()
{
}


PositionConstraint::~PositionConstraint()
{
}

void PositionConstraint::BuildFixedTerms()
{
	EvaluateC();
	EvaluateJMJ();
}
