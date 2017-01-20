#pragma once
#include "YshMath.h"

#define MAX_POLYGON_VERTICES 256

struct Polygon
{
public:
	Polygon();
	Polygon(const fVec2* const x, int n);

	void AddVertex(const fVec2& vertex);

	Polygon PruneColinearVertices(double cosThesh) const;

	// The following function assumes both polygons are convex
	Polygon Intersect(const Polygon& poly) const;
	Polygon IntersectLineSegment(const fVec2& A, const fVec2& B) const;

	bool PointEnclosed(const dVec2& x) const;

	Polygon ReflectX() const;

	const fVec2* GetVertices(int& n) const;

private:
	int m_nVertices;
	fVec2 m_vertices[MAX_POLYGON_VERTICES];

	struct Vertex
	{
		fVec2 x;
		Vertex* next;
		Vertex* prev;
	};

	struct HalfEdge
	{
		fVec2 vert;
		HalfEdge* prev;
		HalfEdge* next;
		HalfEdge* twin;

		bool visited;
	};
	// even indices are CCW edges, odd are twin CW edges
	void BuildEdges(HalfEdge* edges) const;
	Polygon WalkingIntersect(const Polygon& poly) const;

	Polygon DoIntersect(const Polygon& poly) const;
};
