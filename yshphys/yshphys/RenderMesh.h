#pragma once

#include "Vec3.h"
#include "Quat.h"
#include "glew.h"
#include "Shader.h"
#include "Mesh.h"

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	unsigned int GetNTriangles() const;
	GLuint GetVAO() const;
	GLuint GetIBO() const;

	void GetMeshData(
		unsigned int& nVertices, const float* positions, const float* normals, const float* diffuse, const float* specular,
		unsigned int& nTriangles, const unsigned int* indices
	) const;

	void SetVertex(unsigned int i, const fVec3& position, const fVec3& normal, const fVec3& diffuse, const fVec3& specular);
	void SetTriangleIndices(unsigned int iTriangle, unsigned int iVertexA, unsigned int iVertexB, unsigned int iVertexC);

	void Draw(const Shader* const shader, const fMat44& projectionMatrix, const fMat44& viewMatrix, const fMat44& modelMatrix) const;

	// FACTORY
	void CreateTriangle(const fVec3& v0, const fVec3 v1, const fVec3& v2, const fVec3& diffuse, const fVec3& specular);
	void CreateBox(
		float halfDimX, float halfDimY, float halfDimZ,
		unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ,
		const fVec3& diffuse, const fVec3& specular
		);
	void CreateCylinder(float radius, float halfHeight, const fVec3& diffuse, const fVec3& specular);
	void CreateCapsule(float radius, float halfHeight, const fVec3& diffuse, const fVec3& specular);
	void CreateCone(float radius, float height, const fVec3& diffuse, const fVec3& specular);
	void CreateSphere(float radius, const fVec3& diffuse, const fVec3& specular);
	void CreateMesh(const Mesh& mesh, const fVec3& diffuse, const fVec3& specular);

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
	float* m_diffuse; // 3 floats per vertex
	float* m_specular; // 3 floats per vertex

	unsigned int* m_triangles; // 3 unsigned ints per triangle 

	GLuint m_VAO;
	GLuint m_VBO_positions;
	GLuint m_VBO_normals;
	GLuint m_VBO_diffuse;
	GLuint m_VBO_specular;
	GLuint m_IBO;
};
