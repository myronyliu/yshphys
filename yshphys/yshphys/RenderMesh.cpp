#include "stdafx.h"
#include "RenderMesh.h"


RenderMesh::RenderMesh()
{
}


RenderMesh::~RenderMesh()
{
}


void RenderMesh::ClearMesh()
{
	m_nVertices = 0;
	m_nIndices = 0;

	delete[] m_indices;

	delete[] m_positions;
	delete[] m_normals;
	delete[] m_colors;
}

void RenderMesh::AllocateMesh(unsigned int nVertices, unsigned int nIndices)
{
	ClearMesh();

	m_nVertices = nVertices;
	m_nIndices = nIndices;

	m_positions = new Vec3[nVertices];
	m_normals = new Vec3[nVertices];
	m_colors = new Vec3[nVertices];

	m_indices = new unsigned int[3 * nIndices];
}

void RenderMesh::CreateBox
(
	double halfDimX, double halfDimY, double halfDimZ,
	unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ
)
{
	const unsigned int nVertices(2 * (
		(divisionsX + 2)*(divisionsY + 2) +
		(divisionsY + 2)*(divisionsZ + 2) +
		(divisionsZ + 2)*(divisionsX + 2)));
	const unsigned int nIndices(4 * (
		(divisionsX + 1)*(divisionsY + 1) +
		(divisionsY + 1)*(divisionsZ + 1) +
		(divisionsZ + 1)*(divisionsX + 1)));
	AllocateMesh(nVertices, nIndices);

	unsigned int iVertex = 0;


}