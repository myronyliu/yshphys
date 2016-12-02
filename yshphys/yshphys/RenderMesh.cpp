#include "stdafx.h"
#include "RenderMesh.h"


RenderMesh::RenderMesh() : m_VAO(0), m_VBO_positions(0), m_VBO_normals(0), m_VBO_colors(0), m_IBO(0)
{
}

RenderMesh::~RenderMesh()
{
	ClearMesh();
	ClearGLBufferObjects();
}

unsigned int RenderMesh::GetNTriangles() const
{
	return m_nTriangles;
}

GLuint RenderMesh::GetVAO() const
{
	return m_VAO;
}

void RenderMesh::GetMeshData
(
	unsigned int& nVertices, const fVec3* positions, const fVec3* normals, const fVec3* colors,
	unsigned int& nTriangles, const unsigned int* indices
)
const
{
	nVertices = m_nVertices;
	nTriangles = m_nTriangles;
	positions = m_positions;
	normals = m_normals;
	colors = m_colors;
}

void RenderMesh::ClearMesh()
{
	m_nVertices = 0;
	m_nTriangles = 0;

	delete[] m_triangles;

	delete[] m_positions;
	delete[] m_normals;
	delete[] m_colors;
}

void RenderMesh::AllocateMesh(unsigned int nVertices, unsigned int nTriangles)
{
	ClearMesh();

	m_nVertices = nVertices;
	m_nTriangles = nTriangles;

	m_positions = new fVec3[nVertices];
	m_normals = new fVec3[nVertices];
	m_colors = new fVec3[nVertices];

	m_triangles = new TriangleVertexIndices[nTriangles];
}

void RenderMesh::ClearGLBufferObjects()
{
	glBindVertexArray(m_VAO);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO_positions);
	glDeleteBuffers(1, &m_VBO_normals);
	glDeleteBuffers(1, &m_VBO_colors);
	glDeleteBuffers(1, &m_IBO);
}
void RenderMesh::GenerateGLBufferObjects()
{
	ClearGLBufferObjects();

	// allocate GPU memory
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO_positions);
	glGenBuffers(1, &m_VBO_normals);
	glGenBuffers(1, &m_VBO_colors);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);
	{
		// Specify the vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO_positions);
		{
			glBufferData(GL_ARRAY_BUFFER, 3 * m_nVertices * sizeof(GL_FLOAT), m_positions, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO_normals);
		{
			glBufferData(GL_ARRAY_BUFFER, 3 * m_nVertices * sizeof(GL_FLOAT), m_normals, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO_colors);
		{
			glBufferData(GL_ARRAY_BUFFER, 3 * m_nVertices * sizeof(GL_FLOAT), m_colors, GL_STATIC_DRAW);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Enable all three vertex attributes (position, normals, colors)
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		// Specify the triangle indices data
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * m_nTriangles * sizeof(GL_UNSIGNED_INT), m_triangles, GL_STATIC_DRAW);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
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
	const unsigned int nTriangles(4 * (
		(divisionsX + 1)*(divisionsY + 1) +
		(divisionsY + 1)*(divisionsZ + 1) +
		(divisionsZ + 1)*(divisionsX + 1)));
	AllocateMesh(nVertices, nTriangles);

	unsigned int iVertex = 0;
	unsigned int iTriangle = 0;

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

				m_triangles[iTriangle].m_vertexIndices[0] = i00;
				m_triangles[iTriangle].m_vertexIndices[1] = i10;
				m_triangles[iTriangle].m_vertexIndices[2] = i11;

				m_triangles[iTriangle + 1].m_vertexIndices[0] = i00;
				m_triangles[iTriangle + 1].m_vertexIndices[1] = i11;
				m_triangles[iTriangle + 1].m_vertexIndices[2] = i01;

				m_triangles[iTriangle + nTriangles / 2].m_vertexIndices[0] = i00 + nVertices / 2;
				m_triangles[iTriangle + nTriangles / 2].m_vertexIndices[1] = i11 + nVertices / 2;
				m_triangles[iTriangle + nTriangles / 2].m_vertexIndices[2] = i10 + nVertices / 2;

				m_triangles[iTriangle + 1 + nTriangles / 2].m_vertexIndices[0] = i00 + nVertices / 2;
				m_triangles[iTriangle + 1 + nTriangles / 2].m_vertexIndices[1] = i01 + nVertices / 2;
				m_triangles[iTriangle + 1 + nTriangles / 2].m_vertexIndices[2] = i11 + nVertices / 2;

				iTriangle += 2;
			}
		}
	}
}