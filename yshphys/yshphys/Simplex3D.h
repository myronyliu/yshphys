#pragma once
#include "YshMath.h"
#include "Vec3.h"

struct MinkowskiPoint 
{
	dVec3 m_MinkDif;
	dVec3 m_MinkSum;
};

struct GJKSimplex
{
	GJKSimplex() : m_nPts(0) {}

	MinkowskiPoint ClosestPointToOrigin(GJKSimplex& closestFeature) const;
	void AddPoint(const MinkowskiPoint& pt) { m_pts[m_nPts] = pt; m_nPts++; }
	int GetNumPoints() const { return m_nPts; }

	MinkowskiPoint m_pts[4];
	int m_nPts;

private:
	inline MinkowskiPoint ClosestPointToOrigin2(int iA, int iB, GJKSimplex& closestFeature) const;
	inline MinkowskiPoint ClosestPointToOrigin3(int iA, int iB, int iC, GJKSimplex& closestFeature) const;
	inline MinkowskiPoint ClosestPointToOrigin4(int iA, int iB, int iC, int iD, GJKSimplex& closestFeature) const;
};

struct EPAHull
{

};