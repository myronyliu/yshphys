#pragma once
#include "Geometry.h"

class PhysicsNode;

class PhysicsObject
{
	friend class PhysicsNode;
public:
	PhysicsObject();
	virtual ~PhysicsObject();
private:
	
	PhysicsNode* m_node;
};

