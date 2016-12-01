#pragma once

#include "RenderMesh.h"
#include "glew.h"

class RenderObject
{
	friend class RenderNode;
public:
	RenderObject();
	virtual ~RenderObject();


	fVec3 GetPosition() const;
	fQuat GetRotation() const;

	void SetPosition(const fVec3& pos);
	void SetRotation(const fQuat& rot);

	RenderMesh* GetRenderMesh() const;
	RenderNode* GetRenderNode() const;

protected:

	fVec3 m_pos;
	fQuat m_rot;

	RenderNode* m_node;

	RenderMesh* m_mesh;
	GLuint m_forwardShader;
};

