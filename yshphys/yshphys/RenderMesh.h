#pragma once

#include "Vec3.h"
#include "Quat.h"

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	void GetMeshData(
		unsigned int& nVertices, const fVec3* positions, const fVec3* normals, const fVec3* colors,
		unsigned int& nIndices, const unsigned int* indices
	) const;

	// FACTORY
	void CreateBox(
		float halfDimX, float halfDimY, float halfDimZ,
		unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ
		);
//	void CreateCylinder(float radius, float halfHeight);

private:

	void ClearMesh();
	void AllocateMesh(unsigned int nVertices, unsigned int nIndices);

	unsigned int m_nVertices;
	unsigned int m_nIndices;

	fVec3* m_positions;
	fVec3* m_normals;
	fVec3* m_colors;

	unsigned int* m_indices;
};
