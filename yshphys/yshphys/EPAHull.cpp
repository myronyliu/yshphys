#include "stdafx.h"
#include "EPAHull.h"

void EPAHull::FaceHeap::HeapifyUp(int index)
{
	int i = index;
	int j = (i + 1) / 2 - 1; // index of i's parent

	while (i > 0 && m_faces[i]->distance < m_faces[j]->distance)
	{
		Face* swp = m_faces[i];
		m_faces[i] = m_faces[j];
		m_faces[j] = swp;

		m_faces[i]->iHeap = i;
		m_faces[j]->iHeap = j;

		i = j;
		j = (i + 1) / 2 - 1;
	}
}
void EPAHull::FaceHeap::HeapifyDown(int index)
{
	int i = index;

	while (true)
	{
		int iLeft = 2 * (i + 1) - 1;
		int iRight = iLeft + 1;
		if (iRight > m_nFaces)
		{
			break;
		}
		else
		{
			int j;
			if (iRight == m_nFaces)
			{
				j = iLeft;
			}
			else // ... if (iRight < m_nElements)
			{
				j = (m_faces[iLeft]->distance <= m_faces[iRight]->distance) ? iLeft : iRight;
			}

			if (m_faces[j]->distance < m_faces[i]->distance)
			{
				Face* swp = m_faces[i];
				m_faces[i] = m_faces[j];
				m_faces[j] = swp;

				m_faces[i]->iHeap = i;
				m_faces[j]->iHeap = j;

				i = j;
			}
			else
			{
				break;
			}
		}
	}
}
void EPAHull::FaceHeap::RemoveFaceFromHeap(Face* face)
{
	const int& i = face->iHeap;
	RemoveFaceFromHeap(face->iHeap);
}
void EPAHull::FaceHeap::RemoveFaceFromHeap(int i)
{
	m_faces[i]->iHeap = -1;
	m_faces[i] = m_faces[--m_nFaces];
	m_faces[i]->iHeap = i;
	HeapifyDown(i);
}
void EPAHull::FaceHeap::Push(Face* face)
{
	face->iHeap = m_nFaces;
	m_faces[m_nFaces] = face;
	HeapifyUp(m_nFaces++);
}
void EPAHull::FaceHeap::Pop()
{
	RemoveFaceFromHeap((int)0);
}
EPAHull::Face* EPAHull::FaceHeap::Top() const
{
	return m_faces[0];
}

EPAHull::EPAHull(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1,
	const GJKSimplex& tetrahedron) :
	m_nHorizonEdges(0)
{
	assert(tetrahedron.GetNumPoints() == 4);

	m_geom0.geom = geom0;
	m_geom0.pos = pos0;
	m_geom0.rot = rot0;

	m_geom1.geom = geom1;
	m_geom1.pos = pos1;
	m_geom1.rot = rot1;

	for (int e = 0; e < EPAHULL_MAXEDGES; ++e)
	{
		m_edges[e].index = e;
	}

	m_verts[0] = fMinkowskiPoint(tetrahedron.m_pts[0]);
	m_verts[1] = fMinkowskiPoint(tetrahedron.m_pts[1]);
	m_verts[2] = fMinkowskiPoint(tetrahedron.m_pts[2]);
	m_verts[3] = fMinkowskiPoint(tetrahedron.m_pts[3]);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			m_edges[3 * i + j].twin = &m_edges[3 * j + (i - 1)];
			m_edges[3 * i + j].vert = &m_verts[j];
		}
		for (int j = i + 1; j < 4; ++j)
		{
			m_edges[3 * i + (j - 1)].twin = &m_edges[3 * j + i];
			m_edges[3 * i + (j - 1)].vert = &m_verts[j];
		}
	}

	const dVec3 v[4] =
	{
		dVec3(m_verts[0].m_MinkDif),
		dVec3(m_verts[1].m_MinkDif),
		dVec3(m_verts[2].m_MinkDif),
		dVec3(m_verts[3].m_MinkDif)
	};

	for (int f = 0; f < 4; ++f)
	{
		const int iA = (f + 0) % 4;
		const int iB = (f + 1) % 4;
		const int iC = (f + 2) % 4;
		const int iD = (f + 3) % 4;
		const dVec3& A = v[iA];
		const dVec3& B = v[iB];
		const dVec3& C = v[iC];
		const dVec3& D = v[iD];

		dVec3 n = (C - B).Cross(D - B);
		assert(n.Dot(n) > FLT_MIN);
		n = n.Scale(1.0 / sqrt(n.Dot(n)));

		HalfEdge * e[3];

		if (n.Dot(B - A) < 0.0)
		{
			e[0] = &m_edges[3 * iC + iB - (iC < iB)];
			e[1] = &m_edges[3 * iB + iD - (iB < iD)];
			e[2] = &m_edges[3 * iD + iC - (iD < iC)];

			n = -n;
		}
		else
		{
			e[0] = &m_edges[3 * iB + iC - (iB < iC)];
			e[1] = &m_edges[3 * iC + iD - (iC < iD)];
			e[2] = &m_edges[3 * iD + iB - (iD < iB)];
		}

		for (int i = 0; i < 3; ++i)
		{
			e[i]->next = e[(i + 1) % 3];
			e[i]->prev = e[(i + 2) % 3];
			e[i]->face = &m_faces[f];
		}
		m_faces[f].edge = e[0];

		const double d = B.Dot(n);

		m_faces[f].normal = n;
		m_faces[f].distance = d;

		m_faceHeap.Push(&m_faces[f]);
	}

	for (int i = 0; i < EPAHULL_MAXFACES - 4; ++i)
	{
		m_freeFaces[i] = &m_faces[(EPAHULL_MAXFACES - 1) - i];
	}
	for (int i = 0; i < EPAHULL_MAXEDGES- 12; ++i)
	{
		m_freeEdges[i] = (EPAHULL_MAXEDGES - 1) - i;
	}

	m_nFreeFaces = EPAHULL_MAXFACES - 4;
	m_nFreeEdges = EPAHULL_MAXEDGES - 12;

	m_nVerts = 4;
}

void EPAHull::CarveHorizon(const fVec3& fEye, Face* visibleFace)
{
	const dVec3 dEye(fEye);
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->isHorizon = false;
	}
	m_nHorizonEdges = 0;

	struct EdgeStack
	{
	public:
		void Push(HalfEdge* edge) { e[n++] = edge; }
		HalfEdge* Pop() { return e[--n]; }
		bool Empty() const { return n == 0; }
	private:
		HalfEdge* e[1024];
		int n = 0;
	}
	edgeStack;

	std::vector<Face*> visitedFaces;
	auto MarkFaceAsVisited = [&](Face* face)
	{
		visitedFaces.push_back(face);
		face->visited = true;
	};

	for (HalfEdge* e : { visibleFace->edge, visibleFace->edge->prev, visibleFace->edge->next })
	{
		edgeStack.Push(e);
		MarkFaceAsVisited(e->twin->face);
	}
	MarkFaceAsVisited(visibleFace);
	visibleFace->visible = true;

	m_faceHeap.RemoveFaceFromHeap(visibleFace);
	PushFreeFace(visibleFace);

	int nFreedEdges = 0;
	std::vector<HalfEdge*> freedEdges;

	HalfEdge* horizonStart = nullptr;

	while (!edgeStack.Empty())
	{
		HalfEdge* edge = edgeStack.Pop();
		HalfEdge* twin = edge->twin;

		Face* opposingFace = twin->face;

		const dVec3& n = opposingFace->normal;
		const dVec3 x(edge->vert->m_MinkDif);
		const double dot = (dEye - x).Dot(n);

//		assert(abs(dot) > 8.0*FLT_MIN);

		if ((float)dot > 0.0f)
		{
			m_faceHeap.RemoveFaceFromHeap(opposingFace);
			PushFreeFace(opposingFace);

			if (!twin->prev->twin->face->visited)
			{
				edgeStack.Push(twin->prev);
				MarkFaceAsVisited(twin->prev->twin->face);
			}
			if (!twin->next->twin->face->visited)
			{
				edgeStack.Push(twin->next);
				MarkFaceAsVisited(twin->next->twin->face);
			}
			PushFreeEdge(edge);
			PushFreeEdge(twin);
			nFreedEdges += 2;

			freedEdges.push_back(edge);
			freedEdges.push_back(twin);

			opposingFace->visible = true;
		}
		else
		{
			horizonStart = edge;
		}
	}

	HalfEdge* edge = horizonStart;
	do
	{
		assert(m_nHorizonEdges < EPAHULL_MAXHORIZONEDGES);
		m_horizonEdges[m_nHorizonEdges++] = edge;
		edge->isHorizon = true;

		HalfEdge* e0 = edge;

		do
		{
			edge = edge->next->twin;

		} while (edge->face->visible);

		/////////////////////////////////////////
		// Make sure the horizon is not "pinched"
		HalfEdge* e = edge;
		while (e != e0)
		{
			assert(!e->face->visible);
			e = e->next->twin;
		}
		/////////////////////////////////////////

		edge = edge->twin;

	} while (edge != horizonStart);

	for (Face* face: visitedFaces)
	{
		face->visible = false;
		face->visited = false;
	}
}

bool EPAHull::PatchHorizon(const fMinkowskiPoint* eye)
{
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		HalfEdge* curr = m_horizonEdges[i];

		// We need to use a new face because of our sorted heap mumbo jumbo

		Face* face = PopFreeFace();

		HalfEdge* prev = PopFreeEdge();
		HalfEdge* next = PopFreeEdge();

		face->edge = curr;
		const dVec3 B(curr->vert->m_MinkDif);
		const dVec3 A(curr->twin->vert->m_MinkDif);
		const dVec3 D(eye->m_MinkDif);
		dVec3 n = (A - D).Cross(B - D);
//		assert(n.Dot(n) > FLT_MIN);
		if (n.Dot(n) < FLT_MIN)
		{
			return false;
		}
		n = n.Scale(1.0 / sqrt(n.Dot(n)));
		const double d = D.Dot(n);
		face->normal = n;
		face->distance = d;

//		assert(d >= 0.0);

		m_faceHeap.Push(face);

		prev->face = face;
		curr->face = face;
		next->face = face;

		prev->vert = curr->twin->vert;
		next->vert = eye;

		prev->next = curr;
		curr->next = next;
		next->next = prev;

		prev->prev = next;
		curr->prev = prev;
		next->prev = curr;
	}

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->next->twin = m_horizonEdges[(i + 1) % m_nHorizonEdges]->prev;
		m_horizonEdges[i]->prev->twin = m_horizonEdges[(i - 1 + m_nHorizonEdges) % m_nHorizonEdges]->next;
	}
	return true;
}

bool EPAHull::Expand()
{
	if (!m_faceHeap.Empty())
	{
		Face* closestFace = m_faceHeap.Top();

		const dVec3& n = closestFace->normal;
		const dVec3 pt0 = m_geom0.Support(n);
		const dVec3 pt1 = m_geom1.Support(-n);

		const dVec3 dEye(pt0 - pt1);
		const double deltaDist = dEye.Dot(n) - closestFace->distance;

		if (deltaDist < 0.0001 || (closestFace->distance > FLT_EPSILON && deltaDist / closestFace->distance < 0.01))
		{
			// The new point is so close to the face, that it doesnt warrant adding. So leave the heap unmodified
			return false;
		}

		const fVec3 fEye(dEye);
		CarveHorizon(fEye, closestFace);

		fMinkowskiPoint* newVert = &m_verts[m_nVerts];
		newVert->m_MinkDif = fEye;
		newVert->m_MinkSum = fVec3(pt0 + pt1);
		m_nVerts++;

		if (!PatchHorizon(newVert))
		{
			return false;
		}

		SanityCheck();

		return true;
	}
	assert(false);
	return false;
}

dMinkowskiPoint EPAHull::Face::ComputeClosestPointToOrigin() const
{
	GJKSimplex triangle;
	triangle.AddPoint(dMinkowskiPoint(*edge->vert));
	triangle.AddPoint(dMinkowskiPoint(*edge->next->vert));
	triangle.AddPoint(dMinkowskiPoint(*edge->prev->vert));
	return triangle.ClosestPointToOrigin(GJKSimplex());
}

static int maxIters = 0;

bool EPAHull::ComputeIntersection(dVec3& pt0, dVec3& n0, dVec3& pt1, dVec3& n1)
{
	auto UpdateIntersection = [&]()
	{
		Face* closestFace = m_faceHeap.Top();

		dMinkowskiPoint pt = closestFace->ComputeClosestPointToOrigin();
		pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
		pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);

		n0 = closestFace->normal;
		n1 = -n0;

		return (float)closestFace->distance >= 0.0f;
	};

	for (int i = 0; i < EPAHULL_MAXITERS; ++i)
	{
		if (!Expand())
		{
			if (i > maxIters)
			{
				maxIters = i;
				std::printf("EPA worst case iterations till convergence: %d\n", maxIters);
			}
			return UpdateIntersection();
		}
	}
	assert(false);
	return UpdateIntersection();
}

void EPAHull::SanityCheck() const
{
	// Verify that the horizon forms a cycle and that the horizon has no "coinciding" edges
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		assert(m_horizonEdges[(i + 1) % m_nHorizonEdges]->twin->vert == m_horizonEdges[i]->vert);
		assert(std::find(m_horizonEdges, m_horizonEdges + m_nHorizonEdges, m_horizonEdges[i]->twin) == m_horizonEdges + m_nHorizonEdges);
	}

	int nF = 0;
	int nHE = 0;

	std::vector<const HalfEdge*> edges;
	std::set<const fMinkowskiPoint*> verts;

	std::vector<Face*> faces = m_faceHeap.GetFaces();

	for (Face* f : faces)
	{
		HalfEdge* e[3];
		e[0] = f->edge;
		e[1] = f->edge->next;
		e[2] = f->edge->prev;

		// Verify that the face is a triangle
		assert(e[0]->next->next->next == e[0]);
		assert(e[0]->prev->prev->prev == e[0]);

		// Verify the two endpoints of each edge
		assert(e[0]->vert == e[1]->twin->vert);
		assert(e[1]->vert == e[2]->twin->vert);
		assert(e[2]->vert == e[0]->twin->vert);

		edges.push_back(e[0]);
		edges.push_back(e[1]);
		edges.push_back(e[2]);

		nF++;
		nHE += 3;
	}

	// Verify that there are an even number of triangles
	assert(nF % 2 == 0);

	for (const HalfEdge* e : edges)
	{
		verts.insert(e->vert);
	}
	const int nV = (int)verts.size();

	// Verify that each point we added in the expansion ended up on the hull
	assert(nV == m_nVerts);

	const int nE = nHE / 2;

	// Verify EULER'S CHARACTERISTIC
	assert(nV - nE + nF == 2);

	// Verify that all the new edges and the horizon are convex
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		const dVec3& n0 = m_horizonEdges[i]->face->normal;
		for (HalfEdge* e : { m_horizonEdges[i], m_horizonEdges[i]->next, m_horizonEdges[i]->prev })
		{
			const dVec3& n1 = e->twin->face->normal;

			const dVec3 AB(e->vert->m_MinkDif - e->twin->vert->m_MinkDif);

			const double AB_AB = AB.Dot(AB);

			assert((float)AB_AB > FLT_MIN);

			const dVec3 v = AB.Scale(1.0 / sqrt(AB_AB));

//			assert((float)n0.Cross(n1).Dot(v) >= 0.0f);
		}
	}


	// Verify the freelists
	assert(nHE + m_nFreeEdges == EPAHULL_MAXEDGES);
	assert((int)faces.size() + m_nFreeFaces == EPAHULL_MAXFACES);

	// Verify that all mutable face attributes are reset for the next pass
	for (const Face face : m_faces)
	{
		assert(!face.visited);
		assert(!face.visible);
	}
}

void EPAHull::DebugDraw(DebugRenderer* renderer) const
{
	std::set<const HalfEdge*> edges;

	std::vector<Face*> faces = m_faceHeap.GetFaces();

	for (Face* face : faces)
	{
		HalfEdge* es[3];
		es[0] = face->edge;
		es[1] = face->edge->next;
		es[2] = face->edge->prev;

		fVec3 ABC[3];
		ABC[0] = es[0]->vert->m_MinkDif;
		ABC[1] = es[1]->vert->m_MinkDif;
		ABC[2] = es[2]->vert->m_MinkDif;

		renderer->DrawPolygon(ABC, 3, fVec3(1.0f, 1.0f, 1.0f), false, false);

		for (const HalfEdge* e : es)
		{
			if (edges.find(e->twin) == edges.end())
			{
				edges.insert(e);
			}
		}
	}

	for (const HalfEdge* edge : edges)
	{
		fVec3 color(0.0f, 0.0f, 0.0f);
		float kWidth = 0.01f;
		if (edge->isHorizon || edge->twin->isHorizon)
		{
			color = fVec3(0.0f, 1.0f, 0.0f);
			kWidth *= 2.0f;
		}

		const fVec3& A = edge->vert->m_MinkDif;
		const fVec3& B = edge->twin->vert->m_MinkDif;

		fVec3 v = B - A;
		float vLen = sqrtf(v.Dot(v));
		fVec3 n = v.Scale(1.0f / vLen);
		fVec3 cross = fVec3(0.0f, 0.0f, 1.0f).Cross(n);
		float sin = sqrtf(cross.Dot(cross));
		float cos = n.z;

		fQuat rot = fQuat::Identity();
		if (sin > 0.00001f)
		{
			rot = fQuat(cross.Scale(1.0f / sin), std::atan2f(sin, cos));
		}

		auto FaceArea = [](const Face* face)
		{
			const fVec3& A = face->edge->vert->m_MinkDif;
			const fVec3& B =face->edge->next->vert->m_MinkDif;
			const fVec3& C = face->edge->prev->vert->m_MinkDif;
			const fVec3 cross = (B - A).Cross(C - A);
			return sqrtf(cross.Dot(cross));
		};

		const float edgeWidth = sqrtf(std::min(FaceArea(edge->face), FaceArea(edge->twin->face)))* kWidth;

		renderer->DrawBox(edgeWidth, edgeWidth, vLen*0.5f, (A + B).Scale(0.5f), rot, color, false, false);
	}
}