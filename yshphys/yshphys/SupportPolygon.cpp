#include "stdafx.h"
#include "SupportPolygon.h"

double SupportPolygon::ComputeSeparation(const SupportPolygon& poly0, dVec3& pt0, const SupportPolygon& poly1, dVec3& pt1)
{
	if (poly0.nVertices == 0 || poly1.nVertices == 0)
	{
		return -1.0;
	}
	if (poly0.nVertices == 1 && poly1.nVertices == 1)
	{
		pt0 = poly0.vertices[0];
		pt1 = poly1.vertices[0];
		return sqrt((pt1 - pt0).Dot(pt1 - pt0));
	}
	if (poly0.nVertices == 1 || poly1.nVertices == 1)
	{
		const SupportPolygon* pPoly0 = &poly0;
		const SupportPolygon* pPoly1 = &poly1;
		dVec3* pPt0 = &pt0;
		dVec3* pPt1 = &pt1;

		if (poly1.nVertices == 1)
		{
			pPoly0 = &poly1;
			pPoly1 = &poly0;
			pPt0 = &pt1;
			pPt1 = &pt0;
		}
		*pPt0 = pPoly0->vertices[0];

		dVec3 v = pPoly1->vertices[pPoly1->nVertices - 1] - pPoly0->vertices[0];
		double t = v.Dot(*pPt0 - pPoly1->vertices[0]) / v.Dot(v);
		t = std::max(0.0, std::min(t, 1.0));
		*pPt1 = pPoly1->vertices[0] + v.Scale(t);
		double dSqrMin = (*pPt1 - *pPt0).Dot(*pPt1 - *pPt0);

		for (int i = 0; i < pPoly1->nVertices - 1; ++i)
		{
			v = pPoly1->vertices[i + 1] - pPoly1->vertices[i];
			t = v.Dot(*pPt0 - pPoly1->vertices[i]) / v.Dot(v);
			t = std::max(0.0, std::min(t, 1.0));
			const dVec3 pt = pPoly1->vertices[i] + v.Scale(t);
			double dSqr = (pt - *pPt0).Dot(pt - *pPt0);
			if (dSqr < dSqrMin)
			{
				*pPt1 = pt;
				dSqrMin = dSqr;
			}
		}
		return sqrt(dSqrMin);
	}

	double dSqrMin = 88888888.0;

	for (int i = 0; i < poly0.nVertices; ++i)
	{
		const int ii = (i + 1) % 3;

		const dVec3 u = poly0.vertices[ii] - poly0.vertices[i];

		double uu = u.Dot(u);

		for (int j = 0; j < poly1.nVertices; ++j)
		{
			const int jj = (j + 1) % 3;

			const dVec3 v = poly1.vertices[jj] - poly1.vertices[j];

			double vv = v.Dot(v);

			const double uv = u.Dot(v);

			if (uv*uv / (uu*vv) > 0.999)
			{
				// REGULARIZATION
				uu *= 1.01;
				vv *= 1.01;
			}
			dMat22 A;
			A(0, 0) = -uu;
			A(0, 1) = uv;
			A(1, 0) = uv;
			A(1, 1) = -vv;
			dVec2 b(
				(poly0.vertices[i] + poly1.vertices[j]).Dot(u),
				(poly0.vertices[i] + poly1.vertices[j]).Dot(v)
			);

			dVec2 x = A.Inverse().Transform(b);
			x[0] = std::max(0.0, std::min(x[0], 1.0));
			x[1] = std::max(0.0, std::min(x[1], 1.0));

			dVec3 p0 = poly0.vertices[i] + u.Scale(x[0]);
			dVec3 p1 = poly1.vertices[j] + v.Scale(x[1]);
			double dSqr = (p1 - p0).Dot(p1 - p0);

			if (dSqr < dSqrMin)
			{
				pt0 = p0;
				pt1 = p1;
				dSqrMin = dSqr;
			}
		}
	}

	return sqrt(dSqrMin);
}