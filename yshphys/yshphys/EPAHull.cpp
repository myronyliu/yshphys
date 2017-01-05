#include "stdafx.h"
#include "EPAHull.h"

bool EPAHull::CompareFacesByDistance(const Face* face0, const Face* face1)
{
	if (face0->distance > face1->distance)
	{
		return true;
	}
	else if (face0->distance < face1->distance)
	{
		return false;
	}
	else
	{
		return face0 > face1;
	}
}


EPAHull::EPAHull(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1,
		const GJKSimplex& tetrahedron)
{
	assert(tetrahedron.GetNumPoints() == 4);

	m_geom0.geom = geom0;
	m_geom0.pos = pos0;
	m_geom0.rot = rot0;

	m_geom1.geom = geom1;
	m_geom1.pos = pos1;
	m_geom1.rot = rot1;

	std::memset(m_faceValidities, 1, sizeof(m_faceValidities));

	for (int f = 0; f < EPAHULL_MAXFACES; ++f)
	{
		m_faces[f].index = f;
	}
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
		n = n.Scale(1.0 / sqrt(n.Dot(n)));

		m_faceHeap[f] = &m_faces[f];

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
	}

	for (int i = 0; i < EPAHULL_MAXFACES - 4; ++i)
	{
		m_freeFaces[i] = (EPAHULL_MAXFACES - 1) - i;
	}
	for (int i = 0; i < EPAHULL_MAXEDGES- 12; ++i)
	{
		m_freeEdges[i] = (EPAHULL_MAXEDGES - 1) - i;
	}

	m_nFreeFaces = EPAHULL_MAXFACES - 4;
	m_nFreeEdges = EPAHULL_MAXEDGES - 12;

	m_nFacesInHeap = 4;
	m_nVerts = 4;
	std::make_heap(m_faceHeap, &m_faceHeap[4], CompareFacesByDistance);
}

void EPAHull::CarveHorizon(const fVec3& fEye, const Face* visibleFace)
{
	const dVec3 dEye(fEye);
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->isHorizon = false;
	}
	m_nHorizonEdges = 0;

	int nFreedEdges = 0;

	std::vector<const Face*> visibleFaces;

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

	visibleFace->visible = true;
	visibleFaces.push_back(visibleFace);

	edgeStack.Push(visibleFace->edge);
	edgeStack.Push(visibleFace->edge->prev);
	edgeStack.Push(visibleFace->edge->next);

	HalfEdge* horizon = nullptr;

	while (!edgeStack.Empty())
	{
		HalfEdge* edge = edgeStack.Pop();
		HalfEdge* twin = edge->twin;

		Face* opposingFace = twin->face;

		const dVec3& n = opposingFace->normal;
		const dVec3 x(edge->vert->m_MinkDif);
		const double dot = (dEye - x).Dot(n);

		if ((float)dot > 0.0f)
		{
			if (!twin->prev->twin->face->visible)
			{
				edgeStack.Push(twin->prev);
			}
			if (!twin->next->twin->face->visible)
			{
				edgeStack.Push(twin->next);
			}
			PushFreeEdge(edge);
			PushFreeEdge(twin);
			nFreedEdges += 2;

			opposingFace->visible = true;
			visibleFaces.push_back(opposingFace);
		}
		else
		{
			horizon = edge;
		}
	}

	HalfEdge* edge = horizon;
	do
	{
		m_horizonEdges[m_nHorizonEdges++] = edge;
		edge->isHorizon = true;

		do
		{
			edge = edge->next->twin;

		} while (edge->face->visible);

		edge = edge->twin;

	} while (edge != horizon);

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		assert(m_horizonEdges[(i + 1) % m_nHorizonEdges]->twin->vert == m_horizonEdges[i]->vert);
	}

	for (const Face* visibleFace : visibleFaces)
	{
		visibleFace->visible = false;
	}


	std::cout << nFreedEdges << std::endl;
}

void EPAHull::PatchHorizon(const fMinkowskiPoint* eye)
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
		n = n.Scale(1.0 / sqrt(n.Dot(n)));
		const double d = D.Dot(n);
		face->normal = n;
		face->distance = d;

		PushFaceHeap(face);

		m_faceValidities[curr->face->index] = false;

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
}

bool EPAHull::Expand()
{
	while (m_nFacesInHeap > 0)
	{
		Face* closestFace = PopFaceHeap();
		if (m_faceValidities[closestFace->index])
		{
			const dVec3& n = closestFace->normal;
			const dVec3 pt0 = m_geom0.Support(n);
			const dVec3 pt1 = m_geom1.Support(-n);

			const dVec3 dEye(pt0 - pt1);
			const double deltaDist = dEye.Dot(n) - closestFace->distance;
			
			if (deltaDist < 0.0001 || (closestFace->distance > FLT_EPSILON && deltaDist / closestFace->distance < 0.01))
			{
				// The new point is so close to the face, that it doesnt warrant adding. So put the face back in the heap.
				PushFaceHeap(closestFace);
				return false;
			}

			const fVec3 fEye(dEye);
			CarveHorizon(fEye, closestFace);

			fMinkowskiPoint* newVert = &m_verts[m_nVerts];
			newVert->m_MinkDif = fEye;
			newVert->m_MinkSum = fVec3(pt0 + pt1);
			m_nVerts++;

			PatchHorizon(newVert);

			PushFreeFace(closestFace); // Only after we are done working with the horizon can we push the closestFace onto the free list
			assert(m_nFacesInHeap + m_nFreeFaces == EPAHULL_MAXFACES);

			const int eulerCharacteristic = EulerCharacteristic();
			assert(eulerCharacteristic == 2);

			return true;
		}
		PushFreeFace(closestFace);
	}
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

double EPAHull::ComputePenetration(dVec3& pt0, dVec3& pt1)
{
	for (int i = 0; i < EPAHULL_MAXITERS; ++i)
	{
		if (!Expand())
		{
			Face* closestFace = PopFaceHeap();
			PushFaceHeap(closestFace);

			dMinkowskiPoint pt = closestFace->ComputeClosestPointToOrigin();
			pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
			pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);

			return sqrt((pt0 - pt1).Dot(pt0 - pt1));
		}
	}
	Face* closestFace = PopFaceHeap();
	PushFaceHeap(closestFace);

	dMinkowskiPoint pt = closestFace->ComputeClosestPointToOrigin();
	pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
	pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);

	return sqrt((pt0 - pt1).Dot(pt0 - pt1));
}

int EPAHull::EulerCharacteristic() const
{
	int nF = 0;
	std::set<HalfEdge*> heSet;
	std::set<const fMinkowskiPoint*> vSet;
	for (int i = 0; i < m_nFacesInHeap; ++i)
	{
		if (m_faceValidities[m_faceHeap[i]->index])
		{
			nF++;
			HalfEdge* e0 = m_faceHeap[i]->edge;
			HalfEdge* e = e0;
			do
			{
				heSet.insert(e);
				e = e->next;
			} while (e != e0);
		}
	}

	for (HalfEdge* he : heSet)
	{
		vSet.insert(he->vert);
	}

	const int nHE = (int)heSet.size();
	assert(nHE % 2 == 0);
	assert(nHE + m_nFreeEdges == EPAHULL_MAXEDGES);
	int nE = nHE / 2;

	const int nV = (int)vSet.size();
	return nV - nE + nF;
}

void EPAHull::DebugDraw(DebugRenderer* renderer) const
{
	fVec3 boxMin(0.0f, 0.0f, 0.0f);
	fVec3 boxMax(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < m_nFacesInHeap; ++i)
	{
		Face* face = m_faceHeap[i];
		if (m_faceValidities[face->index])
		{
			fVec3 ABC[3];

			ABC[0] = face->edge->vert->m_MinkDif;
			ABC[1] = face->edge->next->vert->m_MinkDif;
			ABC[2] = face->edge->next->next->vert->m_MinkDif;

			float x[5] = { boxMin.x, ABC[0].x, ABC[1].x, ABC[2].x, boxMax.x };
			float y[5] = { boxMin.y, ABC[0].y, ABC[1].y, ABC[2].y, boxMax.y };
			float z[5] = { boxMin.z, ABC[0].z, ABC[1].z, ABC[2].z, boxMax.z };

			boxMin.x = *std::min_element(x, x + 4);
			boxMin.y = *std::min_element(y, y + 4);
			boxMin.z = *std::min_element(z, z + 4);

			boxMax.x = *std::max_element(x + 1, x + 5);
			boxMax.y = *std::max_element(y + 1, y + 5);
			boxMax.z = *std::max_element(z + 1, z + 5);

			renderer->DrawPolygon(ABC, 3, fVec3(1.0f, 1.0f, 1.0f), false);
		}
	}

	std::set<HalfEdge*> heSet;
	for (int i = 0; i < m_nFacesInHeap; ++i)
	{
		if (m_faceValidities[m_faceHeap[i]->index])
		{
			HalfEdge* e0 = m_faceHeap[i]->edge;
			HalfEdge* e = e0;
			do
			{
				heSet.insert(e);
				e = e->next;
			} while (e != e0);
		}
	}

	float hullSpan[3] = { boxMax.x - boxMin.x, boxMax.y - boxMin.y, boxMax.z - boxMin.z };
	float edgeWidth = *std::min_element(hullSpan, hullSpan + 3) * 0.001f;

	for (HalfEdge* edge : heSet)
	{
		fVec3 color = (edge->isHorizon || edge->twin->isHorizon) ? fVec3(0.0f, 1.0f, 0.0f) : fVec3(0.0f, 0.0f, 0.0f);
//		renderer->DrawLine(edge->vert->m_MinkDif, edge->twin->vert->m_MinkDif, color);

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

		renderer->DrawBox(edgeWidth, edgeWidth, vLen*0.5f, (A + B).Scale(0.5f), rot, color, false, false);
	}
}