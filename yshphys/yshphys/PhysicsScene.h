#pragma once
#include "BVTree.h"
class PhysicsScene
{
public:
	PhysicsScene();
	virtual ~PhysicsScene();
protected:
	BVTree* asdf;
};

