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

bool EPAHull::HalfEdge::IsConvex(double faceThickness) const
{
	Face* face0 = face;
	Face* face1 = twin->face;

	auto Centroid = [](const HalfEdge* edge)
	{
		const dVec3 n = edge->face->normal;

		const HalfEdge* e = edge;

		double a = 0;
		dVec3 c(0.0, 0.0, 0.0);

		do
		{
			const dVec3& u = e->vert->m_MinkDif;
			const dVec3& v = e->next->vert->m_MinkDif;

			const double da = u.Cross(v).Dot(n);

			a += da;
			c = c + (u + v).Scale(da);

			e = e->next;
		} while (e != edge);

		return c.Scale(1.0 / 3.0 / a);
	};

	const dVec3 centroid0 = Centroid(this);
	const dVec3 centroid1 = Centroid(twin);

	return face0->normal.Dot(centroid0 - centroid1) > faceThickness 
		&& face1->normal.Dot(centroid1 - centroid0) > faceThickness;
}

void EPAHull::MergeFacesAlongEdge(HalfEdge* edge0)
{
	HalfEdge* edge1 = edge0->twin;
	m_faceValidities[edge1->face->index] = false;

	Face* face = edge0->face;
	face->edge = edge0->next;

	HalfEdge* e = edge1->next;
	do
	{
		e->face = face;
		e = e->next;
	} while (e != edge1);

	edge1->prev->next = edge0->next;
	edge1->next->prev = edge0->prev;
	edge0->prev->next = edge1->next;
	edge0->next->prev = edge1->prev;

	// Compute best fit normal using Newell's Method 

	e = e->prev;
	const HalfEdge* const eInit = e;

	dVec3 n(0.0, 0.0, 0.0);

	do
	{
		const dVec3& u = e->vert->m_MinkDif;
		const dVec3& v = e->next->vert->m_MinkDif;

		n.x += (u.y - v.y)*(u.z + v.z);
		n.y += (u.z - v.z)*(u.x + v.x);
		n.z += (u.x - v.x)*(u.y + v.y);

		e = e->next;
	} while (e != eInit);

	face->normal = n.Scale(1.0 / sqrt(n.Dot(n)));
}

EPAHull::EPAHull(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1,
		const GJKSimplex& tetrahedron) :
	m_box(0.0,0.0,0.0)
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

		m_box.x = std::max(m_box.x, abs(A.x));
		m_box.y = std::max(m_box.y, abs(A.y));
		m_box.z = std::max(m_box.z, abs(A.z));

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

	m_nFacesInHeap = 4;
	m_nFaces = 4;
	m_nVerts = 4;
	m_nEdges = 12;
	std::make_heap(m_faceHeap, &m_faceHeap[4], CompareFacesByDistance);
}

std::vector<EPAHull::HalfEdge*> EPAHull::ComputeHorizon(const dVec3& eye, const Face* visibleFace) const
{
	std::vector<HalfEdge*> horizon;
	std::vector<const Face*> visitedFaces;
	visitedFaces.push_back(visibleFace);

	const HalfEdge* initialEdge = visibleFace->edge;
	HalfEdge* edge = initialEdge->next->twin;
	visibleFace->visited = true;

	while (edge != initialEdge)
	{
		Face* face = edge->face;

		HalfEdge* nextEdge = nullptr;

		if (!face->visited && (eye - edge->vert->m_MinkDif).Dot(face->normal) < 0.000001)
		{
			horizon.push_back(edge->twin);
			// Backcross the edge to return to the previous triangle
			nextEdge = edge->twin;
		}
		else
		{
			nextEdge = edge->next->twin;

			while (true)
			{
				assert(m_faceValidities[nextEdge->face->index]);
				if (!nextEdge->face->visited)
				{
					break;
				}
				else if (nextEdge == edge->twin)
				{
					nextEdge = edge->next->twin;
					break;
				}
				nextEdge = nextEdge->twin->next->twin;
			}
		}
		if (!face->visited)
		{
			face->visited = true;
			visitedFaces.push_back(face);
		}
		edge = nextEdge;
	}

	for (const Face* visitedFace : visitedFaces)
	{
		visitedFace->visited = false;
	}

	const int nHorizon = (int)horizon.size();
	for (int i = 0; i < nHorizon; ++i)
	{
		assert(horizon[(i + 1) % nHorizon]->twin->vert == horizon[i]->vert);
	}

	return horizon;
}

void EPAHull::PatchHorizon(std::vector<HalfEdge*> horizon, const MinkowskiPoint* eye)
{
	for (HalfEdge* curr : horizon)
	{
		// We need to use a new face because of our sorted heap mumbo jumbo

		Face* face = &m_faces[m_nFaces];
		m_nFaces++;
		HalfEdge* prev = &m_edges[m_nEdges];
		m_nEdges++;
		HalfEdge* next = &m_edges[m_nEdges];
		m_nEdges++;

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

	const int nHorizon = (int)horizon.size();
	for (int i = 0; i < nHorizon; ++i)
	{
		horizon[i]->next->twin = horizon[(i + 1) % nHorizon]->prev;
		horizon[i]->prev->twin = horizon[(i - 1 + nHorizon) % nHorizon]->next;
	}
}

void EPAHull::EnforceHorizonConvexity(std::vector<EPAHull::HalfEdge*> horizon)
{
	const double eps = 3.0 * (m_box.x + m_box.y + m_box.z) * DBL_EPSILON;
	for (HalfEdge* edge : horizon)
	{
		if (!edge->IsConvex(eps))
		{
			MergeFacesAlongEdge(edge);
		}
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

			m_box.x = std::max(m_box.x, abs(D.x));
			m_box.y = std::max(m_box.y, abs(D.y));
			m_box.z = std::max(m_box.z, abs(D.z));

			std::vector<HalfEdge*> horizon = ComputeHorizon(D, closestFace);

			MinkowskiPoint* newVert = &m_verts[m_nVerts];
			newVert->m_MinkDif = pt0 - pt1;
			newVert->m_MinkSum = pt0 + pt1;
			m_nVerts++;

			PatchHorizon(horizon, newVert);
			EnforceHorizonConvexity(horizon);

			const int eulerCharacteristic = EulerCharacteristic();
			assert(eulerCharacteristic == 2);

			return true;
		}
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
	int nE = nHE / 2;

	const int nV = (int)vSet.size();
	return nV - nE + nF;
}

void EPAHull::DebugDraw(DebugRenderer* renderer) const
{
	fVec3 color(1.0f, 1.0f, 1.0f);

	for (int i = 0; i < m_nFacesInHeap; ++i)
	{
		if (m_faceValidities[m_faceHeap[i]->index])
		{
			fVec3 x[EPAHULL_MAXVERTS];
			int n = 0;

			HalfEdge* e0 = m_faceHeap[i]->edge;
			HalfEdge* e = e0;
			do
			{
				x[n++] = e->vert->m_MinkDif;
				e = e->next;
			} while (e != e0);

			renderer->DrawPolygon(x, n, color, false);
			color = color.Scale(0.8f);
		}
	}
}