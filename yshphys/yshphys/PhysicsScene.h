#pragma once
#include "BVTree.h"
#include "RigidBody.h"
#include "PhysicsObject.h"
#include "PhysicsNode.h"
#include "DebugRenderer.h"
#include "Ray.h"
#include "Island.h"

#define MAX_PHYSICS_NODES 1024

struct PhysicsRayCastHit
{
	RigidBody* body;
	dVec3 offset;
};

class PhysicsScene
{
public:
	PhysicsScene();
	virtual ~PhysicsScene();

	void AddPhysicsObject(RigidBody* physicsObject);
	void RemovePhysicsObject(RigidBody* physicsObject);

	PhysicsRayCastHit RayCast(const Ray& ray) const;

	const BVTree& GetBVTree() const;

	void Step(double dt);

	void DebugDraw(DebugRenderer* renderer) const;

protected:
	void ComputeContacts();
	void ResolveContacts() const;
	void ClearIslands();

	std::stack<FreedPhysicsNode> m_freedNodeStack;
	PhysicsNode* m_physicsNodes;
	PhysicsNode* m_firstNode;

	BVTree m_bvTree;

	Island* m_firstIsland;
};

