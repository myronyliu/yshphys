#include "stdafx.h"
#include "Simplex3D.h"


Simplex3D::Simplex3D() : m_nVertices(0)
{
}


Simplex3D::~Simplex3D()
{
}

void Simplex3D::SetVertices(unsigned int nVertices, const dVec3* vertices)
{
	assert(nVertices <= 4);
	m_nVertices = nVertices;
	std::memcpy(m_vertices, vertices, nVertices * sizeof(dVec3));
}

void Simplex3D::AddVertex(const dVec3& vertex)
{
	assert(m_nVertices < 4);
	m_vertices[m_nVertices] = vertex;
	m_nVertices++;
}

int Simplex3D::GetNumVertices() const
{
	return m_nVertices;
}
int Simplex3D::GetVertices(dVec3* const vertices) const
{
	std::memcpy(vertices, m_vertices, m_nVertices * sizeof(dVec3));
	return m_nVertices;
}

dVec3 Simplex3D::ClosestPoint1(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature)
{
	closestFeature.SetVertices(1, v);
	return v[0];
}
dVec3 Simplex3D::ClosestPoint2(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature)
{
	const dVec3 ax = x - v[0];
	const dVec3 ab = v[1] - v[0];

	const double t = ax.Dot(ab) / ab.Dot(ab);
	
	if (t <= 0.0)
	{
		closestFeature.SetVertices(1, &v[0]);
		return v[0];
	}
	else if (t >= 1.0)
	{
		closestFeature.SetVertices(1, &v[1]);
		return v[1];
	}
	else
	{
		closestFeature.SetVertices(2, v);
		return v[0] + ab.Scale(t);
	}
}
dVec3 Simplex3D::ClosestPoint3(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature)
{
	const dVec3 ax = x - v[0];
	const dVec3 ab = v[1] - v[0];
	const dVec3 ac = v[2] - v[0];

	const double ab_ab = ab.Dot(ab);
	const double ab_ac = ab.Dot(ac);

	double L[3];
	L[0] = sqrt(ab_ab);
	L[1] = ab_ac / L[0];
	L[2] = sqrt(ac.Dot(ac) - ab_ac*ab_ac / ab_ab);

	double s = ax.Dot(ab) / L[0];
	double t = (ax.Dot(ac) - L[1] * s) / L[2];

	if (s <= 0.0)
	{
		const dVec3 line[2] = { v[2],v[0] };
		return ClosestPoint2(line, x, closestFeature);
	}
	else if (t <= 0.0)
	{
		const dVec3 line[2] = { v[0],v[1] };
		return ClosestPoint2(line, x, closestFeature);
	}
	else if (s + t >= 1.0)
	{
		const dVec3 line[2] = { v[1],v[2] };
		return ClosestPoint2(line, x, closestFeature);
	}
	else
	{
		closestFeature.SetVertices(3, v);
		return v[0] + ab.Scale(s) + ac.Scale(t);
	}
}
dVec3 Simplex3D::ClosestPoint4(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature)
{
	const dVec3 ax = x - v[0];
	const dVec3 ab = v[1] - v[0];
	const dVec3 ac = v[2] - v[0];
	const dVec3 ad = v[3] - v[0];

	const double ab_ab = ab.Dot(ab);
	const double ab_ac = ab.Dot(ac);
	const double ab_ad = ab.Dot(ad);

	// CHOLESKY (L0 = row0, L1 = row1, L2 = row2)

	double L0[6];
	double* const L1 = &L0[1];
	double* const L2 = &L0[3];

	L0[0] = sqrt(ab_ab);
	L1[0] = ab_ac / L0[0];
	L2[0] = ab_ad / L0[0];

	const double L11_sqr = ac.Dot(ac) - ab_ac*ab_ac / ab_ab;
	const double L21_num = ac.Dot(ad) - ab_ac*ab_ad / ab_ab;

	L1[1] = sqrt(L11_sqr);
	L2[1] = L21_num / L1[1];

	L2[2] = sqrt(ad.Dot(ad) - (ab_ad*ab_ad / ab_ab) - L21_num*L21_num / L11_sqr);

	double t[3] = {
	ax.Dot(ab) / L0[0],
	(ax.Dot(ac) - L1[0] * t[0]) / L1[1],
	(ax.Dot(ad) - L2[0] * t[0] - L2[1] * t[1]) / L2[2]
	};

	double LT[3][3];
	LT[0][0] = L0[0];
	LT[0][1] = L1[0];
	LT[0][2] = L2[0];
	LT[1][0] = 0.0;
	LT[1][1] = L1[1];
	LT[1][2] = L2[1];
	LT[2][0] = 0.0;
	LT[2][1] = 0.0;
	LT[2][2] = L2[2];

	double A[3][3];
	A[0][0] = ab.Dot(ab);
	A[0][1] = ab.Dot(ac);
	A[0][2] = ab.Dot(ad);
	A[1][0] = ac.Dot(ab);
	A[1][1] = ac.Dot(ac);
	A[1][2] = ac.Dot(ad);
	A[2][0] = ad.Dot(ab);
	A[2][1] = ad.Dot(ac);
	A[2][2] = ad.Dot(ad);
	double LT__[3][3];

	MathUtils::CholeskyFactorization(&A[0][0], 3, &LT__[0][0]);

	if (t[0] <= 0.0)
	{
		const dVec3 triangle[3] = { v[2],v[3],v[0] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t[1] <= 0.0)
	{
		const dVec3 triangle[3] = { v[3],v[0],v[1] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t[2] <= 0.0)
	{
		const dVec3 triangle[3] = { v[0],v[1],v[2] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t[0] + t[1] + t[2] >= 1.0)
	{
		const dVec3 triangle[3] = { v[1],v[2],v[3] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else
	{
		closestFeature.SetVertices(4, v);
		return v[0] + ab.Scale(t[0]) + ac.Scale(t[1]) + ad.Scale(t[2]);
	}
}

dVec3 Simplex3D::ClosestPoint(const dVec3& x, Simplex3D& closestFeature) const
{
	switch (m_nVertices)
	{
	case 1:
		return ClosestPoint1(m_vertices, x, closestFeature);
		break;
	case 2:
		return ClosestPoint2(m_vertices, x, closestFeature);
		break;
	case 3:
		return ClosestPoint3(m_vertices, x, closestFeature);
		break;
	case 4:
		return ClosestPoint4(m_vertices, x, closestFeature);
		break;
	}
	return dVec3(0.0, 0.0, 0.0);
}
