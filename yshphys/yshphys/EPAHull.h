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

	double ComputePenetration(dVec3& pt0, dVec3& pt1);

	int EulerCharacteristic() const;
	
	bool Expand();

	void DebugDraw(DebugRenderer* renderer) const;

private:

	struct HalfEdge
	{
		const MinkowskiPoint*	vert;
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

		Face() : edge(nullptr), visited(false), index(-1) {}
		void Reset()
		{
			edge = nullptr;
			visited = false;
		}

		MinkowskiPoint ComputeClosestPointToOrigin() const;
	};

	static bool CompareFacesByDistance(const Face*, const Face*);

	Face*			m_faceHeap[EPAHULL_MAXFACES];
	Face			m_faces[EPAHULL_MAXFACES];
	MinkowskiPoint	m_verts[EPAHULL_MAXVERTS];
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
	bool			m_faceValidities[EPAHULL_MAXFACES];

	OrientedGeometry	m_geom0;
	OrientedGeometry	m_geom1;

	void PushFaceHeap(Face* face)
	{
		m_faceHeap[m_nFacesInHeap] = face;
		m_nFacesInHeap++;
		std::push_heap(m_faceHeap, &m_faceHeap[m_nFacesInHeap], CompareFacesByDistance);
	}
	Face* PopFaceHeap()
	{
		std::pop_heap(m_faceHeap, &m_faceHeap[m_nFacesInHeap], CompareFacesByDistance);
		m_nFacesInHeap--;
		return m_faceHeap[m_nFacesInHeap];
	}

	void PushFreeFace(Face* face)
	{
		m_freeFaces[m_nFreeFaces] = face->index;
		m_nFreeFaces++;

		face->Reset();
		m_faceValidities[face->index] = true;
	}
	Face* PopFreeFace()
	{
		m_nFreeFaces--;
		return &m_faces[m_freeFaces[m_nFreeFaces]];
	}

	void PushFreeEdge(HalfEdge* edge)
	{
		m_freeEdges[m_nFreeEdges] = edge->index;
		m_nFreeEdges++;
	}
	HalfEdge* PopFreeEdge()
	{
		m_nFreeEdges--;
		return &m_edges[m_freeEdges[m_nFreeEdges]];
	}

	void CarveHorizon(const dVec3& eye, const Face* visibleFace);
	void PatchHorizon(const MinkowskiPoint* eye);
};