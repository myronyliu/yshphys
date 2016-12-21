#pragma once
#include "YshMath.h"

#define MAX_TRIMESH_VERTICES 256
#define MAX_TRIMESH_TRIANGLES 256
#define MAX_TRIMESH_EDGES 256

struct EdgeVertexIndices
{
	unsigned int operator [](int i) const { return m_vertexIndices[i]; }
	unsigned int& operator [](int i) { return m_vertexIndices[i]; }
private:
	unsigned int m_vertexIndices[2];
};
struct TriangleVertexIndices
{
	unsigned int operator [](int i) const { return m_vertexIndices[i]; }
	unsigned int& operator [](int i) { return m_vertexIndices[i]; }
private:
	unsigned int m_vertexIndices[3];
};

class Trimesh 
{
public:
	Trimesh();
	virtual ~Trimesh();

	dVec3 m_vertices[MAX_TRIMESH_VERTICES];
	TriangleVertexIndices m_triangles[MAX_TRIMESH_TRIANGLES];
	dVec3 m_edges[MAX_TRIMESH_EDGES];
};

