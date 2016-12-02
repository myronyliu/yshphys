#pragma once

#include "Vec3.h"
#include "Quat.h"
#include "glew.h"

class TriangleVertexIndices
{
public:
	unsigned int m_vertexIndices[3];
};

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	unsigned int GetNTriangles() const;
	GLuint GetVAO() const;

	void GetMeshData(
		unsigned int& nVertices, const fVec3* positions, const fVec3* normals, const fVec3* colors,
		unsigned int& nTriangles, const unsigned int* indices
	) const;

	// FACTORY
	void CreateBox(
		float halfDimX, float halfDimY, float halfDimZ,
		unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ
		);
//	void CreateCylinder(float radius, float halfHeight);

private:

	void ClearMesh();
	void AllocateMesh(unsigned int nVertices, unsigned int nTriangles);

	void ClearGLBufferObjects();
	void GenerateGLBufferObjects();

	unsigned int m_nVertices;
	unsigned int m_nTriangles;

	fVec3* m_positions;
	fVec3* m_normals;
	fVec3* m_colors;

	TriangleVertexIndices* m_triangles;

	GLuint m_VAO;
	GLuint m_VBO_positions;
	GLuint m_VBO_normals;
	GLuint m_VBO_colors;
	GLuint m_IBO;
};
