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

	const double L00 = sqrt(ab_ab);
	const double L10 = ab_ac / L00;
	const double L11 = sqrt(ac.Dot(ac) - ab_ac*ab_ac / ab_ab);

	const double y0 = ax.Dot(ab) / L00;
	const double y1 = (ax.Dot(ac) - L10 * y0) / L11;

	const double t = y1 / L11;
	const double s = (y0 - L10 * t) / L00;

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

	// CHOLESKY

	const double L00 = sqrt(ab_ab);
	const double L10 = ab_ac / L00;
	const double L20 = ab_ad / L00;

	const double L11_sqr = std::max(0.0, ac.Dot(ac) - ab_ac*ab_ac / ab_ab);
	const double L21_num = ac.Dot(ad) - ab_ac*ab_ad / ab_ab;

	const double L11 = sqrt(L11_sqr);
	const double L21 = L21_num / L11;

	const double L22_sqr = std::max(0.0, ad.Dot(ad) - (ab_ad*ab_ad / ab_ab) - L21_num*L21_num / L11_sqr);
	const double L22 = sqrt(L22_sqr);

	const double y0 = ax.Dot(ab) / L00;
	const double y1 = (ax.Dot(ac) - L10 * y0) / L11;
	const double y2 = (ax.Dot(ad) - L20 * y0 - L21 * y1) / L22;

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

	if (t0 <= 0.0)
	{
		const dVec3 triangle[3] = { v[2],v[3],v[0] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t1 <= 0.0)
	{
		const dVec3 triangle[3] = { v[3],v[0],v[1] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t2 <= 0.0)
	{
		const dVec3 triangle[3] = { v[0],v[1],v[2] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else if (t0 + t1 + t2 >= 1.0)
	{
		const dVec3 triangle[3] = { v[1],v[2],v[3] };
		return ClosestPoint3(triangle, x, closestFeature);
	}
	else
	{
		closestFeature.SetVertices(4, v);
		const dVec3 asdf = v[0] + ab.Scale(t0) + ac.Scale(t1) + ad.Scale(t2) - x;
		assert(asdf.Dot(asdf) < 0.000001);
		return x;
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
