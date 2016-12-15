#include "stdafx.h"
#include "PhysicsScene.h"

PhysicsScene::PhysicsScene() : m_firstNode(nullptr)
{
	for (int i = 0; i < MAX_PHYSICS_NODES - 1; ++i)
	{
		m_physicsNodes[i].PrependTo(&m_physicsNodes[i + 1]);
	}

	for (int i = MAX_PHYSICS_NODES- 1; i > 0; --i)
	{
		FreedPhysicsNode freeNode;
		freeNode.m_node = &m_physicsNodes[i];
		freeNode.m_precedingNode = &m_physicsNodes[i - 1];
		m_freedNodeStack.push(freeNode);
	}
	FreedPhysicsNode freeNode;
	freeNode.m_node = &m_physicsNodes[0];
	freeNode.m_precedingNode = nullptr;
	m_freedNodeStack.push(freeNode);
}


PhysicsScene::~PhysicsScene()
{
}

PhysicsObject* PhysicsScene::RayCast(const Ray& ray) const
{
	dVec3 o = ray.GetOrigin();
	dVec3 d = ray.GetDirection();

	const BVNode* node = m_bvTree.Root();
	if (!node)
	{
		return nullptr;
	}
	std::stack<const BVNode*> nodeStack;
	nodeStack.push(node);

	struct CandidateLeaf
	{
		double t;
		const BVNode* node;

		bool operator < (const CandidateLeaf& candidate)
		{
			return t < candidate.t;
		}
	};
	std::vector<CandidateLeaf> candidateLeaves;

	while (!nodeStack.empty())
	{
		node = nodeStack.top();
		nodeStack.pop();

		double tMin, tMax;
		if (ray.IntersectAABB(node->GetAABB(), tMin, tMax))
		{
			if (node->IsLeaf())
			{
				CandidateLeaf candidate;
				candidate.t = tMin;
				candidate.node = node;
				candidateLeaves.push_back(candidate);
			}
			else
			{
				nodeStack.push(node->GetLeftChild());
				nodeStack.push(node->GetRightChild());
			}
		}
	}

	if (candidateLeaves.empty())
	{
		return nullptr;
	}

	std::sort(candidateLeaves.begin(), candidateLeaves.end());
	for (int i = 0; i < candidateLeaves.size(); ++i)
	{
		// TODO: Add the actual intersection test
	}

	return (PhysicsObject*)candidateLeaves.begin()->node->GetContent();
}

const BVTree& PhysicsScene::GetBVTree() const
{
	return m_bvTree;
}

void PhysicsScene::AddPhysicsObject(PhysicsObject* physicsObject)
{
	if (!m_freedNodeStack.empty())
	{
		FreedPhysicsNode& freeNode = m_freedNodeStack.top();
		freeNode.m_node->BindPhysicsObject(physicsObject);

		if (freeNode.m_precedingNode)
		{
			freeNode.m_node->AppendTo(freeNode.m_precedingNode);
		}
		else
		{
			freeNode.m_node->PrependTo(m_firstNode);
			m_firstNode = freeNode.m_node;
		}

		m_freedNodeStack.pop();

		physicsObject->UpdateAABB();
		m_bvTree.DeepInsertNewLeaf(physicsObject->GetAABB(), physicsObject);
	}
}

void PhysicsScene::RemovePhysicsObject(PhysicsObject* physicsObject)
{
	if (PhysicsNode* node = physicsObject->GetPhysicsNode())
	{
		if (node->GetPrev() == nullptr && node->GetNext() != nullptr)
		{
			m_firstNode = node->GetNext();
		}
		node->BindPhysicsObject(nullptr);
		FreedPhysicsNode freeNode;
		freeNode.m_precedingNode = node->GetPrev();
		freeNode.m_node = node;
		node->Remove();
		m_freedNodeStack.push(freeNode);

		physicsObject->GetBVNode()->Detach();
	}
}

void PhysicsScene::Step(double dt)
{
	PhysicsNode* node = m_firstNode;
	while (node != nullptr)
	{
		node->GetPhysicsObject()->Step(dt);
		node = node->GetNext();
	}
}