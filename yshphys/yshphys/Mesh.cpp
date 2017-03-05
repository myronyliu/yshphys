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

int Mesh::ISupportEdgeLocal(const dVec3& v, int i_) const
{
	int i = i_;
	while (true)
	{
		const HalfEdge* e = &m_halfEdges[i];
		int i1 = e->iNext;
		int i1_best = -1;
		double d_best = (double)FLT_MIN;

		while (i1 != i)
		{
			HalfEdge* e1 = &m_halfEdges[i1];

			const int j1 = e1->iTwin;
			const HalfEdge* t1 = &m_halfEdges[j1];

			const dVec3 B(m_verts[e1->iVert]);
			const dVec3 A(m_verts[t1->iVert]);
			const double d = v.Dot(B - A);

			if (d > d_best)
			{
				i1_best = i1;
				d_best = d;
			}
			i1 = t1->iNext;
		}

		if (i1_best < 0)
		{
			break;
		}
		else
		{
			i = i1_best;
		}
	}
	return i;
}

dVec3 Mesh::SupportLocal(const dVec3& v) const
{
	int i = -1;
	const dVec3 v_v = v.Times(v);
	for (int dim = 0; dim < 3; dim++)
	{
		if (v_v[dim] >= v_v[(dim + 1) % 3] && v_v[dim] >= v_v[(dim + 2) % 3])
		{
			i = (v[dim] < 0.0) ? m_iCardinalEdges[dim][0] : m_iCardinalEdges[dim][1];
			const HalfEdge* e = &m_halfEdges[i];
			const int j = e->iTwin;
			const HalfEdge* t = &m_halfEdges[j];
			const dVec3 B(m_verts[e->iVert]);
			const dVec3 A(m_verts[t->iVert]);
			if (v.Dot(B - A) < 0.0)
			{
				i = j;
			}
			break;
		}
	}
	const int iSupportEdge = ISupportEdgeLocal(v, i);
	const HalfEdge& supportEdge = m_halfEdges[iSupportEdge];
	const int iSupportVert = supportEdge.iVert;
	return dVec3(m_verts[iSupportVert]);
}

void Mesh::InitCardinalEdges()
{
	m_iCardinalEdges[0][0] = ISupportEdgeLocal(dVec3(-1.0, 0.0, 0.0), 0);

	m_iCardinalEdges[1][0] = ISupportEdgeLocal(dVec3(0.0, -1.0, 0.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[1][1] = ISupportEdgeLocal(dVec3(0.0, 1.0, 0.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[2][0] = ISupportEdgeLocal(dVec3(0.0, 0.0, -1.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[2][1] = ISupportEdgeLocal(dVec3(0.0, 0.0, 1.0), m_iCardinalEdges[0][0]);

	m_iCardinalEdges[0][1] = ISupportEdgeLocal(dVec3(1.0, 0.0, 0.0), m_iCardinalEdges[2][1]);

	for (int dim = 0; dim < 3; ++dim)
	{
		m_localOOBB.min[dim] = m_verts[m_halfEdges[m_iCardinalEdges[dim][0]].iVert][dim];
		m_localOOBB.max[dim] = m_verts[m_halfEdges[m_iCardinalEdges[dim][1]].iVert][dim];
	}
}