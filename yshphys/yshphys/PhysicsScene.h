#pragma once
#include "BVTree.h"
#include "RigidBody.h"
#include "PhysicsObject.h"

#define MAX_PHYSICS_NODES 1024

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

	void AddPhysicsObject(PhysicsObject* physicsObject);
	void RemovePhysicsObject(PhysicsObject* physicsObject);
protected:

	std::stack<FreedPhysicsNode> m_freedNodeStack;
	PhysicsNode m_physicsNodes[MAX_PHYSICS_NODES];
	PhysicsNode* m_firstNode;

	BVTree m_bvTree;
};

