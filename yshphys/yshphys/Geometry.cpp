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


double Geometry::ComputeSeparation(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom) const
{
	return 0.0;
//	Vec3 simplex[4];

//	// First pass is special. Don't check for termination since v is just a guess.
//	Vec3 v(m_pos - geom->m_pos);
//	ptSelf = Support(-v);
//	ptGeom = geom->Support(v);
//	simplex[0] = ptSelf - ptGeom;

//	// POINT

//	v = simplex[0];
//	double vSqr = v.Dot(v);
//	if (vSqr < MIN_SUPPORT_SQR)
//	{
//		return sqrt(simplex[0].Dot(simplex[0]));
//	}
//	ptSelf = Support(-v);
//	ptGeom = geom->Support(v);
//	simplex[1] = ptSelf - ptGeom;
//	if (fabs(simplex[1].Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
//	{
//		return sqrt(simplex[1].Dot(simplex[1]));
//	}

//	// LINE
//	v = MathUtils::ClosestPointOnLineToPoint(simplex[0], simplex[1], Vec3(0.0, 0.0, 0.0));
//	vSqr = v.Dot(v);
//	if (vSqr < MIN_SUPPORT_SQR)
//	{
//		return sqrt(simplex[1].Dot(simplex[1]));
//	}
//	ptSelf = Support(-v);
//	ptGeom = geom->Support(v);
//	simplex[2] = ptSelf - ptGeom;
//	if (fabs(simplex[2].Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
//	{
//		return sqrt(simplex[2].Dot(simplex[2]));
//	}

//	// TRIANGLE
//	v = MathUtils::ClosestPointOnTriangleToPoint(simplex[0], simplex[1], simplex[2], Vec3(0.0, 0.0, 0.0));
//	vSqr = v.Dot(v);
//	if (vSqr < MIN_SUPPORT_SQR)
//	{
//		return sqrt(simplex[2].Dot(simplex[2]));
//	}
//	ptSelf = Support(-v);
//	ptGeom = geom->Support(v);
//	simplex[3] = ptSelf - ptGeom;
//	if (fabs(simplex[3].Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
//	{
//		return sqrt(simplex[3].Dot(simplex[3]));
//	}

//	unsigned int nIter = 0;
//	int iNewPt = 3;

//	while (nIter < 256)
//	{
//		// TETRAHEDRON
//		v = MathUtils::ClosestPointOnTetrahedronToPoint(simplex[0], simplex[1], simplex[2], simplex[3], Vec3(0.0, 0.0, 0.0));
//		vSqr = v.Dot(v);
//		if (vSqr < MIN_SUPPORT_SQR)
//		{
//			return sqrt(simplex[iNewPt].Dot(simplex[iNewPt]));
//		}
//		ptSelf = Support(-v);
//		ptGeom = geom->Support(v);
//		Vec3 newPt = ptSelf - ptGeom; // support on the minkowski difference "this - geom"
//		iNewPt = (iNewPt + 1) % 4;
//		simplex[iNewPt] = ptSelf - ptGeom; 
//		if (fabs(simplex[iNewPt].Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
//		{
//			return sqrt(simplex[iNewPt].Dot(simplex[iNewPt]));
//		}
//	}
//	return sqrt(simplex[iNewPt].Dot(simplex[iNewPt]));
}