#pragma once
#include "BVTree.h"
#include "RigidBody.h"
#include "PhysicsObject.h"
class PhysicsNode
{
	friend class PhysicsScene;
public:
	PhysicsObject* GetPhysicsObject() const;
	PhysicsNode* GetPrev() const;
	PhysicsNode* GetNext() const;

private:
	PhysicsNode();
	virtual ~PhysicsNode();

	void BindPhysicsObject(PhysicsObject* physicsObject);
	void Remove();
	void AppendTo(PhysicsNode* prev);
	void PrependTo(PhysicsNode* next);

	PhysicsObject* m_physicsObject;

	PhysicsNode* m_next;
	PhysicsNode* m_prev;
};

class FreedPhysicsNode
{
public:
	FreedPhysicsNode();
	virtual ~FreedPhysicsNode();

	PhysicsNode* m_node;
	PhysicsNode* m_precedingNode;
};

class PhysicsScene
{
public:
	PhysicsScene();
	virtual ~PhysicsScene();
protected:
	RigidBody m_rigidBodies[MAX_BV_NODES];

	BVTree m_bvTree;
};

