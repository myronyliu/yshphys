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

void RenderMesh::CreateTriangle(const fVec3& v0, const fVec3 v1, const fVec3& v2, const fVec3& color)
{
	AllocateMesh(3, 1);

	fVec3 n = (v1 - v0).Cross(v2 - v0);
	n.Scale(sqrtf(1.0f / n.Dot(n)));

	SetVertex(0, v0, n, color);
	SetVertex(1, v1, n, color);
	SetVertex(2, v2, n, color);

	SetTriangleIndices(0, 0, 1, 2);

	GenerateGLBufferObjects();
}

void RenderMesh::CreateBox
(
	float halfDimX, float halfDimY, float halfDimZ,
	unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ,
	const fVec3& color
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

void RenderMesh::CreateCylinder(float radius, float halfHeight, const fVec3& color)
{
	const int nCirclePts = 32; // also the number of segments
	const int nRadiusPts = 8;

	const int nHeightPts = int((halfHeight / (fPI*radius))*(float)nCirclePts) + 1;

	const int nWallVertices = nHeightPts*nCirclePts;
	const int nWallTriangles = 2 * (nHeightPts - 1)*nCirclePts;

	const int nCapVertices = 1 + (nRadiusPts - 1) * nCirclePts;
	const int nCapTriangles = 2 * (nRadiusPts - 1) * nCirclePts + nCirclePts;

	const int nVertices = nWallVertices + 2 * nCapVertices;
	const int nTriangles = nWallTriangles + 2 * nCapTriangles;

	AllocateMesh(nVertices, nTriangles);

	int iVertex = 0;

	for (int j = 0; j < nCirclePts; ++j)
	{
		const float phi = (float)j * 2.0f *fPI / nCirclePts;
		const float cosPhi = cos(phi);
		const float sinPhi = sin(phi);

		for (int i = 0; i < nHeightPts; ++i, ++iVertex)
		{
			fVec3 position;
			position.x = radius*cosPhi;
			position.y = radius*sinPhi;
			position.z = ((float)i*2.0f / (float)(nHeightPts - 1) - 1.0f) * halfHeight;
			fVec3 normal;
			normal.x = cosPhi;
			normal.y = sinPhi;
			normal.z = 0.0f;
			SetVertex(iVertex, position, normal, color);
		}
	}

	SetVertex(iVertex++, fVec3(0.0f, 0.0f, -halfHeight), fVec3(0.0f, 0.0f, -1.0f), color);
	for (int j = 0; j < nCirclePts; ++j)
	{
		const float phi = (float)j * 2.0f *fPI / nCirclePts;
		const float cosPhi = cos(phi);
		const float sinPhi = sin(phi);

		for (int i = 1; i < nRadiusPts; ++i, ++iVertex)
		{
			const float r = (float)i * radius / (float)(nRadiusPts - 1);

			fVec3 position;
			position.x = r*cosPhi;
			position.y = r*sinPhi;
			position.z = -halfHeight;
			fVec3 normal;
			normal.x = 0.0f;
			normal.y = 0.0f;
			normal.z = -1.0f;
			SetVertex(iVertex, position, normal, color);
		}
	}
	SetVertex(iVertex++, fVec3(0.0f, 0.0f, halfHeight), fVec3(0.0f, 0.0f, 1.0f), color);
	for (int j = 0; j < nCirclePts; ++j)
	{
		const float phi = (float)j * 2.0f *fPI / nCirclePts;
		const float cosPhi = cos(phi);
		const float sinPhi = sin(phi);

		for (int i = 1; i < nRadiusPts; ++i, ++iVertex)
		{
			const float r = (float)i * radius / (float)(nRadiusPts - 1);

			fVec3 position;
			position.x = r*cosPhi;
			position.y = r*sinPhi;
			position.z = halfHeight;
			fVec3 normal;
			normal.x = 0.0f;
			normal.y = 0.0f;
			normal.z = 1.0f;
			SetVertex(iVertex, position, normal, color);
		}
	}

	int iTriangle = 0;

	for (int j = 0; j < nCirclePts; ++j)
	{
		const int j0 = (j + 0) % nCirclePts;
		const int j1 = (j + 1) % nCirclePts;

		for (int i = 0; i < nHeightPts - 1; ++i, iTriangle +=2)
		{
			const int i00 = (i + 0) + nHeightPts*j0;
			const int i10 = (i + 1) + nHeightPts*j0;
			const int i11 = (i + 1) + nHeightPts*j1;
			const int i01 = (i + 0) + nHeightPts*j1;
			SetTriangleIndices(iTriangle + 0, i00, i01, i11);
			SetTriangleIndices(iTriangle + 1, i00, i11, i10);
		}
		SetTriangleIndices(
			iTriangle++,
			nWallVertices,
			nWallVertices + 1 + 0 + (nRadiusPts - 1)*j0,
			nWallVertices + 1 + 0 + (nRadiusPts - 1)*j1
		);
		for (int i = 0; i < nRadiusPts - 2; ++i, iTriangle +=2)
		{
			const int i00 = nWallVertices + 1 + (i + 0) + (nRadiusPts - 1)*j0;
			const int i10 = nWallVertices + 1 + (i + 1) + (nRadiusPts - 1)*j0;
			const int i11 = nWallVertices + 1 + (i + 1) + (nRadiusPts - 1)*j1;
			const int i01 = nWallVertices + 1 + (i + 0) + (nRadiusPts - 1)*j1;
			SetTriangleIndices(iTriangle + 0, i00, i01, i11);
			SetTriangleIndices(iTriangle + 1, i00, i11, i10);
		}
		SetTriangleIndices(
			iTriangle++,
			nWallVertices + nCapVertices,
			nWallVertices + nCapVertices + 1 + 0 + (nRadiusPts - 1)*j0,
			nWallVertices + nCapVertices + 1 + 0 + (nRadiusPts - 1)*j1
		);
		for (int i = 0; i < nRadiusPts - 2; ++i, iTriangle +=2)
		{
			const int i00 = nWallVertices + nCapVertices + 1 + (i + 0) + (nRadiusPts - 1)*j0;
			const int i10 = nWallVertices + nCapVertices + 1 + (i + 1) + (nRadiusPts - 1)*j0;
			const int i11 = nWallVertices + nCapVertices + 1 + (i + 1) + (nRadiusPts - 1)*j1;
			const int i01 = nWallVertices + nCapVertices + 1 + (i + 0) + (nRadiusPts - 1)*j1;
			SetTriangleIndices(iTriangle + 0, i00, i01, i11);
			SetTriangleIndices(iTriangle + 1, i00, i11, i10);
		}
	}
	GenerateGLBufferObjects();
}
void RenderMesh::CreateCapsule(float radius, float halfHeight, const fVec3& color)
{
	const int nCirclePts = 32; // also the number of segments
	const int nTheta = 16;

	const int nHeightPts = int((halfHeight / (fPI*radius))*(float)nCirclePts) + 1;
	const int nStripPts = 2 * (nTheta - 2) + nHeightPts;

	const int nVertices = nStripPts*nCirclePts + 2;
	const int nTriangles = 2 * nStripPts *nCirclePts;

	AllocateMesh(nVertices, nTriangles);

	const int iTipBot = 0;
	const int iTipTop = nVertices - 1;

	int iVertex = 0;

	SetVertex(iTipBot, fVec3(0.0f, 0.0f, -halfHeight - radius), fVec3(0.0f, 0.0f, -1.0f), color);
	iVertex++;

	for (int j = 0; j < nCirclePts; ++j)
	{
		const float phi = (float)j * 2.0f *fPI / nCirclePts;
		const float cosPhi = cos(phi);
		const float sinPhi = sin(phi);
		
		for (int i = 1; i < nTheta - 1; ++i, ++iVertex)
		{
			float theta = (float)i / ((float)nTheta - 1.0f)*(fPI*0.5f);
			const float cosTheta = cos(theta);
			const float sinTheta = sin(theta);
			fVec3 position;
			position.x = radius*sinTheta*cosPhi;
			position.y = radius*sinTheta*sinPhi;
			position.z = -halfHeight - radius*cosTheta;
			fVec3 normal;
			normal.x = sinTheta*cosPhi;
			normal.y = sinTheta*sinPhi;
			normal.z = -cosTheta;
			SetVertex(iVertex, position, normal, color);
		}
		for (int i = 0; i < nHeightPts; ++i, ++iVertex)
		{
			fVec3 position;
			position.x = radius*cosPhi;
			position.y = radius*sinPhi;
			position.z = ((float)i*2.0f / (float)(nHeightPts - 1) - 1.0f) * halfHeight;
			fVec3 normal;
			normal.x = cosPhi;
			normal.y = sinPhi;
			normal.z = 0.0f;
			SetVertex(iVertex, position, normal, color);
		}
		for (int i = 1; i < nTheta - 1; ++i, ++iVertex)
		{
			float theta = (float)(nTheta - 1 - i) / ((float)nTheta - 1.0f)*(fPI*0.5f);
			const float cosTheta = cos(theta);
			const float sinTheta = sin(theta);
			fVec3 position;
			position.x = radius*sinTheta*cosPhi;
			position.y = radius*sinTheta*sinPhi;
			position.z = halfHeight + radius*cosTheta;
			fVec3 normal;
			normal.x = sinTheta*cosPhi;
			normal.y = sinTheta*sinPhi;
			normal.z = cosTheta;
			SetVertex(iVertex, position, normal, color);
		}
	}

	SetVertex(iTipTop, fVec3(0.0f, 0.0f, halfHeight + radius), fVec3(0.0f, 0.0f, 1.0f), color);
	iVertex++;

	int iTriangle = 0;

	for (int j = 0; j < nCirclePts; ++j)
	{
		const int j0 = (j + 0) % nCirclePts;
		const int j1 = (j + 1) % nCirclePts;

		int a(1 + nStripPts * j0);
		int b(1 + nStripPts * j1);
		SetTriangleIndices(iTriangle, iTipBot, b, a);
		iTriangle += 1;

		for (int i = 0; i < nStripPts - 1; ++i)
		{
			const int i00 = 1 + (i + 0) + nStripPts*j0;
			const int i10 = 1 + (i + 1) + nStripPts*j0;
			const int i11 = 1 + (i + 1) + nStripPts*j1;
			const int i01 = 1 + (i + 0) + nStripPts*j1;
			SetTriangleIndices(iTriangle + 0, i00, i01, i11);
			SetTriangleIndices(iTriangle + 1, i00, i11, i10);
			iTriangle += 2;
		}

		a = 1 + (nStripPts - 1) + nStripPts*j0;
		b = 1 + (nStripPts - 1) + nStripPts*j1;
		SetTriangleIndices(iTriangle, iTipTop, a, b);
		iTriangle += 1;
	}
	GenerateGLBufferObjects();
}