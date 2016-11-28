#include "stdafx.h"
#include "Geometry.h"
#include "MathUtils.h"

#define MIN_SUPPORT_SQR 0.0001
#define GJK_TERMINATION_RATIO 0.001

Geometry::Geometry()
{
}


Geometry::~Geometry()
{
}

Vec3 Geometry::GetPosition() const
{
	return m_pos;
}
Quat Geometry::GetRotation() const
{
	return m_rot;
}

Vec3 Geometry::Support(const Vec3& v) const
{
	return Vec3(0.0, 0.0, 0.0);
}

double Geometry::ComputePenetration(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom, Simplex3D tetrahedron) const
{
	return -1.0;
}

double Geometry::ComputeSeparation(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom) const
{
	Simplex3D simplex;

	// First pass is special. Don't check for termination since v is just a guess.
	Vec3 v(m_pos - geom->m_pos);
	ptSelf = Support(-v);
	ptGeom = geom->Support(v);
	Vec3 ptSimplex(ptSelf - ptGeom);
	simplex.SetVertices(1, &ptSimplex);

	int nIter = 0;

	while (nIter < 16)
	{
		// Get the closest point on the convex hull of the simplex, set it to the new support direction "v"
		// and discard any existing points on the simplex that are not needed to express "v"
		Simplex3D newSimplex;
		v = simplex.ClosestPoint(Vec3(0.0, 0.0, 0.0), newSimplex);

		if (newSimplex.GetNumVertices() == 4)
		{
			return ComputePenetration(geom, ptSelf, ptGeom, newSimplex);
		}

		double vSqr = v.Dot(v);
		if (vSqr < MIN_SUPPORT_SQR)
		{
			return 0.0;
		}
		ptSelf = Support(-v);
		ptGeom = geom->Support(v);
		ptSimplex = ptSelf - ptGeom;
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