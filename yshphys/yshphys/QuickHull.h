#pragma once
#include "YshMath.h"

#define QUICKHULL_MAXHORIZONEDGES 16
#define QUICKHULL_MAXINTERNALEDGES 64
#define QUICKHULL_MAXINTERNALFACES 32

#define QUICKHULL_MAXFACES 1024
#define QUICKHULL_MAXEDGES 1024

#define QUICKHULL_FACEFIFO_MAXSIZE (2 * QUICKHULL_MAXFACES)

// Contrast with EPAHull:
//   For EPAHull, we try to do everything with stack memory, since we construct EPAHulls every frame (which complicates the code with all this freelist mumbo jumbo)
//   QuickHull is for constructing the convex hull of a point cloud, which is a one-time ordeal, so we just use heap memory and keep it simple.
//   QuickHull is further simplified by the fact that we don't have to maintain a face heap.

class QuickHull
{
public:
	QuickHull(const fVec3* verts, int nVerts);
	virtual ~QuickHull();

	bool Expand(double dSlack);

private:
	struct HalfEdge;
	struct Face;
	struct FaceFIFO;

	struct HalfEdge
	{
		const fVec3*	vert;
		HalfEdge*		next;
		HalfEdge*		prev;
		HalfEdge*		twin;
		Face*			face;

		mutable bool			isHorizon;

		HalfEdge() : vert(nullptr), next(nullptr), prev(nullptr), twin(nullptr), face(nullptr), isHorizon(false) {}
	};
	struct Face
	{
		dVec3						normal;
		HalfEdge*					edge;

		std::vector<const fVec3*>	vertSet;

		FaceFIFO*					FIFO;
		int							iFIFO;

		mutable bool	visited;
		mutable bool	visible;

		Face() : normal(0.0, 0.0, 0.0), vertSet(), edge(nullptr), visited(false), visible(false), iFIFO(-1) {}
		~Face() { FIFO->InvalidateElement(iFIFO); }
		void Reset() { normal.x = 0.0; normal.y = 0.0; normal.z = 0.0; vertSet.clear(); edge = nullptr; visited = false; visible = false; }
	};
	struct FaceFIFO
	{
	public:
		FaceFIFO() : m_iStart(0), m_iEnd(0) {}

		void Push(Face* face);
		QuickHull::Face* Pop();

		int Size();

		void InvalidateElement(int iFace) { m_queue[iFace].valid = false; }

	private:
		struct ValidatedFace 
		{
			ValidatedFace() : face(nullptr), valid(false) {}
			QuickHull::Face*	face;
			bool				valid;
		};
		ValidatedFace m_queue[QUICKHULL_FACEFIFO_MAXSIZE];

		int m_iStart;
		int m_iEnd;
	};

	FaceFIFO m_faceFIFO;

//	Face* m_internalFaces[QUICKHULL_MAXINTERNALFACES];
//	int m_nInternalFaces;

//	HalfEdge* m_internalEdges[QUICKHULL_MAXINTERNALEDGES];
//	int m_nInternalEdges;

	HalfEdge* m_horizonEdges[QUICKHULL_MAXHORIZONEDGES];
	int m_nHorizonEdges;

	std::vector<HalfEdge*> m_allocatedEdges;
	std::vector<Face*> m_allocatedFaces;

	const fVec3* m_verts;
	int m_nVerts;

	std::vector<const fVec3*> m_orphanedVerts;

	void InitTetrahedron(double dSlack);

	void CarveHorizon(const fVec3& eye, Face* visibleFace);
	bool PatchHorizon(const fVec3* eye, double dSlack);
};

