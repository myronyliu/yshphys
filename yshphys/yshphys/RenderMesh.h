#pragma once

#include "Vec3.h"
#include "Quat.h"
#include "glew.h"

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	unsigned int GetNTriangles() const;
	GLuint GetVAO() const;
	GLuint GetIBO() const;

	void GetMeshData(
		unsigned int& nVertices, const float* positions, const float* normals, const float* colors,
		unsigned int& nTriangles, const unsigned int* indices
	) const;

	void SetVertex(unsigned int i, const fVec3& position, const fVec3& normal, const fVec3& color);
	void SetTriangleIndices(unsigned int iTriangle, unsigned int iVertexA, unsigned int iVertexB, unsigned int iVertexC);

	// FACTORY
	void CreateTriangle();
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

	// UGH we gotta do it this way unless we force fVec3's to be tightly packed

	float* m_positions; // 3 floats per vertex 
	float* m_normals; // 3 floats per vertex
	float* m_colors; // 3 floats per vertex

	unsigned int* m_triangles; // 3 unsigned ints per triangle 

	GLuint m_VAO;
	GLuint m_VBO_positions;
	GLuint m_VBO_normals;
	GLuint m_VBO_colors;
	GLuint m_IBO;
};
