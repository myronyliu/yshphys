#pragma once
#include "YshMath.h"

#define MAX_POLYGON_VERTICES 16

struct Polygon
{
public:
	Polygon();

	void AddVertex(const fVec2& vertex);

	Polygon PruneColinearVertices() const;

	// The following function assumes both polygons are convex
	Polygon Intersect(const Polygon& poly) const;

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
