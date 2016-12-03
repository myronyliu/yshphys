#include "stdafx.h"
#include "RenderMesh.h"

#define VERTEX_POSITION_DIM 3
#define VERTEX_NORMAL_DIM 3
#define VERTEX_COLOR_DIM 3

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

GLuint RenderMesh::GetIBO() const
{
	return m_IBO;
}

void RenderMesh::GetMeshData
(
	unsigned int& nVertices, const float* positions, const float* normals, const float* colors,
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
void RenderMesh::SetVertex(unsigned int i, const fVec3& position, const fVec3& normal, const fVec3& color)
{
	float* position_i = &m_positions[3 * i];
	float* normal_i = &m_normals[3 * i];
	float* color_i = &m_colors[3 * i];
	position_i[0] = position.x;
	position_i[1] = position.y;
	position_i[2] = position.z;
	normal_i[0] = normal.x;
	normal_i[1] = normal.y;
	normal_i[2] = normal.z;
	color_i[0] = color.x;
	color_i[1] = color.y;
	color_i[2] = color.z;
}
void RenderMesh::SetTriangleIndices(unsigned int iTriangle, unsigned int iVertexA, unsigned int iVertexB, unsigned int iVertexC)
{
	unsigned int* triangle = &m_triangles[3 * iTriangle];
	triangle[0] = iVertexA;
	triangle[1] = iVertexB;
	triangle[2] = iVertexC;
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

	m_positions = new float[nVertices*VERTEX_POSITION_DIM];
	m_normals = new float[nVertices*VERTEX_NORMAL_DIM];
	m_colors = new float[nVertices*VERTEX_COLOR_DIM];

	m_triangles = new unsigned int[nTriangles * 3];
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
	}
	glBindVertexArray(0);

	// Specify the triangle indices data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * m_nTriangles * sizeof(GL_UNSIGNED_INT), m_triangles, GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderMesh::CreateTriangle()
{
	AllocateMesh(3, 1);
	m_nTriangles = 1;
	m_nVertices = 3;

	m_positions[0] = -1.0f;
	m_positions[1] = -1.0f;
	m_positions[2] = 0.0f;
	m_positions[3] = 1.0f;
	m_positions[4] = -1.0f;
	m_positions[5] = 0.0f;
	m_positions[6] = 0.0f;
	m_positions[7] = 1.0f;
	m_positions[8] = 0.0f;

	m_triangles[0] = 0;
	m_triangles[1] = 1;
	m_triangles[2] = 2;

	m_colors[0] = 1.0f;
	m_colors[1] = 1.0f;
	m_colors[2] = 1.0f;
	m_colors[3] = 1.0f;
	m_colors[4] = 1.0f;
	m_colors[5] = 1.0f;
	m_colors[6] = 1.0f;
	m_colors[7] = 1.0f;
	m_colors[8] = 1.0f;

	GenerateGLBufferObjects();
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
		const unsigned int iFaceVertex0 = iVertex; // cache this index, so that we know where to start for this face's triangle indices

		const unsigned int iDimY((iDimX + 1) % 3);
		const unsigned int iDimZ((iDimX + 2) % 3);

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
				const float y((2.0f * (float)i / float(nPointsY - 1) - 1.0f) * halfDimY);
				const float z((2.0f * (float)j / float(nPointsZ - 1) - 1.0f) * halfDimZ);

				fVec3 position;
				fVec3 normal;
				fVec3 color(1.0f, 1.0f, 1.0f);

				position[iDimX] = -halfDimX;
				position[iDimY] = y;
				position[iDimZ] = z;

				normal[iDimX] = -1.0f;
				normal[iDimY] = 0.0f;
				normal[iDimZ] = 0.0f;

				SetVertex(iVertex, position, normal, color);
				
				position[iDimX] = halfDimX;
				position[iDimY] = y;
				position[iDimZ] = z;

				normal[iDimX] = 1.0f;
				normal[iDimY] = 0.0f;
				normal[iDimZ] = 0.0f;

				SetVertex(iVertex + nVertices / 2, position, normal, color);

				iVertex++;
			}
		}

		// FACE INDICES
		for (unsigned int j = 0; j < nEdgesZ; ++j)
		{
			for (unsigned int i = 0; i < nEdgesY; ++i)
			{
				const unsigned int i00((i + 0) + nPointsY * (j + 0) + iFaceVertex0);
				const unsigned int i10((i + 1) + nPointsY * (j + 0) + iFaceVertex0);
				const unsigned int i11((i + 1) + nPointsY * (j + 1) + iFaceVertex0);
				const unsigned int i01((i + 0) + nPointsY * (j + 1) + iFaceVertex0);

				SetTriangleIndices(iTriangle + 0, i00, i11, i10);
				SetTriangleIndices(iTriangle + 1, i00, i01, i11);
				SetTriangleIndices(iTriangle + 0 + nTriangles / 2, i00 + nVertices / 2, i10 + nVertices / 2, i11 + nVertices / 2);
				SetTriangleIndices(iTriangle + 1 + nTriangles / 2, i00 + nVertices / 2, i11 + nVertices / 2, i01 + nVertices / 2);

				iTriangle += 2;
			}
		}
	}
	GenerateGLBufferObjects();
}