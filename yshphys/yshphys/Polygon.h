#pragma once
#include "YshMath.h"

#define MAX_POLYGON_VERTICES 16

struct Polygon
{
public:
	Polygon();
	Polygon(const fVec2* const x, int n);

	void AddVertex(const fVec2& vertex);

	Polygon PruneColinearVertices() const;

	// The following function assumes both polygons are convex
	Polygon Intersect(const Polygon& poly) const;
	Polygon IntersectLineSegment(const fVec2& A, const fVec2& B) const;

	bool PointEnclosed(const dVec2& x) const;

	Polygon ReflectX() const;

	const fVec2* GetVertices(int& n) const;

private:
	int m_nVertices;
	fVec2 m_vertices[MAX_POLYGON_VERTICES];

	struct BoundingSquare
	{
		fVec2 min, max;
	}
	m_boundingSquare;

	struct Edge
	{
		fVec2 vert;
		Edge* prev;
		Edge* next;
	};
	void BuildEdges(Edge* edges) const;
};
