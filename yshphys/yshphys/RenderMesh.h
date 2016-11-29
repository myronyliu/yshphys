#pragma once

#include "Vec3.h"

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	void CreateBox(
		double halfDimX, double halfDimY, double halfDimZ,
		unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ
		);
	void CreateCylinder(double radius, double halfHeight);

private:

	void ClearMesh();
	void AllocateMesh(unsigned int nVertices, unsigned int nIndices);

	unsigned int m_nVertices;
	unsigned int m_nIndices;

	Vec3* m_positions;
	Vec3* m_normals;
	Vec3* m_colors;

	unsigned int* m_indices;
};

