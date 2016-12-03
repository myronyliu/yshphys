#pragma once

#include "YshMath.h"
#include "RenderMesh.h"
#include "Shader.h"
#include <glew.h>

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
	Shader* GetShader() const;

	void SetRenderMesh(RenderMesh* mesh);
	void SetShader(Shader* shader);


	RenderNode* GetRenderNode() const;

	fMat44 CreateModelMatrix() const;

protected:

	fVec3 m_pos;
	fQuat m_rot;

	RenderNode* m_node; // The render-node is used by the manager to keep tabs on all rendered objects

	RenderMesh* m_mesh;
	Shader* m_shader;
};

