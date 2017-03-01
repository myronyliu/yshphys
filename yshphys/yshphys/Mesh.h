#pragma once
#include "Geometry.h"
class Mesh :
	public Geometry
{
public:
	struct HalfEdge;
	struct Face;

	struct HalfEdge
	{
		const fVec3*	vert; // Our Convention: the vertex at the TIP of the HalfEdge (as opposed to at the TAIL)
		HalfEdge*		next;
		HalfEdge*		prev;
		HalfEdge*		twin;
		Face*			face;

		mutable bool visited;

		HalfEdge() : vert(nullptr), next(nullptr), prev(nullptr), twin(nullptr), face(nullptr), visited(false) {}
	};
	struct Face
	{
		dVec3						normal;
		HalfEdge*					edge;

		mutable bool visited;

		Face() : normal(0.0, 0.0, 0.0), edge(nullptr), visited(false) {}
	};

	Mesh();
	virtual ~Mesh();

	void AllocateMesh(int nEdges, int nFaces, int nVerts);

	virtual dVec3 SupportLocal(const dVec3& v) const;

protected:

	// entryEdge is the starting point of our steepest descent search.
	// The returned HalfEdge's vert is the support point.
	HalfEdge* SupportLocal(const dVec3& v, HalfEdge* entryEdge) const;
	void InitCardinalEdges();

	HalfEdge*	m_halfEdges;
	int			m_nHalfEdges;
	Face*		m_faces;
	int			m_nFaces;
	fVec3*		m_verts;
	int			m_nVerts;

	// m_cardinalEdge[i][0] corresponds to the edge pointing to the vertex that is minimal in the ith-direction
	// m_cardinalEdge[i][1] corresponds to the edge pointing to the vertex that is maximal in the ith-direction
	// i=0 corresponds to the x direction
	// i=1 corresponds to the y direction
	// i=2 corresponds to the z direction
	// (This is to slightly accelerate the Support function)

	HalfEdge*	m_cardinalEdges[3][2];
};

