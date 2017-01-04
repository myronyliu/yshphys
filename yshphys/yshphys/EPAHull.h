#include "Simplex3D.h"
#include "Geometry.h"
#include "DebugRenderer.h"

#define EPAHULL_MAXITERS 16

#define EPAHULL_MAXFACES 128
#define EPAHULL_MAXEDGES 256
#define EPAHULL_MAXVERTS (4 + EPAHULL_MAXITERS)

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

	std::vector<HalfEdge*> ComputeHorizon(const dVec3& eye, const Face* visibleFace) const;

	int EulerCharacteristic() const;
	
	bool Expand();

	void DebugDraw(DebugRenderer* renderer) const;

private:

	struct HalfEdge
	{
		const MinkowskiPoint*	vert = nullptr;
		HalfEdge*				next = nullptr;
		HalfEdge*				prev = nullptr;
		HalfEdge*				twin = nullptr;
		Face*	 				face = nullptr;

		bool IsConvex(double faceThickness) const;
	};
	struct Face
	{
		double			distance;
		dVec3			normal;
		HalfEdge*		edge;
		mutable bool	visited;
		short			index;

		Face() : edge(nullptr), visited(false), index(-1) {}

		MinkowskiPoint ComputeClosestPointToOrigin() const;
	};

	static bool CompareFacesByDistance(const Face*, const Face*);

	Face*			m_faceHeap[EPAHULL_MAXFACES];
	Face			m_faces[EPAHULL_MAXFACES];
	MinkowskiPoint	m_verts[EPAHULL_MAXVERTS];
	HalfEdge		m_edges[EPAHULL_MAXEDGES];

	int				m_nFacesInHeap;
	int				m_nFaces;
	int				m_nVerts;
	int				m_nEdges;

	// ugh this is so ugly, but we cannot manipulate the sorted heap directly, hence the indirection
	bool			m_faceValidities[EPAHULL_MAXFACES];

	OrientedGeometry	m_geom0;
	OrientedGeometry	m_geom1;

	dVec3			m_box;

	void MergeFacesAlongEdge(HalfEdge* edge0);

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

	void PatchHorizon(std::vector<HalfEdge*> horizon, const MinkowskiPoint* eye);
	void EnforceHorizonConvexity(std::vector<HalfEdge*> horizon);
};