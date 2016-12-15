#include "stdafx.h"
#include "Geometry.h"
#include "MathUtils.h"

#define MIN_SUPPORT_SQR 0.0001
#define GJK_TERMINATION_RATIO 0.001

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

double Geometry::ComputePenetration(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1,
	const Simplex3D& tetrahedron)
{
	dVec3 simplex[4];
	if (tetrahedron.GetVertices(simplex) == 4)
	{
		int nIter = 0;

		while (nIter < 16)
		{
			nIter++;

			int iFace = 0;
			dVec3 nFace(0.0, 0.0, 0.0);
			double dFace = 88888888.0;

			for (int i = 0; i < 4; ++i)
			{
				const dVec3& a = simplex[(i + 0) % 3];
				const dVec3& b = simplex[(i + 1) % 3];
				const dVec3& c = simplex[(i + 2) % 3];
				const dVec3& d = simplex[(i + 3) % 3];

				dVec3 n = (c - b).Cross(d - b);
				n = n.Scale(MathUtils::sgn(n.Dot(b - a)));
				n.Scale(1.0 / sqrt(n.Dot(n)));

				double dist = b.Dot(n);

				if (dist < dFace)
				{
					iFace = i;
					nFace = n;
					dFace = dist;
				}
			}

			if (dFace < MIN_SUPPORT_SQR)
			{
				return 0.0;
			}

			pt0 = geom0->Support(pos0, rot0, nFace);
			pt1 = geom1->Support(pos1, rot1, -nFace);
			simplex[iFace] = pt0 - pt1;
			if (abs((simplex[iFace].Dot(nFace) - dFace) / dFace) < GJK_TERMINATION_RATIO)
			{
				return sqrt(simplex[iFace].Dot(simplex[iFace]));
			}
		}
	}
	return 0.0;
}

double Geometry::ComputeSeparation(
	const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
	const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1)
{
	Simplex3D simplex;

	// First pass is special. Don't check for termination since v is just a guess.
	dVec3 v(pos0 - pos1);
	pt0 = geom0->Support(pos0, rot0, -v);
	pt1 = geom1->Support(pos1, rot1, v);
	dVec3 ptSimplex(pt0 - pt1);
	simplex.SetVertices(1, &ptSimplex);

	int nIter = 0;

	while (nIter < 16)
	{
		nIter++;
		// Get the closest point on the convex hull of the simplex, set it to the new support direction "v"
		// and discard any existing points on the simplex that are not needed to express "v"
		Simplex3D newSimplex;
		v = simplex.ClosestPoint(dVec3(0.0, 0.0, 0.0), newSimplex);

		if (newSimplex.GetNumVertices() == 4)
		{
			return ComputePenetration(
				geom0, pos0, rot0, pt0,
				geom1, pos1, rot1, pt1,
				newSimplex);
		}

		double vSqr = v.Dot(v);
		if (vSqr < MIN_SUPPORT_SQR)
		{
			return 0.0;
		}
		pt0 = geom0->Support(pos0, rot0, -v);
		pt1 = geom1->Support(pos1, rot1, v);
		ptSimplex = pt0 - pt1;
		const double dSqr(ptSimplex.Dot(ptSimplex));

		if (dSqr < MIN_SUPPORT_SQR)
		{
			return 0.0;
		}
		else if (fabs(ptSimplex.Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
		{
			return sqrt(dSqr);
		}

		// Add the newly found support point to the simplex
		simplex = newSimplex;
		simplex.AddVertex(ptSimplex);
	}
	return false;
}