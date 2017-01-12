#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{

}

void Polygon::AddVertex(const dVec2& vertex)
{
	m_vertices[m_nVertices++] = vertex;
}

// http://stackoverflow.com/questions/13101288/intersection-of-two-convex-polygons
Polygon Polygon::Intersect(const Polygon& poly) const
{
	const dVec2* const p0 = m_vertices;
	const dVec2* const p1 = poly.m_vertices;

	const int n0 = m_nVertices;
	const int n1 = poly.m_nVertices;

	auto FindExtremeIndices = [](const dVec2* const p, int n, int& iTrough, int& iPeak)
	{
		double yMin = p[0].y;
		double yMax = p[0].y;
		iTrough = 0;
		iPeak = 0;
		for (int i = 1; i < n; ++i)
		{
			if (p[i].y < yMin)
			{
				yMin = p[i].y;
				iTrough = i;
			}
			else if (p[i].y > yMax)
			{
				yMax = p[i].y;
				iPeak= i;
			}
		}
	};

	int iTrough0, iPeak0;
	int iTrough1, iPeak1;

	FindExtremeIndices(p0, n0, iTrough0, iPeak0);
	FindExtremeIndices(p1, n1, iTrough1, iPeak1);

	struct Segment
	{
		int iBot;
		int iTop;
	};

	Segment segments0[2];
	Segment segments1[2];

	segments0[0].iBot = iTrough0;
	segments0[1].iBot = iTrough0;
	segments0[0].iTop = (iTrough0 - 1) % n0;
	segments0[1].iTop = (iTrough0 + 1) % n0;

	segments1[0].iBot = iTrough1;
	segments1[1].iBot = iTrough1;
	segments1[0].iTop = (iTrough1 - 1) % n1;
	segments1[1].iTop = (iTrough1 + 1) % n1;

	while (true)
	{
//		if 
	}
}