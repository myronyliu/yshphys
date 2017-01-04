#pragma once
#include "YshMath.h"
#include "Vec3.h"

template <typename T>
struct MinkowskiPoint_t
{
	MinkowskiPoint_t() {}

	template <typename S>
	MinkowskiPoint_t(const MinkowskiPoint_t<S>& pt) : m_MinkDif(pt.m_MinkDif), m_MinkSum(pt.m_MinkSum) {}

	Vec3_t<T> m_MinkDif;
	Vec3_t<T> m_MinkSum;
};
template struct MinkowskiPoint_t<float>;
template struct MinkowskiPoint_t<double>;
typedef MinkowskiPoint_t<float> fMinkowskiPoint;
typedef MinkowskiPoint_t<double> dMinkowskiPoint;
template MinkowskiPoint_t<float>::MinkowskiPoint_t(const MinkowskiPoint_t<double>&);
template MinkowskiPoint_t<double>::MinkowskiPoint_t(const MinkowskiPoint_t<float>&);

struct GJKSimplex
{
	GJKSimplex() : m_nPts(0) {}

	dMinkowskiPoint ClosestPointToOrigin(GJKSimplex& closestFeature) const;
	void AddPoint(const dMinkowskiPoint& pt) { m_pts[m_nPts] = pt; m_nPts++; }
	int GetNumPoints() const { return m_nPts; }

	dMinkowskiPoint m_pts[4];
	int m_nPts;

private:
	inline dMinkowskiPoint ClosestPointToOrigin2(int iA, int iB, GJKSimplex& closestFeature) const;
	inline dMinkowskiPoint ClosestPointToOrigin3(int iA, int iB, int iC, GJKSimplex& closestFeature) const;
	inline dMinkowskiPoint ClosestPointToOrigin4(int iA, int iB, int iC, int iD, GJKSimplex& closestFeature) const;
};

