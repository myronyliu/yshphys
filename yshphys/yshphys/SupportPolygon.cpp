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
	else if (poly0.nVertices + poly1.nVertices == 3)
	{
		const SupportPolygon* point = &poly0;
		const SupportPolygon* lineSegment = &poly1;
		dVec3* pPt0 = &pt0;
		dVec3* pPt1 = &pt1;

		if (poly1.nVertices == 1)
		{
			point = &poly1;
			lineSegment = &poly0;
			pPt0 = &pt1;
			pPt1 = &pt0;
		}

		const dVec3& x = point->vertices[0];

		const dVec3& a = lineSegment->vertices[0];
		const dVec3& b = lineSegment->vertices[1];

		const dVec3 ab = b - a;
		const dVec3 ax = x - a;
		double t = ab.Dot(ax)/ab.Dot(ab);
		t = std::max(0.0, std::min(t, 1.0));
		*pPt1 = a + ab.Scale(t);
		*pPt0 = x;
		double dSqrMin = (*pPt1 - *pPt0).Dot(*pPt1 - *pPt0);
		return sqrt(dSqrMin);
	}
	else if (poly0.nVertices == 2 && poly1.nVertices == 2)
	{
		const dVec3& a = poly0.vertices[0];
		const dVec3& b = poly0.vertices[1];

		const dVec3& c = poly1.vertices[0];
		const dVec3& d = poly1.vertices[1];

		const dVec3 ab = b - a;
		const dVec3 cd = d - c;

		const double A[2][2] = { {ab.Dot(ab),-ab.Dot(cd)}, {-ab.Dot(cd),cd.Dot(cd)} };
		const double v[2] = { (c - a).Dot(ab),(a - c).Dot(cd) };
		double t[2];
		double tMin[2] = { 0.0,0.0 };
		double tMax[2] = { 1.0,1.0 };
		
		MathUtils::GaussSeidel(&A[0][0], v, tMin, tMax, 2, t);

		pt0 = a + ab.Scale(t[0]);
		pt1 = c + cd.Scale(t[1]);

		return sqrt((pt1 - pt0).Dot(pt1 - pt0));
	}
	assert(false);
	return -1.0;
}