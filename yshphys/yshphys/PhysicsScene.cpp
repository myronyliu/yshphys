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

PhysicsRayCastHit PhysicsScene::RayCast(const Ray& ray) const
{
	PhysicsRayCastHit hit;
	hit.body = nullptr;

	const BVNode* node = m_bvTree.Root();
	if (!node)
	{
		return hit;
	}
	std::stack<const BVNode*> nodeStack;
	nodeStack.push(node);

	struct CandidateLeaf
	{
		double tMin, tMax;
		const BVNode* node;

		bool operator < (const CandidateLeaf& candidate)
		{
			if (tMin < candidate.tMin)
			{
				return true;
			}
			else if (tMin > candidate.tMin)
			{
				return false;
			}

			if (tMax < candidate.tMax)
			{
				return true;
			}
			else if (tMax > candidate.tMax)
			{
				return false;
			}

			if (node < candidate.node)
			{
				return true;
			}
			else if (node > candidate.node)
			{
				return false;
			}
			return false;
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
				candidate.tMin = tMin;
				candidate.tMax = tMax;
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
		return hit;
	}

	std::sort(candidateLeaves.begin(), candidateLeaves.end());

	double tBest = 888888888.0f;

	dVec3 o = ray.GetOrigin();
	dVec3 d = ray.GetDirection();

	for (int i = 0; i < candidateLeaves.size(); ++i)
	{
		const double& tMin_AABB = candidateLeaves[i].tMin;

		if (tMin_AABB > tBest)
		{
			break;
		}

		Ray ray_shifted = ray;
		ray_shifted.SetOrigin(dVec3(0.0, 0.0, 0.0));

		RigidBody* rigidBody = (RigidBody*)candidateLeaves[i].node->GetContent();

		const dQuat q0 = rigidBody->GetRotation();

		dVec3 x;
		dQuat q;

		rigidBody->GetGeometry();
		rigidBody->GetGeometryGlobalTransform(x, q);

		const dVec3 x0_shifted = rigidBody->GetPosition() - (o + d.Scale(tMin_AABB));
		const dVec3 x_shifted = x - (o + d.Scale(tMin_AABB));

		double tMin_shifted, tMax_shifted;
		if (ray_shifted.IntersectOOBB(rigidBody->GetGeometry()->GetLocalOOBB(), x_shifted, q, tMin_shifted, tMax_shifted))
		{
			if (tMin_shifted + tMin_AABB < tBest)
			{
				dVec3 hitPt_shifted;
				if (rigidBody->GetGeometry()->RayIntersect(x_shifted, q, ray_shifted, hitPt_shifted))
				{
					const double t = (hitPt_shifted).Dot(d) + tMin_AABB;

					if (t < tBest)
					{
						hit.body = rigidBody;
						hit.offset = (-q0).Transform(hitPt_shifted - x0_shifted);
						tBest = t;
					}
				}
			}
		}
		
	}
	return hit;
}

const BVTree& PhysicsScene::GetBVTree() const
{
	return m_bvTree;
}

void PhysicsScene::AddPhysicsObject(RigidBody* physicsObject)
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

void PhysicsScene::RemovePhysicsObject(RigidBody* physicsObject)
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

void PhysicsScene::ComputeContacts() const
{
	std::vector<BVNodePair> intersectingLeaves = m_bvTree.Root()->FindIntersectingLeaves();
	for (BVNodePair pair : intersectingLeaves)
	{
		RigidBody* rb0 = (RigidBody*)pair.nodes[0]->GetContent();
		RigidBody* rb1 = (RigidBody*)pair.nodes[1]->GetContent();

		Geometry* geom0 = rb0->GetGeometry();
		Geometry* geom1 = rb1->GetGeometry();

		dVec3 pos0, pos1;
		dQuat rot0, rot1;

		rb0->GetGeometryGlobalTransform(pos0, rot0);
		rb1->GetGeometryGlobalTransform(pos1, rot1);

		dVec3 pt0, pt1;

//		const double d = Geometry::ComputeSeparation(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1);

//		if (d <= 0.0)
//		{

//		}
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

void PhysicsScene::DebugDraw(DebugRenderer* renderer) const
{
	const BVNode* root = m_bvTree.Root();
	assert(root->GetParent() == nullptr);
	if (!root)
	{
		return;
	}

	std::vector<BVNodePair> intersectingLeaves = root->FindIntersectingLeaves();
	for (BVNodePair pair: intersectingLeaves)
	{
		RigidBody* rb0 = (RigidBody*)pair.nodes[0]->GetContent();
		RigidBody* rb1 = (RigidBody*)pair.nodes[1]->GetContent();

		Geometry* geom0 = rb0->GetGeometry();
		Geometry* geom1 = rb1->GetGeometry();

		dVec3 pos0, pos1;
		dQuat rot0, rot1;

		rb0->GetGeometryGlobalTransform(pos0, rot0);
		rb1->GetGeometryGlobalTransform(pos1, rot1);

		dVec3 pt0, pt1, n0, n1;

		bool intersecting = Geometry::Intersect(geom0, pos0, rot0, pt0, n0, geom1, pos1, rot1, pt1, n1);

		const float k = 0.1f;
		const fVec3 c = intersecting ? fVec3(0.0f, 1.0f, 0.0f) : fVec3(1.0f, 0.0f, 0.0f);

		renderer->DrawBox(k, k, k, pt0, rot0, c, false, false);
		renderer->DrawBox(k, k, k, pt1, rot1, c, false, false);

		renderer->DrawLine(pt0, pt1, fVec3(0.0f, 0.0f, 1.0f));
	}

	std::stack<const BVNode*> nodeStack;
	nodeStack.push(root);
	while (!nodeStack.empty())
	{
		const BVNode* node = nodeStack.top();
		nodeStack.pop();

		const AABB aabb = node->GetAABB();

		const fVec3 aabbCenter(
			float(aabb.max.x + aabb.min.x)*0.5f,
			float(aabb.max.y + aabb.min.y)*0.5f,
			float(aabb.max.z + aabb.min.z)*0.5f);

		renderer->DrawBox(
			float(aabb.max.x - aabb.min.x)*0.5f,
			float(aabb.max.y - aabb.min.y)*0.5f,
			float(aabb.max.z - aabb.min.z)*0.5f,
			aabbCenter, fQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), true
		);

		if (!node->IsLeaf())
		{
			nodeStack.push(node->GetLeftChild());
			nodeStack.push(node->GetRightChild());
		}
	}
}