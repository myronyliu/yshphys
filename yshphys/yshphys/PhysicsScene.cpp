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

void PhysicsScene::ComputeContacts()
{
	std::vector<BVNodePair> intersectingLeaves = m_bvTree.Root()->FindIntersectingLeaves();
	for (BVNodePair pair : intersectingLeaves)
	{
		Contact contact;

		contact.body[0] = (RigidBody*)pair.nodes[0]->GetContent();
		contact.body[1] = (RigidBody*)pair.nodes[1]->GetContent();

		Geometry* geom0 = contact.body[0]->GetGeometry();
		Geometry* geom1 = contact.body[1]->GetGeometry();

		dVec3 pos0, pos1;
		dQuat rot0, rot1;

		contact.body[0]->GetGeometryGlobalTransform(pos0, rot0);
		contact.body[1]->GetGeometryGlobalTransform(pos1, rot1);

		if (Geometry::Intersect(geom0, pos0, rot0, contact.x[0], contact.n[0], geom1, pos1, rot1, contact.x[1], contact.n[1]))
		{
			contact.n[0] = -contact.n[0];
			contact.n[1] = -contact.n[1];

			Island* island[2];
			island[0] = contact.body[0]->GetIsland();
			island[1] = contact.body[1]->GetIsland();

			if (island[0] == nullptr && island[1] == nullptr)
			{
				Island* newIsland = new Island();
				newIsland->AddContact(contact);

				if (m_firstIsland == nullptr)
				{
					m_firstIsland = newIsland;
				}

				// Add the new island to the end of the "ring"

				newIsland->m_next = m_firstIsland;
				newIsland->m_prev = m_firstIsland->m_prev->m_prev;

				m_firstIsland->m_prev = newIsland;
				m_firstIsland->m_next = newIsland->m_prev->m_next;
			}
			else if (island[0] == nullptr)
			{
				contact.body[0]->SetIsland(island[1]);
				island[1]->AddContact(contact);
			}
			else if (island[1] == nullptr)
			{
				contact.body[1]->SetIsland(island[0]);
				island[0]->AddContact(contact);
			}
			else if (island[0] == island[1])
			{
				island[0]->AddContact(contact);
			}
			else // both bodies are already associated with islands. Then we must merge the islands
			{
				if (m_firstIsland == island[1])
				{
					m_firstIsland = island[1]->m_next;
				}
				island[0]->Merge(island[1]);
			}
		}
	}

}

void PhysicsScene::ClearIslands()
{
	if (m_firstIsland != nullptr)
	{
		Island* island = m_firstIsland;
		do
		{
			Island* nextIsland = island->m_next;
			delete island;
			island = nextIsland;

		} while (island != m_firstIsland);
		m_firstIsland = nullptr;
	}

	PhysicsNode* node = m_firstNode;
	while (node != nullptr)
	{
		((RigidBody*)node->GetPhysicsObject())->SetIsland(nullptr);
		node = node->GetNext();
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
	ComputeContacts();
	ClearIslands();
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

		const bool intersecting = Geometry::Intersect(geom0, pos0, rot0, pt0, n0, geom1, pos1, rot1, pt1, n1);

		const float k = 0.1f;
		const fVec3 c = intersecting ? fVec3(0.0f, 1.0f, 0.0f) : fVec3(1.0f, 0.0f, 0.0f);

		renderer->DrawBox(k, k, k, pt0, rot0, c, false, false);
		renderer->DrawBox(k, k, k, pt1, rot1, c, false, false);
		renderer->DrawLine(pt0, pt1, fVec3(0.0f, 0.0f, 1.0f));

		if (intersecting)
		{
			dQuat qAlign0 = dQuat::Identity();
			dQuat qAlign1 = dQuat::Identity();

			const dVec3 cross = dVec3(0.0, 0.0, 1.0).Cross(n0);
			const double sinSqr = cross.Dot(cross);
			if (sinSqr > (double)FLT_EPSILON)
			{
				const double sin = sqrt(sinSqr);
				const double cos = n0.z;
				const dVec3 axis = cross.Scale(1.0 / sin);
				const double angle0 = atan2(sin, cos);
				const double angle1 = angle0 + dPI;
				qAlign0 = dQuat(axis, angle0);
				qAlign1 = dQuat(axis, angle1);
			}
			renderer->DrawBox(0.01f, 0.01f, 0.5f, fVec3(pt0 + n0.Scale(0.5)), fQuat(qAlign0), c, false, false);
			renderer->DrawBox(0.01f, 0.01f, 0.5f, fVec3(pt1 + n1.Scale(0.5)), fQuat(qAlign1), c, false, false);
		}
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