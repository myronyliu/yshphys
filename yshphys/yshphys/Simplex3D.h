#pragma once
#include "YshMath.h"
#include "Vec3.h"

struct GJKSimplex
{
	GJKSimplex() : m_nPts(0) {}

	struct SimplexPt
	{
		dVec3 m_MinkDif;
		dVec3 m_MinkSum;
	};

	SimplexPt ClosestPointToOrigin(GJKSimplex& closestFeature) const;
	void AddPoint(const SimplexPt& pt) { m_pts[m_nPts] = pt; m_nPts++; }
	int GetNumPoints() const { return m_nPts; }

	SimplexPt m_pts[4];
	int m_nPts;

private:
	inline SimplexPt ClosestPointToOrigin2(int iA, int iB, GJKSimplex& closestFeature) const;
	inline SimplexPt ClosestPointToOrigin3(int iA, int iB, int iC, GJKSimplex& closestFeature) const;
	inline SimplexPt ClosestPointToOrigin4(int iA, int iB, int iC, int iD, GJKSimplex& closestFeature) const;


};