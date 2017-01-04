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

	m_verts[0] = tetrahedron.m_pts[0];
	m_verts[1] = tetrahedron.m_pts[1];
	m_verts[2] = tetrahedron.m_pts[2];
	m_verts[3] = tetrahedron.m_pts[3];

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
	for (int f = 0; f < 4; ++f)
	{
		const int iA = (f + 0) % 4;
		const int iB = (f + 1) % 4;
		const int iC = (f + 2) % 4;
		const int iD = (f + 3) % 4;
		const dVec3& A = m_verts[iA].m_MinkDif;
		const dVec3& B = m_verts[iB].m_MinkDif;
		const dVec3& C = m_verts[iC].m_MinkDif;
		const dVec3& D = m_verts[iD].m_MinkDif;

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

void EPAHull::CarveHorizon(const dVec3& eye, const Face* visibleFace)
{
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->isHorizon = false;
	}
	m_nHorizonEdges = 0;

	std::vector<const Face*> visitedFaces;
	visitedFaces.push_back(visibleFace);

	HalfEdge* const initialEdge = visibleFace->edge;
	HalfEdge* edge = initialEdge->next->twin;
	visibleFace->visited = true;

	int nFreedEdges = 0;

	while (edge != initialEdge)
	{
		Face* face = edge->face;

		HalfEdge* nextEdge = nullptr;

		if (!face->visited && (eye - edge->vert->m_MinkDif).Dot(face->normal) < 0.00000001)
		{
			m_horizonEdges[m_nHorizonEdges] = edge->twin;
			m_nHorizonEdges++;

			edge->twin->isHorizon = true;

			// Backcross the edge to return to the previous triangle
			nextEdge = edge->twin;
		}
		else
		{
			HalfEdge* next = edge->next;
			assert(m_faceValidities[next->face->index]);
			if (!next->twin->face->visited)
			{
				nextEdge = next->twin;
			}
			else
			{
				HalfEdge* prev = edge->prev;
				assert(m_faceValidities[prev->face->index]);
				if (!prev->twin->face->visited)
				{
					nextEdge = prev->twin;
				}
				else
				{
					nextEdge = next->twin;

					// We've come full circle, which means that we are working our way back to the root. Since this is depth first search,
					// this will be the last time we pass through this face, so put any non-horizon halfEdges of this face on the free list.
					for (HalfEdge* e : { edge, next, prev })
					{
						if (!e->isHorizon)
						{
							PushFreeEdge(e);
							nFreedEdges++;
						}
					}
				}
			}
		}
		if (!face->visited)
		{
			face->visited = true;
			visitedFaces.push_back(face);
		}
		edge = nextEdge;
	}

	for (HalfEdge* e : { initialEdge, initialEdge->next, initialEdge->prev })
	{
		if (!e->isHorizon)
		{
			PushFreeEdge(e);
			nFreedEdges++;
		}
	}

	for (const Face* visitedFace : visitedFaces)
	{
		visitedFace->visited = false;
	}

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		assert(m_horizonEdges[(i + 1) % m_nHorizonEdges]->twin->vert == m_horizonEdges[i]->vert);
	}

	std::cout << nFreedEdges << std::endl;
}

void EPAHull::PatchHorizon(const MinkowskiPoint* eye)
{
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		HalfEdge* curr = m_horizonEdges[i];

		// We need to use a new face because of our sorted heap mumbo jumbo

		Face* face = PopFreeFace();

		HalfEdge* prev = PopFreeEdge();
		HalfEdge* next = PopFreeEdge();

		face->edge = curr;
		const dVec3& B = curr->vert->m_MinkDif;
		const dVec3& A = curr->twin->vert->m_MinkDif;
		const dVec3& D = eye->m_MinkDif;
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

			const dVec3 D = pt0 - pt1;
			if (abs(D.Dot(n)) < 0.0001 || abs((D.Dot(n) - closestFace->distance) / closestFace->distance) < 0.01)
			{
				// The new point is so close to the face, that it doesnt warrant adding. So put the face back in the heap.
				PushFaceHeap(closestFace);
				return false;
			}

			CarveHorizon(D, closestFace);

			MinkowskiPoint* newVert = &m_verts[m_nVerts];
			newVert->m_MinkDif = pt0 - pt1;
			newVert->m_MinkSum = pt0 + pt1;
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

MinkowskiPoint EPAHull::Face::ComputeClosestPointToOrigin() const
{
	const MinkowskiPoint pivot = *edge->vert;

	double dSqrMin = 88888888.0;
	MinkowskiPoint closestPoint;

	HalfEdge* e = edge->next;
	do
	{
		GJKSimplex triangle;
		triangle.AddPoint(pivot);
		triangle.AddPoint(*e->vert);
		triangle.AddPoint(*e->next->vert);
		MinkowskiPoint x = triangle.ClosestPointToOrigin(GJKSimplex());
		double dSqr = x.m_MinkDif.Dot(x.m_MinkDif);

		if (dSqr < dSqrMin)
		{
			closestPoint = x;
			dSqrMin = dSqr;
		}
	} while (e != edge->prev);

	return closestPoint;
}

double EPAHull::ComputePenetration(dVec3& pt0, dVec3& pt1)
{
	for (int i = 0; i < EPAHULL_MAXITERS; ++i)
	{
		if (!Expand())
		{
			Face* closestFace = PopFaceHeap();
			PushFaceHeap(closestFace);

			MinkowskiPoint pt = closestFace->ComputeClosestPointToOrigin();
			pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
			pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);

			return sqrt((pt0 - pt1).Dot(pt0 - pt1));
		}
	}
	Face* closestFace = PopFaceHeap();
	PushFaceHeap(closestFace);

	MinkowskiPoint pt = closestFace->ComputeClosestPointToOrigin();
	pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
	pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);

	return sqrt((pt0 - pt1).Dot(pt0 - pt1));
}

int EPAHull::EulerCharacteristic() const
{
	int nF = 0;
	std::set<HalfEdge*> heSet;
	std::set<const MinkowskiPoint*> vSet;
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

			ABC[0] = fVec3(face->edge->vert->m_MinkDif);
			ABC[1] = fVec3(face->edge->next->vert->m_MinkDif);
			ABC[2] = fVec3(face->edge->next->next->vert->m_MinkDif);

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

		const fVec3 A(edge->vert->m_MinkDif);
		const fVec3 B(edge->twin->vert->m_MinkDif);

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