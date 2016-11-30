#pragma once

#include "Vec3.h"
#include "Quat.h"

class RenderMesh
{
public:
	RenderMesh();
	virtual ~RenderMesh();

	fVec3 GetPosition() const;
	fQuat GetRotation() const;

	void SetPosition(const fVec3& pos);
	void SetRotation(const fQuat& rot);

	void CreateBox(
		float halfDimX, float halfDimY, float halfDimZ,
		unsigned int divisionsX, unsigned int divisionsY, unsigned int divisionsZ
		);
	void CreateCylinder(float radius, float halfHeight);

private:

	void ClearMesh();
	void AllocateMesh(unsigned int nVertices, unsigned int nIndices);

	unsigned int m_nVertices;
	unsigned int m_nIndices;

	fVec3* m_positions;
	fVec3* m_normals;
	fVec3* m_colors;

	unsigned int* m_indices;

	fVec3 m_pos;
	fQuat m_rot;
};
