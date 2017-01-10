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
		void Reset()
		{
			vert = nullptr;
			next = nullptr;
			prev = nullptr;
			twin = nullptr;
			face = nullptr;

			isHorizon = false;
		}
	};
	struct Face
	{
		double			distance;
		dVec3			normal;
		HalfEdge*		edge;

		int				index;

		mutable bool	visited;
		mutable bool	visible;

		Face() : edge(nullptr), visited(false), visible(false), index(-1) {}
		void Reset()
		{
			edge = nullptr;
			visited = false;
			visible = false;
		}

		dMinkowskiPoint ComputeClosestPointToOrigin() const;
	};

	static bool CompareFacesByDistance(const Face*, const Face*);

	Face*			m_faceHeap[EPAHULL_MAXFACES];
	Face			m_faces[EPAHULL_MAXFACES];
	fMinkowskiPoint	m_verts[EPAHULL_MAXVERTS]; // store data in single precision, but do computations in double precision
	HalfEdge		m_edges[EPAHULL_MAXEDGES];

	int				m_freeFaces[EPAHULL_MAXFACES];
	int				m_nFreeFaces;

	int				m_freeEdges[EPAHULL_MAXEDGES];
	int				m_nFreeEdges;

	int				m_nFacesInHeap;
	int				m_nVerts;

	mutable HalfEdge*	m_horizonEdges[EPAHULL_MAXHORIZONEDGES];
	mutable int			m_nHorizonEdges;

	// ugh this is so ugly, but we cannot manipulate the sorted heap directly, hence the indirection
	struct FaceStatus
	{
		bool active = false;
		bool inHeap = false;
	}
	m_faceStatuses[EPAHULL_MAXFACES];

	OrientedGeometry	m_geom0;
	OrientedGeometry	m_geom1;

	// When we add a face to the heap, it is assumed to be ACTIVE
	void PushFaceHeap(Face* face)
	{
		for (int i = 0; i < m_nFacesInHeap; ++i)
		{
			assert(face != m_faceHeap[i]);
		}
		m_faceHeap[m_nFacesInHeap] = face;
		m_nFacesInHeap++;

		m_faceStatuses[face->index].active = true;
		m_faceStatuses[face->index].inHeap = true;

		std::push_heap(m_faceHeap, &m_faceHeap[m_nFacesInHeap], CompareFacesByDistance);
	}
	Face* PopFaceHeap()
	{
		std::pop_heap(m_faceHeap, &m_faceHeap[m_nFacesInHeap], CompareFacesByDistance);
		m_nFacesInHeap--;
		Face* face = m_faceHeap[m_nFacesInHeap];

		m_faceStatuses[face->index].inHeap = false;

		return face;
	}

	// When we free a face, it becomes INACTIVE
	void PushFreeFace(Face* face)
	{
		if (!m_faceStatuses[face->index].inHeap)
		{
			for (int i = 0; i < m_nFacesInHeap; ++i)
			{
				assert(face != m_faceHeap[i]);
			}
			for (int i = 0; i < m_nFreeFaces; ++i)
			{
				assert(face->index != m_freeFaces[i]);
			}

			m_freeFaces[m_nFreeFaces] = face->index;
			m_nFreeFaces++;
		}
		m_faceStatuses[face->index].active = false;
	}
	Face* PopFreeFace()
	{
		m_nFreeFaces--;
		Face* face = &m_faces[m_freeFaces[m_nFreeFaces]];
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

	bool FaceIsActive(const Face* face) const
	{
		return m_faceStatuses[face->index].active;
	}

	void CarveHorizon(const fVec3& eye, Face* visibleFace);
	void PatchHorizon(const fMinkowskiPoint* eye);
};