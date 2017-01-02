#include "stdafx.h"
#include "Geometry.h"
#include "MathUtils.h"
#include "Point.h"
#include "Ray.h"

#define MIN_SUPPORT_SQR 0.0001
#define GJK_TERMINATION_RATIO 0.01

Geometry::Geometry() : m_pos(0.0, 0.0, 0.0), m_rot(0.0, 0.0, 0.0, 1.0)
{
}


Geometry::~Geometry()
{
}

dVec3 Geometry::GetPosition() const
{
	return m_pos;
}
dQuat Geometry::GetRotation() const
{
	return m_rot;
}
BoundingBox Geometry::GetLocalOOBB() const
{
	return m_localOOBB;
}

void Geometry::SetPosition(const dVec3& pos)
{
	m_pos = pos;
}

void Geometry::SetRotation(const dQuat& rot)
{
	m_rot = rot;
}

dVec3 Geometry::Support(const dVec3& x, const dQuat& q, const dVec3& v) const
{
	return x + q.Transform(SupportLocal((-q).Transform(v)));
}

dVec3 Geometry::SupportLocal(const dVec3& v) const
{
	return dVec3(0.0, 0.0, 0.0);
}

bool Geometry::RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hit) const
{
	GJKSimplex simplex;
	hit = ray.GetOrigin();
	const dVec3 rayDir = ray.GetDirection();

	Point pt;

	dVec3 closest;
	dVec3 dummy;

	while (true)
	{
		double sep = Geometry::ComputeSeparation(&pt, hit, dQuat::Identity(), dummy, this, pos, rot, closest, simplex);

		const dVec3 hit2closest = closest - hit;

		if (sep < 0.001)
		{
			return true;
		}

		if (hit2closest.Dot(hit2closest) < 0.001*0.001)
		{
			return true;
		}
		else if (hit2closest.Dot(rayDir) / sqrt(hit2closest.Dot(hit2closest)*rayDir.Dot(rayDir)) < 0.001)
		{
			// if the closest point is perpendicular to or behind the ray direction, then the ray missed (or will miss)
			return false;
		}

		// march the ray forward, and shift the simplex for the next GJK pass
		const dVec3 dHit = rayDir.Scale(hit2closest.Dot(rayDir) / rayDir.Dot(rayDir));
		hit = hit + dHit;

		for (int i = 0; i < simplex.m_nPts; ++i)
		{
			simplex.m_pts[i].m_MinkDif = simplex.m_pts[i].m_MinkDif + dHit;
			simplex.m_pts[i].m_MinkSum = simplex.m_pts[i].m_MinkSum + dHit;
		}
	}
	return false;
}

double Geometry::ComputePenetration(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1,
	const GJKSimplex& simplex)
{
	// COMPLETE THE TETRAHEDRON IF THE SIMPLEX HAS FEWER THAN 4 POINTS

	GJKSimplex fullSimplex = simplex;
	switch (simplex.GetNumPoints())
	{
	case 1:
	{
		MinkowskiPoint singlePt = simplex.m_pts[0];
		pt0 = (singlePt.m_MinkSum + singlePt.m_MinkDif).Scale(0.5);
		pt1 = (singlePt.m_MinkSum - singlePt.m_MinkDif).Scale(0.5);
		return (pt0 - pt1).Dot(pt0 - pt1);
	}
	case 2:
	{
		const dVec3& A = simplex.m_pts[0].m_MinkDif;
		const dVec3& B = simplex.m_pts[1].m_MinkDif;
		const dVec3 AB = B - A;
		const dVec3 AB_AB = AB.Times(AB);
		dVec3 n;
		for (int i = 0; i < 3; ++i)
		{
			int j = (i + 1) % 3;
			int k = (i + 2) % 3;
			if (AB_AB[i] <= AB_AB[j] && AB_AB[i] <= AB_AB[k])
			{
				n[i] = 0.0;
				n[j] = AB[k];
				n[k] = -AB[j];

				MinkowskiPoint newSimplexPt;
				dVec3 p0 = geom0->Support(pos0, rot0, -n);
				dVec3 p1 = geom1->Support(pos1, rot1, n);
				const dVec3& C = p0 - p1;
				newSimplexPt.m_MinkDif = C;
				newSimplexPt.m_MinkSum = p0 + p1;
				fullSimplex.AddPoint(newSimplexPt);

				n = (B - A).Cross(C - A);
				p0 = geom0->Support(pos0, rot0, -n);
				p1 = geom1->Support(pos1, rot1, n);
				newSimplexPt.m_MinkDif = p0 - p1;
				newSimplexPt.m_MinkSum = p0 + p1;
				fullSimplex.AddPoint(newSimplexPt);

				break;
			}
		}

		break;
	}
	case 3:
	{
		const dVec3& A = simplex.m_pts[0].m_MinkDif;
		const dVec3& B = simplex.m_pts[1].m_MinkDif;
		const dVec3& C = simplex.m_pts[2].m_MinkDif;
		const dVec3 n = (B - A).Cross(C - A);
		MinkowskiPoint newSimplexPt;
		const dVec3 p0 = geom0->Support(pos0, rot0, -n);
		const dVec3 p1 = geom1->Support(pos1, rot1, n);
		newSimplexPt.m_MinkDif = p0 - p1;
		newSimplexPt.m_MinkSum = p0 + p1;
		fullSimplex.AddPoint(newSimplexPt);
		break;
	}
	}

	// HALF-EDGE DATA STRUCTURE

	struct Face;
	struct HalfEdge;

	struct HalfEdge
	{
		HalfEdge*		twin = nullptr;
		HalfEdge*		next = nullptr;
		Face*	 		face = nullptr;
		MinkowskiPoint*	vert = nullptr;
	};
	struct Face
	{
		double distance;
		dVec3 normal;

		HalfEdge* edge = nullptr;

		bool visited = false;

		bool valid = true;
	};

	bool (*CompareFace)(const Face*, const Face*) = [](const Face* face0, const Face* face1)
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
			return face0->edge > face1->edge;
		}
	};

	Face* heap[256];

	// INITIALIZE EPAHULL

	const int maxIter = 16;
	const int maxFaces = 128;
	const int maxVerts = 4 + maxIter;
	const int maxEdges = 256;

	Face faces[maxFaces];
	MinkowskiPoint verts[maxVerts];
	HalfEdge edges[maxEdges];
	verts[0] = fullSimplex.m_pts[0];
	verts[1] = fullSimplex.m_pts[1];
	verts[2] = fullSimplex.m_pts[2];
	verts[3] = fullSimplex.m_pts[3];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			edges[4 * i + j].twin = &edges[4 * j + i];
			edges[4 * i + j].vert = &verts[j];
		}
	}

	for (int f = 0; f < 4; ++f)
	{
		const int iA = (f + 0) % 4;
		const int iB = (f + 1) % 4;
		const int iC = (f + 2) % 4;
		const int iD = (f + 3) % 4;
		const dVec3& A = verts[iA].m_MinkDif;
		const dVec3& B = verts[iB].m_MinkDif;
		const dVec3& C = verts[iC].m_MinkDif;
		const dVec3& D = verts[iD].m_MinkDif;

		dVec3 n = (C - B).Cross(D - B);
		n = n.Scale(1.0 / sqrt(n.Dot(n)));

		heap[f] = &faces[f];

		if (n.Dot(B - A) < 0.0)
		{
			edges[4 * iC + iB].face = &faces[f];
			edges[4 * iD + iC].face = &faces[f];
			edges[4 * iB + iD].face = &faces[f];

			edges[4 * iC + iB].next = &edges[4 * iB + iD];
			edges[4 * iD + iC].next = &edges[4 * iC + iB];
			edges[4 * iB + iD].next = &edges[4 * iD + iC];

			faces[f].edge = &edges[4 * iC + iB];

			n = -n;
		}
		else
		{
			edges[4 * iB + iC].face = &faces[f];
			edges[4 * iC + iD].face = &faces[f];
			edges[4 * iD + iB].face = &faces[f];

			edges[4 * iB + iC].next = &edges[4 * iC + iD];
			edges[4 * iC + iD].next = &edges[4 * iD + iB];
			edges[4 * iD + iB].next = &edges[4 * iB + iC];

			faces[f].edge = &edges[4 * iB + iC];
		}

		faces[f].normal = n;
		faces[f].distance = B.Dot(n);
	}

	int nHeap = 4;
	int nFaces = 4;
	int nVerts = 4;
	int nEdges = 16;
	std::make_heap(heap, &heap[4], CompareFace);

	// BEGIN MODIFIED QUICKHULL ALGORITHM http://media.steampowered.com/apps/valve/2014/DirkGregorius_ImplementingQuickHull.pdf

	for (int nIter = 0; nIter < maxIter; ++nIter)
	{
		while (nHeap > 0)
		{
			std::pop_heap(heap, &heap[nHeap], CompareFace);
			nHeap--;
			Face* closestFace = heap[nHeap];
			if (closestFace->valid)
			{
				const dVec3& n = closestFace->normal;
				pt0 = geom0->Support(pos0, rot0, n);
				pt1 = geom1->Support(pos1, rot1, -n);

				const dVec3 D = pt0 - pt1;

				if (abs((D.Dot(n) - closestFace->distance) / closestFace->distance) < 0.01)
				{
					GJKSimplex closestTriangle;
					closestTriangle.AddPoint(*closestFace->edge->vert);
					closestTriangle.AddPoint(*closestFace->edge->next->vert);
					closestTriangle.AddPoint(*closestFace->edge->next->next->vert);
					MinkowskiPoint pt = closestTriangle.ClosestPointToOrigin(GJKSimplex());
					pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
					pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);
					return closestFace->distance;
				}

				verts[nVerts].m_MinkDif = D;
				verts[nVerts].m_MinkSum = pt0 + pt1;
				nVerts++;

				std::vector<HalfEdge*> horizon;
				std::vector<Face*> visitedFaces;
				visitedFaces.push_back(closestFace);

				HalfEdge* initialEdge = closestFace->edge;
				HalfEdge* edge = initialEdge->next->twin;
				closestFace->visited = true;

				while (edge != initialEdge)
				{
					Face* face = edge->face;

					if (!face->visited && (D - edge->vert->m_MinkDif).Dot(face->normal) < 0.0001)
					{
						horizon.push_back(edge->twin);
						// Backcross the edge to return to the previous triangle
						edge = edge->twin;
					}
					else
					{
						if (!edge->next->twin->face->visited)
						{
							edge = edge->next->twin;
						}
						else if (!edge->next->next->twin->face->visited)
						{
							edge = edge->next->next->twin;
						}
						else
						{
							edge = edge->next->twin;
						}
					}
					if (!face->visited)
					{
						face->visited = true;
						visitedFaces.push_back(face);
					}
				}

				for (Face* visitedFace : visitedFaces)
				{
					visitedFace->visited = false;
				}

				int nHorizon = horizon.size();
				for (int i = 0; i < nHorizon; ++i)
				{
					// We need to use a new face because of our sorted heap mumbo jumbo

					Face* face = &faces[nFaces];
					nFaces++;
					HalfEdge* next = &edges[nEdges];
					nEdges++;
					HalfEdge* nextNext = &edges[nEdges];
					nEdges++;

					face->edge = horizon[i];
					const dVec3 B = horizon[i]->vert->m_MinkDif;
					const dVec3 A = horizon[i]->twin->vert->m_MinkDif;
					dVec3 n = (A - D).Cross(B - D);
					n = n.Scale(1.0 / sqrt(n.Dot(n)));
					face->normal = n;
					face->distance = D.Dot(n);

					heap[nHeap] = face;
					nHeap++;
					std::push_heap(heap, &heap[nHeap], CompareFace);

					horizon[i]->face->valid = false;
					horizon[i]->face = face;
					next->face = face;
					nextNext->face = face;

					next->vert = &verts[nVerts - 1];
					nextNext->vert = horizon[i]->twin->vert;

					horizon[i]->next = next;
					next->next = nextNext;
					nextNext->next = horizon[i];
				}
				for (int i = 0; i < nHorizon; ++i)
				{
					horizon[i]->next->twin = horizon[(i + 1) % nHorizon]->next->next;
					horizon[i]->next->next->twin = horizon[(i - 1 + nHorizon) % nHorizon]->next;
				}
				
				break;
			}
		}
	}

	/////////////////////////////////////////////////////
	// FAILED TO CONVERGE, JUST RETURN THE BEST RESULT //
	/////////////////////////////////////////////////////

	while (nHeap > 0)
	{
		std::pop_heap(heap, &heap[nHeap], CompareFace);
		nHeap--;
		Face* closestFace = heap[nHeap];
		if (closestFace->valid)
		{
			GJKSimplex closestTriangle;
			closestTriangle.AddPoint(*closestFace->edge->vert);
			closestTriangle.AddPoint(*closestFace->edge->next->vert);
			closestTriangle.AddPoint(*closestFace->edge->next->next->vert);
			MinkowskiPoint pt = closestTriangle.ClosestPointToOrigin(GJKSimplex());
			pt0 = (pt.m_MinkSum + pt.m_MinkDif).Scale(0.5);
			pt1 = (pt.m_MinkSum - pt.m_MinkDif).Scale(0.5);
			return closestFace->distance;
		}
	}
	return 0.0;
}
double Geometry::ComputeSeparation(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1)
{
	GJKSimplex simplex;

	return Geometry::ComputeSeparation(
		geom0, pos0, rot0, pt0,
		geom1, pos1, rot1, pt1,
		simplex);
}
double Geometry::ComputeSeparation(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1,
	GJKSimplex& simplex)
{
	if (simplex.GetNumPoints() == 0)
	{
		dVec3 v(pos0 - pos1);
		pt0 = geom0->Support(pos0, rot0, -v);
		pt1 = geom1->Support(pos1, rot1, v);
		MinkowskiPoint newSimplexPt;
		newSimplexPt.m_MinkDif = pt0 - pt1;
		newSimplexPt.m_MinkSum = pt0 + pt1;
		simplex.AddPoint(newSimplexPt);
	}

	int nIter = 0;

	MinkowskiPoint closestSimplexPt;

	while (nIter < 16)
	{
		nIter++;
		// Get the closest point on the convex hull of the simplex, set it to the new support direction "v"
		// and discard any existing points on the simplex that are not needed to express "v"
		GJKSimplex closestFeature;
		closestSimplexPt = simplex.ClosestPointToOrigin(closestFeature);

		if (closestFeature.GetNumPoints() == 4)
		{
			return -ComputePenetration(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1, simplex);
		}
		else
		{
			const dVec3 v = closestSimplexPt.m_MinkDif;
			double vSqr = v.Dot(v);
			if (vSqr < MIN_SUPPORT_SQR)
			{
				return -ComputePenetration(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1, simplex);
//				pt0 = (closestSimplexPt.m_MinkSum + closestSimplexPt.m_MinkDif).Scale(0.5);
//				pt1 = (closestSimplexPt.m_MinkSum - closestSimplexPt.m_MinkDif).Scale(0.5);
//				return 0.0;
			}
			pt0 = geom0->Support(pos0, rot0, -v);
			pt1 = geom1->Support(pos1, rot1, v);
			MinkowskiPoint newSimplexPt;
			newSimplexPt.m_MinkDif = pt0 - pt1;
			newSimplexPt.m_MinkSum = pt0 + pt1;
			const double dSqr = newSimplexPt.m_MinkDif.Dot(newSimplexPt.m_MinkDif);

			if (dSqr < MIN_SUPPORT_SQR)
			{
				return 0.0;
			}
			else
			{
				const double vNorm = sqrt(vSqr);
				const dVec3 vHat = v.Scale(1.0 / vNorm);
				const double dCloser = fabs(newSimplexPt.m_MinkDif.Dot(vHat) - vNorm);
				if (dCloser < 0.001 || dCloser / vNorm < GJK_TERMINATION_RATIO)
				{
					pt0 = (closestSimplexPt.m_MinkSum + closestSimplexPt.m_MinkDif).Scale(0.5);
					pt1 = (closestSimplexPt.m_MinkSum - closestSimplexPt.m_MinkDif).Scale(0.5);
					return sqrt(dSqr);
				}
			}
			// Add the newly found support point to the simplex
			simplex = closestFeature;
			simplex.AddPoint(newSimplexPt);
		}
	}
	pt0 = (closestSimplexPt.m_MinkSum + closestSimplexPt.m_MinkDif).Scale(0.5);
	pt1 = (closestSimplexPt.m_MinkSum - closestSimplexPt.m_MinkDif).Scale(0.5);
	return sqrt((pt0 - pt1).Dot(pt0 - pt1));
}