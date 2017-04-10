#include "Simplex3D.h"
#include "Geometry.h"
#include "DebugRenderer.h"

#define EPAHULL_MAXITERS 1024 

#define EPAHULL_MAXFACES 1024
#define EPAHULL_MAXEDGES 1024
#define EPAHULL_MAXVERTS (4 + EPAHULL_MAXITERS)

#define EPAHULL_MAXHORIZONEDGES 16

struct OrientedGeometry
{
	const Geometry* geom;
	dVec3 pos;
	dQuat rot;

	dVec3 Support(const dVec3& v) const { return geom->Support(pos, rot, v); }
};

class EPAHull
{
private:
	struct HalfEdge;
	struct Face;
	struct FaceHeap;
public:

	EPAHull(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1,
		const GJKSimplex& tetrahedron);

	bool ComputeIntersection(dVec3& pt0, dVec3& n0, dVec3& pt1, dVec3& n1);

	void SanityCheck() const;
	
	bool Expand();

	void DebugDraw(DebugRenderer* renderer) const;

private:

	struct HalfEdge
	{
		const fMinkowskiPoint*	vert;
		HalfEdge*				next;
		HalfEdge*				prev;
		HalfEdge*				twin;
		Face*	 				face;

		int						index;

		mutable bool			isHorizon;

		HalfEdge() : vert(nullptr), next(nullptr), prev(nullptr), twin(nullptr), face(nullptr), isHorizon(false), index(-1) {}
	};
	struct Face
	{
		friend struct FaceHeap;
	public:
		double			distance;
		dVec3			normal;
		HalfEdge*		edge;

		mutable bool	visited;
		mutable bool	visible;

		Face() : edge(nullptr), visited(false), visible(false), iHeap(-1) {}
		void Reset()
		{
			edge = nullptr;
			visited = false;
			visible = false;
		}

		dMinkowskiPoint ComputeClosestPointToOrigin() const;

	private:
		int				iHeap;
	};

	struct FaceHeap
	{
	public:
		void RemoveFaceFromHeap(Face* face);
		void Push(Face* face);
		void Pop();
		Face* Top() const;
		bool Empty() const { return m_nFaces <= 0; }
	private:
		Face* m_faces[EPAHULL_MAXFACES];
		int m_nFaces = 0;

		void HeapifyUp(int index);
		void HeapifyDown(int index);
		void RemoveFaceFromHeap(int index);
	}
	m_faceHeap;

	Face			m_faces[EPAHULL_MAXFACES];
	fMinkowskiPoint	m_verts[EPAHULL_MAXVERTS]; // store data in single precision, but do computations in double precision
	HalfEdge		m_edges[EPAHULL_MAXEDGES];

	Face*			m_freeFaces[EPAHULL_MAXFACES];
	int				m_nFreeFaces;

	int				m_freeEdges[EPAHULL_MAXEDGES];
	int				m_nFreeEdges;

	int				m_nVerts;

	mutable HalfEdge*	m_horizonEdges[EPAHULL_MAXHORIZONEDGES];
	mutable int			m_nHorizonEdges;

	OrientedGeometry	m_geom0;
	OrientedGeometry	m_geom1;

	// When we free a face, it becomes INACTIVE
	void PushFreeFace(Face* face)
	{
		m_freeFaces[m_nFreeFaces++] = face;
	}
	Face* PopFreeFace()
	{
		Face* face = m_freeFaces[--m_nFreeFaces];
		face->Reset();
		return face;
	}

	void PushFreeEdge(HalfEdge* edge)
	{
		for (int i = 0; i < m_nFreeEdges; ++i)
		{
			assert(edge->index != m_freeEdges[i]);
		}

		m_freeEdges[m_nFreeEdges] = edge->index;
		m_nFreeEdges++;
	}
	HalfEdge* PopFreeEdge()
	{
		m_nFreeEdges--;
		return &m_edges[m_freeEdges[m_nFreeEdges]];
	}

	void CarveHorizon(const fVec3& eye, Face* visibleFace);
	bool PatchHorizon(const fMinkowskiPoint* eye);
};