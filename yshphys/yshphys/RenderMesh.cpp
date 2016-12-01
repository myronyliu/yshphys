#include "stdafx.h"
#include "RenderMesh.h"


RenderMesh::RenderMesh() : m_node(nullptr)
{
}

RenderMesh::~RenderMesh()
{
}

RenderNode* RenderMesh::GetRenderNode() const
{
	return m_node;
}

fVec3 RenderMesh::GetPosition() const
{
	return m_pos;
}
fQuat RenderMesh::GetRotation() const
{
	return m_rot;
}
void RenderMesh::SetPosition(const fVec3& pos)
{
	m_pos = pos;
}
void RenderMesh::SetRotation(const fQuat& rot)
{
	m_rot = rot;
}

void RenderMesh::GetMeshData
(
	unsigned int& nVertices, const fVec3* positions, const fVec3* normals, const fVec3* colors,
	unsigned int& nIndices, const unsigned int* indices
)
const
{
	nVertices = m_nVertices;
	nIndices = m_nIndices;
	positions = m_positions;
	normals = m_normals;
	colors = m_colors;
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

	m_positions = new fVec3[nVertices];
	m_normals = new fVec3[nVertices];
	m_colors = new fVec3[nVertices];

	m_indices = new unsigned int[3 * nIndices];
}

void RenderMesh::CreateBox
(
	float halfDimX, float halfDimY, float halfDimZ,
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
	unsigned int iIndex = 0;

	const float halfDims[3] = { halfDimX, halfDimY, halfDimZ };
	const unsigned int nPoints[3] = { divisionsX + 2, divisionsY + 2, divisionsZ + 2 };
	const unsigned int nEdges[3] = { divisionsX + 1, divisionsY + 1, divisionsZ + 1 };

	for (int iDimX = 0; iDimX < 3; ++iDimX)
	{
		const unsigned int iDimY((iDimX + 1) % 3);
		const unsigned int iDimZ((iDimY + 2) % 3);

		const unsigned int& nPointsY = nPoints[iDimY];
		const unsigned int& nPointsZ = nPoints[iDimZ];

		const unsigned int& nEdgesY = nEdges[iDimY];
		const unsigned int& nEdgesZ = nEdges[iDimZ];

		const float& halfDimY = halfDims[iDimY];
		const float& halfDimZ = halfDims[iDimZ];

		// FACE VERTICES
		for (unsigned int j = 0; j < nPointsZ; ++j)
		{
			for (unsigned int i = 0; i < nPointsY; ++i)
			{
				const float y((float(2 * i) / float(nPointsY + 1)) * halfDimY);
				const float z((float(2 * j) / float(nPointsZ + 1)) * halfDimZ);

				fVec3& position_neg = m_positions[iVertex];
				fVec3& normal_neg = m_normals[iVertex];

				position_neg[iDimX] = -halfDimX;
				position_neg[iDimY] = y;
				position_neg[iDimZ] = z;

				normal_neg[iDimX] = -1.0f;
				normal_neg[iDimY] = 0.0f;
				normal_neg[iDimZ] = 0.0f;

				fVec3& position_pos = m_positions[iVertex + nVertices / 2];
				fVec3& normal_pos = m_normals[iVertex + nVertices / 2];
				
				position_pos[iDimX] = halfDimX;
				position_pos[iDimY] = y;
				position_pos[iDimZ] = z;

				normal_neg[iDimX] = 1.0f;
				normal_neg[iDimY] = 0.0f;
				normal_neg[iDimZ] = 0.0f;
			}
		}

		// FACE INDICES
		for (unsigned int j = 0; j < nEdgesZ; ++j)
		{
			for (unsigned int i = 0; i < nEdgesY; ++i)
			{
				const unsigned int i00((i + 0) + nPointsY * (j+0));
				const unsigned int i01((i + 1) + nPointsY * (j+0));
				const unsigned int i11((i + 1) + nPointsY * (j+1));
				const unsigned int i10((i + 1) + nPointsY * (j+0));

				m_indices[iIndex + 0] = i00;
				m_indices[iIndex + 1] = i10;
				m_indices[iIndex + 2] = i11;

				m_indices[iIndex + 3] = i00;
				m_indices[iIndex + 4] = i11;
				m_indices[iIndex + 5] = i01;

				m_indices[iIndex + 0 + nIndices / 2] = i00 + nVertices / 2;
				m_indices[iIndex + 1 + nIndices / 2] = i11 + nVertices / 2;
				m_indices[iIndex + 2 + nIndices / 2] = i10 + nVertices / 2;

				m_indices[iIndex + 3 + nIndices / 2] = i00 + nVertices / 2;
				m_indices[iIndex + 4 + nIndices / 2] = i01 + nVertices / 2;
				m_indices[iIndex + 5 + nIndices / 2] = i11 + nVertices / 2;

				iIndex += 6;
			}
		}
	}
}