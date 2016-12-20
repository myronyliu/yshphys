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

	int iMax = (poly0.nVertices == 2) ? 1 : poly0.nVertices;
	int jMax = (poly1.nVertices == 2) ? 1 : poly1.nVertices;

	for (int i = 0; i < iMax; ++i)
	{
		const int ii = (i + 1) % poly0.nVertices;

		const dVec3 u = poly0.vertices[ii] - poly0.vertices[i];

		double uu = u.Dot(u);

		for (int j = 0; j < jMax; ++j)
		{
			const int jj = (j + 1) % poly1.nVertices;

			const dVec3 v = poly1.vertices[jj] - poly1.vertices[j];

			double vv = v.Dot(v);

			const double uv = u.Dot(v);

			dVec3 p0, p1;
			double dSqr;

			if (uv*uv / (uu*vv) > 0.999)
			{
				dVec3 lineMin0 = poly0.vertices[i];
				dVec3 lineMax0 = poly0.vertices[ii];

				dVec3 lineMin1 = poly1.vertices[j];
				dVec3 lineMax1 = poly1.vertices[jj];

				double sMin = lineMin0.Dot(u) / u.Dot(u);
				double sMax = lineMax0.Dot(u) / u.Dot(u);

				double tMin = lineMin1.Dot(u) / u.Dot(u);
				double tMax = lineMax1.Dot(u) / u.Dot(u);

				if (sMax < sMin)
				{
					double swp = sMin;
					sMin = sMax;
					sMax = swp;
					dVec3 tmp = lineMin0;
					lineMax0 = lineMin0;
					lineMin0 = tmp;
				}
				if (tMax < tMin)
				{
					double swp = tMin;
					tMin = tMax;
					tMax = swp;
					dVec3 tmp = lineMin1;
					lineMax1 = lineMin1;
					lineMin1 = tmp;
				}

				dVec3 w = poly1.vertices[j] - poly0.vertices[i];
				dVec3 perp = w - u.Scale(w.Dot(u) / u.Dot(u));

				if (sMax < tMin)
				{
					p0 = lineMax0;
					p1 = lineMin1;
				}
				else if (tMax < sMin)
				{
					p0 = lineMin0;
					p1 = lineMax1;
				}
				else if (sMax > tMin)
				{
					p0 = lineMax0;
					p1 = p0 + perp;
				}
				else if (sMin < tMax)
				{
					p0 = lineMin0;
					p1 = p0 + perp;
				}
			}
			else
			{
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

				p0 = poly0.vertices[i] + u.Scale(x[0]);
				p1 = poly1.vertices[j] + v.Scale(x[1]);
			}

			dSqr = (p1 - p0).Dot(p1 - p0);

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