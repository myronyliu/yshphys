#include "stdafx.h"
#include "Geometry.h"
#include "MathUtils.h"
#include "Point.h"
#include "Ray.h"

#define MIN_SUPPORT_SQR 0.0001
#define GJK_TERMINATION_RATIO 0.0001
#define DEGENERATE_SIMPLEX_RATIO 0.01

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
//	struct TriangleDistance
//	{
//		bool operator < (const TriangleDistance& td)
//		{
//			return distance > td.distance;
//		};

//		unsigned int indices[3];
//		double distance;
//	};

//	TriangleDistance triangles[256];
//	dVec3 vertices[256];
//	unsigned int nTriangles = 0;
//	unsigned int nVertices = 0;
//	std::make_heap(&triangles[0], &triangles[nTriangles]);

//	auto PushTriangle = [&](unsigned int A, unsigned int B, unsigned int C, double distance)
//	{
//		TriangleDistance td;
//		td.indices[0] = A;
//		td.indices[1] = B;
//		td.indices[2] = C;
//		td.distance = distance;
//		triangles[nTriangles] = td;
//		nTriangles++;
//		std::push_heap(&triangles[0], &triangles[nTriangles]);
//	};
//	auto PopClosestTriangle = [&](unsigned int& A, unsigned int& B, unsigned int& C, double& distance)
//	{
//		std::pop_heap(&triangles[0], &triangles[nTriangles]);
//		nTriangles--;
//		unsigned int* indices = triangles[nTriangles].indices;
//		A = indices[0];
//		B = indices[1];
//		C = indices[2];
//		distance = triangles[nTriangles].distance;
//	};
//	auto PushVertex = [&](const dVec3& vertex)
//	{
//		vertices[nVertices] = vertex;
//		nVertices++;
//		return nVertices - 1;
//	};

//	dVec3 tetraVerts[4];
//	if (tetrahedron.GetVertices(tetraVerts) == 4)
//	{
//		for (int i = 0; i < 4; ++i)
//		{
//			PushVertex(tetraVerts[i]);
//			const int iA = (i + 0) % 4;
//			const int iB = (i + 1) % 4;
//			const int iC = (i + 2) % 4;
//			const int iD = (i + 3) % 4;
//			const dVec3& a = tetraVerts[iA];
//			const dVec3& b = tetraVerts[iB];
//			const dVec3& c = tetraVerts[iC];
//			const dVec3& d = tetraVerts[iD];

//			dVec3 n = (c - b).Cross(d - b);
//			n = n.Scale(1.0 / sqrt(n.Dot(n)));

//			if (n.Dot(b - a) < 0.0)
//			{
//				PushTriangle(iD, iC, iB, -b.Dot(n));
//			}
//			else
//			{
//				PushTriangle(iB, iC, iD, b.Dot(n));
//			}
//		}

//		int nIter = 0;

//		while (nIter < 16)
//		{
//			nIter++;

//			unsigned int iA, iB, iC;
//			double dFace;
//			PopClosestTriangle(iA, iB, iC, dFace);

//			const dVec3 a = vertices[iA];
//			const dVec3 b = vertices[iB];
//			const dVec3 c = vertices[iC];

//			dVec3 n = (b - a).Cross(c - a);
//			n = n.Scale(1.0 / sqrt(n.Dot(n)));

//			pt0 = geom0->Support(pos0, rot0, n);
//			pt1 = geom1->Support(pos1, rot1, -n);
//			const dVec3 d = pt0 - pt1;

//			const unsigned int iD = PushVertex(d);

//			n = (a - d).Cross(b - d);
//			n = n.Scale(1.0 / sqrt(n.Dot(n)));
//			PushTriangle(iA, iB, iD, d.Dot(n));
////			assert(d.Dot(n) >= 0.0);

//			n = (b - d).Cross(c - d);
//			n = n.Scale(1.0 / sqrt(n.Dot(n)));
//			PushTriangle(iB, iC, iD, d.Dot(n));
////			assert(d.Dot(n) >= 0.0);

//			n = (c - d).Cross(a - d);
//			n = n.Scale(1.0 / sqrt(n.Dot(n)));
//			PushTriangle(iC, iA, iD, d.Dot(n));
////			assert(d.Dot(n) >= 0.0);

//			if (abs((d.Dot(n) - dFace) / dFace) < GJK_TERMINATION_RATIO)
//			{
//				const dVec3 terminatingSimplex[3] = { a,b,c };
//				tetrahedron.SetVertices(3, terminatingSimplex);
//				return dFace;
//			}
//		}
//	}
////	tetrahedron.SetVertices(4, simplex);
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
		GJKSimplex::SimplexPt newSimplexPt;
		newSimplexPt.m_MinkDif = pt0 - pt1;
		newSimplexPt.m_MinkSum = pt0 + pt1;
		simplex.AddPoint(newSimplexPt);
	}

	int nIter = 0;

	while (nIter < 16)
	{
		nIter++;
		// Get the closest point on the convex hull of the simplex, set it to the new support direction "v"
		// and discard any existing points on the simplex that are not needed to express "v"
		GJKSimplex closestFeature;
		GJKSimplex::SimplexPt closestSimplexPt = simplex.ClosestPointToOrigin(closestFeature);

		if (closestFeature.GetNumPoints() == 4)
		{
			return ComputePenetration(geom0, pos0, rot0, pt0, geom1, pos1, rot1, pt1, simplex);
		}
		else
		{
			const dVec3 v = closestSimplexPt.m_MinkDif;
			double vSqr = v.Dot(v);
			if (vSqr < MIN_SUPPORT_SQR)
			{
				pt0 = (closestSimplexPt.m_MinkSum + closestSimplexPt.m_MinkDif).Scale(0.5);
				pt1 = (closestSimplexPt.m_MinkSum - closestSimplexPt.m_MinkDif).Scale(0.5);
				return 0.0;
			}
			pt0 = geom0->Support(pos0, rot0, -v);
			pt1 = geom1->Support(pos1, rot1, v);
			GJKSimplex::SimplexPt newSimplexPt;
			newSimplexPt.m_MinkDif = pt0 - pt1;
			newSimplexPt.m_MinkSum = pt0 + pt1;
			const double dSqr((pt0 - pt1).Dot(pt0 - pt1));

			if (dSqr < MIN_SUPPORT_SQR)
			{
				return 0.0;
			}
			else if (fabs(newSimplexPt.m_MinkDif.Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
			{
				pt0 = (closestSimplexPt.m_MinkSum + closestSimplexPt.m_MinkDif).Scale(0.5);
				pt1 = (closestSimplexPt.m_MinkSum - closestSimplexPt.m_MinkDif).Scale(0.5);
				return sqrt(dSqr);
			}
			// Add the newly found support point to the simplex
			simplex = closestFeature;
			simplex.AddPoint(newSimplexPt);
		}
	}
	assert(0);
	return 88888888.0;
}