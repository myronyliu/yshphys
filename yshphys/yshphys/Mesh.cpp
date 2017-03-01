#include "stdafx.h"
#include "Mesh.h"

Mesh::Mesh() :
	m_halfEdges(nullptr),
	m_faces(nullptr),
	m_verts(nullptr),
	m_nHalfEdges(0),
	m_nFaces(0),
	m_nVerts(0)
{
}

Mesh::~Mesh()
{
	delete[] m_halfEdges;
	delete[] m_faces;
	delete[] m_verts;
}

void Mesh::AllocateMesh(int nEdges, int nFaces, int nVerts)
{
	delete[] m_halfEdges;
	delete[] m_faces;
	delete[] m_verts;
	
	m_nHalfEdges = 2 * nEdges;
	m_nFaces = nFaces;
	m_nVerts = nVerts;

	m_halfEdges = new HalfEdge[m_nHalfEdges];
	m_faces = new Face[m_nFaces];
	m_verts = new fVec3[m_nVerts];
}

Mesh::HalfEdge* Mesh::SupportLocal(const dVec3& v, HalfEdge* e) const
{
	while (true)
	{
		HalfEdge* e1 = e->next;
		HalfEdge* e1_best = nullptr;
		double d_best = (double)FLT_MIN;

		while (e1 != e)
		{
			const double d = v.Dot(dVec3(*e1->vert) - dVec3(*e1->twin->vert));

			if (d > d_best)
			{
				e1_best = e1;
				d_best = d;
			}
			e1 = e1->twin->next;
		}

		if (e1_best == nullptr)
		{
			break;
		}
		else
		{
			e = e1_best;
		}
	}
	return e;
}

dVec3 Mesh::SupportLocal(const dVec3& v) const
{
	HalfEdge* e = nullptr;
	const dVec3 v_v = v.Times(v);
	for (int dim = 0; dim < 3; dim++)
	{
		if (v_v[dim] >= v_v[(dim + 1) % 3] && v_v[dim] >= v_v[(dim + 2) % 3])
		{
			e = (v_v[dim] < 0.0) ? m_cardinalEdges[dim][0] : m_cardinalEdges[dim][1];
			if (v.Dot(dVec3(*e->vert) - dVec3(*e->twin->vert)) < 0.0)
			{
				e = e->twin;
			}
			break;
		}
	}
	return dVec3(*SupportLocal(v, e)->vert);
}

void Mesh::InitCardinalEdges()
{
	m_cardinalEdges[0][0] = SupportLocal(dVec3(-1.0, 0.0, 0.0), m_halfEdges);

	m_cardinalEdges[1][0] = SupportLocal(dVec3(0.0, -1.0, 0.0), m_cardinalEdges[0][0]);
	m_cardinalEdges[1][1] = SupportLocal(dVec3(0.0, 1.0, 0.0), m_cardinalEdges[0][0]);
	m_cardinalEdges[2][0] = SupportLocal(dVec3(0.0, 0.0, -1.0), m_cardinalEdges[0][0]);
	m_cardinalEdges[2][1] = SupportLocal(dVec3(0.0, 0.0, 1.0), m_cardinalEdges[0][0]);

	m_cardinalEdges[0][1] = SupportLocal(dVec3(1.0, 0.0, 0.0), m_cardinalEdges[2][1]);
}