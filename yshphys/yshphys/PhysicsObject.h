#pragma once
#include "Geometry.h"
#include "BVNode.h"

class PhysicsNode;

class PhysicsObject : public BVNodeContent
{
	friend class PhysicsNode;
public:
	PhysicsObject();
	virtual ~PhysicsObject();

	PhysicsNode* GetPhysicsNode() const;

	AABB GetAABB() const;
	virtual void UpdateAABB() = 0;
	virtual void Step(double dt) = 0;

protected:
	PhysicsNode* m_node;

	// CACHED DATA
	AABB m_AABB;
};

