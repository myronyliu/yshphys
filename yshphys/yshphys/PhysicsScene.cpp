#include "stdafx.h"
#include "PhysicsScene.h"
#include "Force_Constant.h"
#include "Material.h"

#define COLINEAR_ANGLE_THRESH (dPI*0.25)

PhysicsScene::PhysicsScene() : m_firstNode(nullptr), m_firstIsland(nullptr)
{
	Material::InitializeTables();

	m_physicsNodes = new PhysicsNode[MAX_PHYSICS_NODES];

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
	delete[] m_physicsNodes;
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

void CreateCoordinateSystem(dVec3& xAxis, dVec3& yAxis, const dVec3& zAxis)
{
	for (int i = 0; i < 3; ++i)
	{
		const int j = (i + 1) % 3;
		const int k = (i + 2) % 3;

		if (abs(zAxis[i]) <= abs(zAxis[j]) && abs(zAxis[i]) <= abs(zAxis[k]))
		{
			yAxis;
			yAxis[i] = 0.0;
			yAxis[j] = zAxis[k];
			yAxis[k] = -zAxis[j];
			yAxis.Scale(1.0 / sqrt(zAxis[j] * zAxis[j] + zAxis[k] * zAxis[k]));

			xAxis = yAxis.Cross(zAxis);

			return;
		}
	}
	assert(false);
}

void PhysicsScene::ComputeContacts()
{
	assert(m_firstIsland == nullptr);
	std::vector<BVNodePair> intersectingLeaves = m_bvTree.Root()->FindIntersectingLeaves();
	for (BVNodePair pair : intersectingLeaves)
	{
		Contact contact;

		RigidBody* body[2];

		body[0] = (RigidBody*)pair.nodes[0]->GetContent();
		body[1] = (RigidBody*)pair.nodes[1]->GetContent();

		if (body[0]->IsStatic() && body[1]->IsStatic())
		{
			continue;
		}

		contact.body[0] = body[0];
		contact.body[1] = body[1];

		Geometry* geom0 = contact.body[0]->GetGeometry();
		Geometry* geom1 = contact.body[1]->GetGeometry();

		dVec3 pos0, pos1;
		dQuat rot0, rot1;

		contact.body[0]->GetGeometryGlobalTransform(pos0, rot0);
		contact.body[1]->GetGeometryGlobalTransform(pos1, rot1);

		dVec3 x0, x1, n0, n1;

		if (Geometry::Intersect(geom0, pos0, rot0, x0, n0, geom1, pos1, rot1, x1, n1))
		{
//			assert(abs(n0.z) > 0.999);

			const double k = 256.0;
			double penetration = (x1 - x0).Dot(n1);
			assert(penetration > 0.0);
			penetration = std::min(0.05, penetration);
			const dVec3 d = n1.Scale(penetration);

			Force_Constant* penalty0 = new Force_Constant();
			Force_Constant* penalty1 = new Force_Constant();
			penalty0->offset = dVec3(0.0, 0.0, 0.0);
			penalty1->offset = dVec3(0.0, 0.0, 0.0);
			penalty0->F = d.Scale(contact.body[0]->GetMass()*k);
			penalty1->F = -d.Scale(contact.body[1]->GetMass()*k);
			contact.body[0]->ApplyBruteForce(penalty0);
			contact.body[1]->ApplyBruteForce(penalty1);

			assert(abs(penalty0->F.x) < 1000000.0f);
			assert(abs(penalty0->F.y) < 1000000.0f);
			assert(abs(penalty0->F.z) < 1000000.0f);
			assert(abs(penalty1->F.x) < 1000000.0f);
			assert(abs(penalty1->F.y) < 1000000.0f);
			assert(abs(penalty1->F.z) < 1000000.0f);

			contact.n[0] = -n0;
			contact.n[1] = -n1;

			dVec3 xHat, yHat;

			CreateCoordinateSystem(xHat,yHat,n0);
			dVec3 xPlane = (x0 + x1).Scale(0.5);

			dMat33 RPlane0;
			RPlane0.SetColumn(0, xHat);
			RPlane0.SetColumn(1, yHat);
			RPlane0.SetColumn(2, n0);

			Polygon poly0 = geom0->IntersectPlane(pos0, rot0, xPlane, n0, xHat, yHat);
			Polygon poly1 = geom1->IntersectPlane(pos1, rot1, xPlane, -n0, -xHat, yHat);
			Polygon intersectionPoly;
			int nVerts0, nVerts1;
			const fVec2* verts0 = poly0.GetVertices(nVerts0);
			const fVec2* verts1 = poly1.GetVertices(nVerts1);
			if (nVerts0 == 1)
			{
				intersectionPoly = poly0;
			}
			else if (nVerts1 == 1)
			{
				intersectionPoly = poly1.ReflectX();
			}
			else
			{
				intersectionPoly = poly0.Intersect(poly1.ReflectX());
			}
			intersectionPoly = intersectionPoly.PruneColinearVertices(COLINEAR_ANGLE_THRESH);
//			intersectionPoly = intersectionPoly.LimitVertices(3);

			int nVerts;
			const fVec2* verts = intersectionPoly.GetVertices(nVerts);

			Island* island[2];
			island[0] = contact.body[0]->GetIsland();
			island[1] = contact.body[1]->GetIsland();

			auto CreateNewIsland = [&]()
			{
				Island* newIsland = new Island();
				if (m_firstIsland == nullptr)
				{
					m_firstIsland = newIsland;
				}
				// Add the new island to the end of the "ring"
				newIsland->PrependTo(m_firstIsland);
				return newIsland;
			};

			Island* isl = nullptr;

			if (body[0]->IsStatic())
			{
				isl = (island[1] == nullptr) ? CreateNewIsland() : island[1];
			}
			else if (body[1]->IsStatic())
			{
				isl = (island[0] == nullptr) ? CreateNewIsland() : island[0];
			}
			else // both bodies are nonstatic
			{
				if (island[0] == nullptr && island[1] == nullptr)
				{
					isl = CreateNewIsland();
				}
				else if (island[0] == nullptr)
				{
					isl = island[1];
				}
				else if (island[1] == nullptr)
				{
					isl = island[0];
				}
				else if (island[0] == island[1])
				{
					isl = island[0];
				}
				else // both bodies are already associated with different islands. Then we must merge the islands
				{
					if (m_firstIsland == island[1])
					{
						assert(island[1]->m_next != island[1]);
						m_firstIsland = island[1]->m_next;
					}
					isl = island[0]->Merge(island[1]);
				}
			}

			if (m_firstIsland != nullptr)
			{
				int n0 = 0;
				int n1 = 0;

				Island* is = m_firstIsland;
				do
				{
					is = is->m_next;
					n0++;
				} while (is != m_firstIsland);

				is = m_firstIsland;
				do
				{
					is = is->m_prev;
					n1++;
				} while (is != m_firstIsland);

				assert(n0 == n1);
			}

			for (int i = 0; i < nVerts; ++i)
			{
				dVec3 x = xPlane + RPlane0.Transform(xHat.Scale((double)verts[i].x) + yHat.Scale((double)verts[i].y));
				contact.x[0] = x;
				contact.x[1] = x;

				isl->AddContact(contact);
			}
		}
	}
}

void PhysicsScene::ResolveContacts() const
{
	if (m_firstIsland != nullptr)
	{
		Island* island = m_firstIsland;
		do
		{
			island->ResolveContacts();
			island = island->m_next;

		} while (island != m_firstIsland);
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
			if (nextIsland == island)
			{
				assert(island == m_firstIsland);
				int asdf = 0;
			}
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
		RigidBody* body = ((RigidBody*)node->GetPhysicsObject());
		body->ApplyForceAtCOM(dVec3(0.0, 0.0, -9.8).Scale(body->GetMass()));
		node = node->GetNext();
	}

	ComputeContacts();
	ResolveContacts();
	ClearIslands();

	node = m_firstNode;
	while (node != nullptr)
	{
		RigidBody* body = ((RigidBody*)node->GetPhysicsObject());
		node->GetPhysicsObject()->Step(dt);
		node = node->GetNext();
	}
}

void PhysicsScene::DebugDraw(DebugRenderer* renderer) const
{
//	return;
#if 1 
	std::vector<BVNodePair> intersectingLeaves = m_bvTree.Root()->FindIntersectingLeaves();
	for (BVNodePair pair : intersectingLeaves)
	{
		RigidBody* body0 = (RigidBody*)pair.nodes[0]->GetContent();
		RigidBody* body1 = (RigidBody*)pair.nodes[1]->GetContent();

		Geometry* geom0 = body0->GetGeometry();
		Geometry* geom1 = body1->GetGeometry();

		dVec3 pos0, pos1;
		dQuat rot0, rot1;

		body0->GetGeometryGlobalTransform(pos0, rot0);
		body1->GetGeometryGlobalTransform(pos1, rot1);

		dVec3 x0, x1, n0, n1;

		if (Geometry::Intersect(geom0, pos0, rot0, x0, n0, geom1, pos1, rot1, x1, n1))
		{
			dVec3 xHat, yHat;

			CreateCoordinateSystem(xHat, yHat, n0);
			dVec3 xPlane = (x0 + x1).Scale(0.5);

			dMat33 RPlane0;
			RPlane0.SetColumn(0, xHat);
			RPlane0.SetColumn(1, yHat);
			RPlane0.SetColumn(2, n0);

			Polygon poly0 = geom0->IntersectPlane(pos0, rot0, xPlane, n0, xHat, yHat);
			Polygon poly1 = geom1->IntersectPlane(pos1, rot1, xPlane, -n0, -xHat, yHat);
			Polygon intersectionPoly;
			int nVerts0, nVerts1;
			const fVec2* verts0 = poly0.GetVertices(nVerts0);
			const fVec2* verts1 = poly1.GetVertices(nVerts1);
			if (nVerts0 == 1)
			{
				intersectionPoly = poly0;
			}
			else if (nVerts1 == 1)
			{
				intersectionPoly = poly1.ReflectX();
			}
			else
			{
				intersectionPoly = poly0.Intersect(poly1.ReflectX());
			}
			intersectionPoly = intersectionPoly.PruneColinearVertices(COLINEAR_ANGLE_THRESH);
//			intersectionPoly = intersectionPoly.LimitVertices(3);

			int nVerts;
			const fVec2* verts = intersectionPoly.GetVertices(nVerts);
//			assert(nVerts > 0);

			const float k = 0.05f;
//			for (Polygon poly : { intersectionPoly })
//			{
//				verts = poly.GetVertices(nVerts);
//				for (int i = 0; i < nVerts; ++i)
//				{
//					const fVec3 x(xPlane + xHat.Scale((double)verts[i].x) + yHat.Scale((double)verts[i].y));
//					const fVec3 y(xPlane + xHat.Scale((double)verts[(i + 1) % nVerts].x) + yHat.Scale((double)verts[(i + 1) % nVerts].y));
//					renderer->DrawBox(k, k, k, x, dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), false, false);
//					renderer->DrawLine(x, y, fVec3(1.0f, 0.0f, 0.0f));
//				}
//			}
			for (Polygon poly : {poly0, poly1.ReflectX()})
			{
				verts = poly.GetVertices(nVerts);
				for (int i = 0; i < nVerts; ++i)
				{
					const fVec3 x(xPlane + xHat.Scale((double)verts[i].x) + yHat.Scale((double)verts[i].y));
					const fVec3 y(xPlane + xHat.Scale((double)verts[(i + 1) % nVerts].x) + yHat.Scale((double)verts[(i + 1) % nVerts].y));
					renderer->DrawLine(x, y, fVec3(0.0f, 1.0f, 0.0f));
					renderer->DrawBox(k, k, k, x, dQuat::Identity(), fVec3(0.0f, 1.0f, 0.0f), false, false);
				}
			}
		}
	}

#else
	std::vector<BVNodePair> intersectingLeaves = m_bvTree.Root()->FindIntersectingLeaves();
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
#endif

	m_bvTree.DebugDraw(renderer);
}