#pragma once
#include "YshMath.h"
#include "Vec3.h"

struct MinkowskiPoint 
{
	dVec3 m_MinkDif;
	dVec3 m_MinkSum;
};

struct GJKSimplex
{
	GJKSimplex() : m_nPts(0) {}

	MinkowskiPoint ClosestPointToOrigin(GJKSimplex& closestFeature) const;
	void AddPoint(const MinkowskiPoint& pt) { m_pts[m_nPts] = pt; m_nPts++; }
	int GetNumPoints() const { return m_nPts; }

	MinkowskiPoint m_pts[4];
	int m_nPts;

private:
	inline MinkowskiPoint ClosestPointToOrigin2(int iA, int iB, GJKSimplex& closestFeature) const;
	inline MinkowskiPoint ClosestPointToOrigin3(int iA, int iB, int iC, GJKSimplex& closestFeature) const;
	inline MinkowskiPoint ClosestPointToOrigin4(int iA, int iB, int iC, int iD, GJKSimplex& closestFeature) const;
};

struct EPAHull
{
private:

	struct Vertex;
	struct Face;
	struct HalfEdge;

	struct HalfEdge
	{
		~HalfEdge();
		void SetVertex(Vertex* vertex);

		HalfEdge*	twin = nullptr;
		HalfEdge*	next = nullptr;
		HalfEdge*	prev = nullptr;
		Face*	 	face = nullptr;
	private:
		Vertex*	 	vert = nullptr;
	};
	struct Face
	{
		HalfEdge* edge = nullptr;

		double distance;
		dVec3 normal;
		
		bool operator < (const Face& face)
		{
			return distance > face.distance;
		}
	};
	struct Vertex
	{
		friend class HalfEdge;

		MinkowskiPoint	point;

	private:
		int nRefs = 0; // reference counting
	};

	// DATA BUFFERS

	Face m_faces[256];
	int m_nFaces = 0;

	MinkowskiPoint m_vertices[256];
	int m_nVertices = 0;

	// MEMORY MANAGEMENT

	HalfEdge* m_halfEdgeFreeList[256];
	int m_nFreeHalfEdges = 0;
	MinkowskiPoint* m_vertexFreeList[256];
	int m_nFreeVertices = 0;

	
	void AddPoint(const MinkowskiPoint& pt, const Face& face);

	void InvalidateFace(Face& face);


};