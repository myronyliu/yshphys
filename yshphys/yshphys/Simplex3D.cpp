#include "stdafx.h"
#include "Simplex3D.h"

dMinkowskiPoint GJKSimplex::ClosestPointToOrigin2(int iA, int iB, GJKSimplex& closestFeature) const
{
	const dVec3& A = m_pts[iA].m_MinkDif;
	const dVec3& B = m_pts[iB].m_MinkDif;
	const dVec3 AO = -A;
	const dVec3 AB = B - A;

	const double t = AO.Dot(AB) / AB.Dot(AB);
	
	if (t <= 0.0)
	{
		closestFeature.m_nPts = 1;
		closestFeature.m_pts[0] = m_pts[iA];
		return m_pts[iA];
	}
	else if (t >= 1.0)
	{
		closestFeature.m_nPts = 1;
		closestFeature.m_pts[0] = m_pts[iB];
		return m_pts[iB];
	}
	else
	{
		closestFeature.m_nPts = 2;
		closestFeature.m_pts[0] = m_pts[iA];
		closestFeature.m_pts[1] = m_pts[iB];

		dMinkowskiPoint pq;
		pq.m_MinkDif = A + AB.Scale(t);
		pq.m_MinkSum = m_pts[iA].m_MinkSum + (m_pts[iB].m_MinkSum - m_pts[iA].m_MinkSum).Scale(t);
		return pq;
	}
}
dMinkowskiPoint GJKSimplex::ClosestPointToOrigin3(int iA, int iB, int iC, GJKSimplex& closestFeature) const
{
	const dVec3& A = m_pts[iA].m_MinkDif;
	const dVec3& B = m_pts[iB].m_MinkDif;
	const dVec3& C = m_pts[iC].m_MinkDif;
	const dVec3 AO = -A;
	const dVec3 AB = B - A;
	const dVec3 AC = C - A;

	const double AB_AB = AB.Dot(AB);
	const double AB_AC = AB.Dot(AC);

	const double L00 = sqrt(AB_AB);
	const double L10 = AB_AC / L00;
	const double L11 = sqrt(AC.Dot(AC) - AB_AC*AB_AC / AB_AB);

	const double y0 = AO.Dot(AB) / L00;
	const double y1 = (AO.Dot(AC) - L10 * y0) / L11;

	const double t = y1 / L11;
	const double s = (y0 - L10 * t) / L00;

	if (s <= 0.0)
	{
		return ClosestPointToOrigin2(iA, iC, closestFeature);
	}
	else if (t <= 0.0)
	{
		return ClosestPointToOrigin2(iA, iB, closestFeature);
	}
	else if (s + t >= 1.0)
	{
		return ClosestPointToOrigin2(iB, iC, closestFeature);
	}
	else
	{
		closestFeature.m_nPts = 3;
		closestFeature.m_pts[0] = m_pts[iA];
		closestFeature.m_pts[1] = m_pts[iB];
		closestFeature.m_pts[2] = m_pts[iC];

		dMinkowskiPoint pq;
		pq.m_MinkDif = A + AB.Scale(s) + AC.Scale(t);
		pq.m_MinkSum = m_pts[iA].m_MinkSum
			+ (m_pts[iB].m_MinkSum - m_pts[iA].m_MinkSum).Scale(s)
			+ (m_pts[iC].m_MinkSum - m_pts[iA].m_MinkSum).Scale(t);
		return pq;
	}
}
dMinkowskiPoint GJKSimplex::ClosestPointToOrigin4(int iA, int iB, int iC, int iD, GJKSimplex& closestFeature) const
{
	const dVec3& A = m_pts[iA].m_MinkDif;
	const dVec3& B = m_pts[iB].m_MinkDif;
	const dVec3& C = m_pts[iC].m_MinkDif;
	const dVec3& D = m_pts[iD].m_MinkDif;

	const dVec3 AO = -A;
	const dVec3 AB = B - A;
	const dVec3 AC = C - A;
	const dVec3 AD = D - A;

	const double AB_AB = AB.Dot(AB);
	const double AB_AC = AB.Dot(AC);
	const double AB_AD = AB.Dot(AD);

	// CHOLESKY

	const double L00 = sqrt(AB_AB);
	const double L10 = AB_AC / L00;
	const double L20 = AB_AD / L00;

	const double L11_sqr = std::max(0.0, AC.Dot(AC) - AB_AC*AB_AC / AB_AB);
	const double L21_num = AC.Dot(AD) - AB_AC*AB_AD / AB_AB;

	const double L11 = sqrt(L11_sqr);
	const double L21 = L21_num / L11;

	const double L22_sqr = std::max(0.0, AD.Dot(AD) - (AB_AD*AB_AD / AB_AB) - L21_num*L21_num / L11_sqr);
	const double L22 = sqrt(L22_sqr);

	const double y0 = AO.Dot(AB) / L00;
	const double y1 = (AO.Dot(AC) - L10 * y0) / L11;
	const double y2 = (AO.Dot(AD) - L20 * y0 - L21 * y1) / L22;

	const double t2 = y2 / L22;
	const double t1 = (y1 - L21*t2) / L11;
	const double t0 = (y0 - L10*t1 - L20*t2) / L00;

	double LT[3][3];
	LT[0][0] = L00;
	LT[0][1] = L10;
	LT[0][2] = L20;
	LT[1][0] = 0.0;
	LT[1][1] = L11;
	LT[1][2] = L21;
	LT[2][0] = 0.0;
	LT[2][1] = 0.0;
	LT[2][2] = L22;

	double M[3][3];
	M[0][0] = AB.Dot(AB);
	M[0][1] = AB.Dot(AC);
	M[0][2] = AB.Dot(AD);
	M[1][0] = AC.Dot(AB);
	M[1][1] = AC.Dot(AC);
	M[1][2] = AC.Dot(AD);
	M[2][0] = AD.Dot(AB);
	M[2][1] = AD.Dot(AC);
	M[2][2] = AD.Dot(AD);
	double LT__[3][3];

	MathUtils::CholeskyFactorization(&M[0][0], 3, &LT__[0][0]);

	if (t0 <= 0.0)
	{
		return ClosestPointToOrigin3(iA, iC, iD, closestFeature);
	}
	else if (t1 <= 0.0)
	{
		return ClosestPointToOrigin3(iA, iB, iD, closestFeature);
	}
	else if (t2 <= 0.0)
	{
		return ClosestPointToOrigin3(iA, iB, iC, closestFeature);
	}
	else if (t0 + t1 + t2 >= 1.0)
	{
		return ClosestPointToOrigin3(iB, iC, iD, closestFeature);
	}
	else
	{
		closestFeature = *this;
		dMinkowskiPoint pq;
		pq.m_MinkDif = dVec3(0.0, 0.0, 0.0);
		pq.m_MinkSum = m_pts[iA].m_MinkSum
			+ (m_pts[iB].m_MinkSum - m_pts[iA].m_MinkSum).Scale(t0)
			+ (m_pts[iC].m_MinkSum - m_pts[iA].m_MinkSum).Scale(t1)
			+ (m_pts[iD].m_MinkSum - m_pts[iA].m_MinkSum).Scale(t2);
		return pq;
	}
}

dMinkowskiPoint GJKSimplex::ClosestPointToOrigin(GJKSimplex& closestFeature) const
{
	switch (m_nPts)
	{
	case 1:
		closestFeature.m_nPts = 1;
		closestFeature.m_pts[0] = m_pts[0];
		return m_pts[0];
	case 2:
		return ClosestPointToOrigin2(0, 1, closestFeature);
	case 3:
		return ClosestPointToOrigin3(0, 1, 2, closestFeature);
	case 4:
		return ClosestPointToOrigin4(0, 1, 2, 3, closestFeature);
	}
	assert(false);
	return dMinkowskiPoint();
}
