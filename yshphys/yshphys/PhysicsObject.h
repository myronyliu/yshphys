#pragma once
#include "Geometry.h"
#include "BVNode.h"
#include "GameObject.h"

class GameObject;

class PhysicsNode;

class PhysicsObject : public BVNodeContent
{
	friend class PhysicsNode;
public:
	PhysicsObject();
	virtual ~PhysicsObject();

	virtual dVec3 GetPosition() const = 0;
	virtual dQuat GetRotation() const = 0;

	PhysicsNode* GetPhysicsNode() const;

	AABB GetAABB() const;
	virtual void UpdateAABB() = 0;
	virtual void Step(double dt) = 0;

protected:
	PhysicsNode* m_node;

	// CACHED DATA
	AABB m_AABB;

	bool m_awake;

	GameObject* m_gameObject;
};

