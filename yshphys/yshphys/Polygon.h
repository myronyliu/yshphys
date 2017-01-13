#pragma once
#include "YshMath.h"

#define MAX_POLYGON_VERTICES 16

struct Polygon
{
public:
	Polygon();

	void AddVertex(const dVec2& vertex);

	Polygon PruneColinearVertices() const;

	// The following function assumes both polygons are convex
	Polygon Intersect(const Polygon& poly) const;

private:
	int m_nVertices;
	dVec2 m_vertices[MAX_POLYGON_VERTICES];
};
