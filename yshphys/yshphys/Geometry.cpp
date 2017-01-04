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

GJKSimplex CompleteSimplex(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1,
	const GJKSimplex& simplex)
{
	GJKSimplex fullSimplex = simplex;
	switch (simplex.GetNumPoints())
	{
//	case 1:
//	{
//		MinkowskiPoint singlePt = simplex.m_pts[0];
//		pt0 = (singlePt.m_MinkSum + singlePt.m_MinkDif).Scale(0.5);
//		pt1 = (singlePt.m_MinkSum - singlePt.m_MinkDif).Scale(0.5);
//		return (pt0 - pt1).Dot(pt0 - pt1);
//	}
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
	return fullSimplex;
}

double Geometry::ComputePenetration(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1,
	const GJKSimplex& simplex)
{
	return 0.0;
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
		MinkowskiPoint*	vert = nullptr;
		HalfEdge*		next = nullptr;
		HalfEdge*		prev = nullptr;
		HalfEdge*		twin = nullptr;
		Face*	 		face = nullptr;
	};
	struct Face
	{
		double distance;
		dVec3 normal;

		HalfEdge* edge = nullptr;

		bool visited = false;

		bool* valid;
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
			return face0 > face1;
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

	// ugh this is so ugly, but we cannot manipulate the sorted heap directly, hence the indirection
	bool faceValidities[maxFaces];
	std::memset(faceValidities, 1, sizeof(faceValidities));
	for (int f = 0; f < maxFaces; ++f)
	{
		faces[f].valid = &faceValidities[f];
	}

	verts[0] = fullSimplex.m_pts[0];
	verts[1] = fullSimplex.m_pts[1];
	verts[2] = fullSimplex.m_pts[2];
	verts[3] = fullSimplex.m_pts[3];

	dVec3 box(0.0, 0.0, 0.0);

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

		HalfEdge * e[3];

		if (n.Dot(B - A) < 0.0)
		{
			e[0] = &edges[4 * iC + iB];
			e[1] = &edges[4 * iB + iD];
			e[2] = &edges[4 * iD + iC];

			n = -n;
		}
		else
		{
			e[0] = &edges[4 * iB + iC];
			e[1] = &edges[4 * iC + iD];
			e[2] = &edges[4 * iD + iB];
		}

		for (int i = 0; i < 3; ++i)
		{
			e[i]->next = e[(i + 1) % 3];
			e[i]->prev = e[(i + 2) % 3];
			e[i]->face = &faces[f];
		}
		faces[f].edge = e[0];

		const double d = B.Dot(n);

		faces[f].normal = n;
		faces[f].distance = d;

		const dVec3 v = n.Scale(d);

		box.x = std::max(box.x, abs(v.x));
		box.y = std::max(box.y, abs(v.y));
		box.z = std::max(box.z, abs(v.z));
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
			if (*closestFace->valid)
			{
				const dVec3& n = closestFace->normal;
				pt0 = geom0->Support(pos0, rot0, n);
				pt1 = geom1->Support(pos1, rot1, -n);

				const dVec3 D = pt0 - pt1;

				if (abs(D.Dot(n)) < 0.0001 || abs((D.Dot(n) - closestFace->distance) / closestFace->distance) < 0.01)
				{
					// TODO: We need to check more than one triangle since we can now merge triangles into polygons
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
					
					HalfEdge* nextEdge = nullptr;

					if (!face->visited && (D - edge->vert->m_MinkDif).Dot(face->normal) < 0.000001)
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
							assert(*nextEdge->face->valid);
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

				for (Face* visitedFace : visitedFaces)
				{
					visitedFace->visited = false;
				}

				const int nHorizon = (int)horizon.size();
				for (HalfEdge* curr : horizon)
				{
					// We need to use a new face because of our sorted heap mumbo jumbo

					Face* face = &faces[nFaces];
					nFaces++;
					HalfEdge* prev = &edges[nEdges];
					nEdges++;
					HalfEdge* next = &edges[nEdges];
					nEdges++;

					face->edge = curr;
					const dVec3 B = curr->vert->m_MinkDif;
					const dVec3 A = curr->twin->vert->m_MinkDif;
					dVec3 n = (A - D).Cross(B - D);
					n = n.Scale(1.0 / sqrt(n.Dot(n)));
					const double d = D.Dot(n);
					face->normal = n;
					face->distance = d;

					const dVec3 v = n.Scale(d);

					box.x = std::max(box.x, abs(v.x));
					box.y = std::max(box.y, abs(v.y));
					box.z = std::max(box.z, abs(v.z));

					heap[nHeap] = face;
					nHeap++;
					std::push_heap(heap, &heap[nHeap], CompareFace);

					*curr->face->valid = false;

					prev->face = face;
					curr->face = face;
					next->face = face;

					prev->vert = curr->twin->vert;
					next->vert = &verts[nVerts - 1];

					prev->next = curr;
					curr->next = next;
					next->next = prev;

					prev->prev = next;
					curr->prev = prev;
					next->prev = curr;
				}
				for (int i = 0; i < nHorizon; ++i)
				{
					auto EdgeIsConvex = [&](HalfEdge* edge0)
					{
						HalfEdge* edge1 = edge0->twin;

						Face* face0 = edge0->face;
						Face* face1 = edge1->face;

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

						const dVec3 centroid0 = Centroid(edge0);
						const dVec3 centroid1 = Centroid(edge1);

						const double eps = 3.0 * (box.x + box.y + box.z) * DBL_EPSILON;

						return face0->normal.Dot(centroid0 - centroid1) > eps
							&& face1->normal.Dot(centroid1 - centroid0) > eps;
					};

					auto MergeFacesAlongEdge = [&](HalfEdge* edge0)
					{
						HalfEdge* edge1 = edge0->twin;
						*edge1->face->valid = false;

						Face* face = edge0->face;
						face->edge = edge0->next;

						HalfEdge* e = edge1;
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
					};

					horizon[i]->next->twin = horizon[(i + 1) % nHorizon]->prev;
					horizon[i]->prev->twin = horizon[(i - 1 + nHorizon) % nHorizon]->next;

					if (!EdgeIsConvex(horizon[i]))
					{
						MergeFacesAlongEdge(horizon[i]);
					}
				}

#if 1 
				int nF = 0;
				std::set<HalfEdge*> heSet;
				std::set<MinkowskiPoint*> vSet;
				for (int i = 0; i < nHeap; ++i)
				{
					if (*heap[i]->valid)
					{
						nF++;
						HalfEdge* e = heap[i]->edge;
						do
						{
							heSet.insert(e);
							e = e->next ;
						} while (e != heap[i]->edge);
					}
				}

				for (HalfEdge* he : heSet)
				{
					vSet.insert(he->vert);
				}

				int nHE = heSet.size();
				assert(nHE % 2 == 0);
				int nE = nHE / 2;

				int nV = vSet.size();
//				assert(nV - nE + nF == 2);
#endif
				
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
		if (*closestFace->valid)
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
				if (simplex.GetNumPoints() == 1)
				{
					MinkowskiPoint singlePt = simplex.m_pts[0];
					pt0 = (singlePt.m_MinkSum + singlePt.m_MinkDif).Scale(0.5);
					pt1 = (singlePt.m_MinkSum - singlePt.m_MinkDif).Scale(0.5);
					return 0.0;
				}
				else
				{
					simplex = CompleteSimplex(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1, simplex);
					return 0.0;
				}
				return -ComputePenetration(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1, simplex);
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