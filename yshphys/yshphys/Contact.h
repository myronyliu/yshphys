#pragma once
#include "YshMath.h"

class RigidBody;

struct Contact
{
public:
	Contact();
	virtual ~Contact();

	RigidBody* body[2];
	dVec3 x[2]; // contact point postions
	dVec3 n[2]; // contact point normals (typical usage has n[1] == -n[0]). The normal force of body[0] points along n[0].
};

