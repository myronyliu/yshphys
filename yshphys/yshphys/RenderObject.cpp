#include "stdafx.h"
#include "RenderObject.h"


RenderObject::RenderObject() :
	m_node(nullptr),
	m_pos(0.0f, 0.0f, 0.0f),
	m_rot(0.0f, 0.0f, 0.0f, 1.0f)
{
}

RenderObject::~RenderObject()
{
}

RenderMesh* RenderObject::GetRenderMesh() const
{
	return m_mesh;
}
Shader* RenderObject::GetShader() const
{
	return m_shader;
}
RenderNode* RenderObject::GetRenderNode() const
{
	return m_node;
}
fVec3 RenderObject::GetPosition() const
{
	return m_pos;
}
fQuat RenderObject::GetRotation() const
{
	return m_rot;
}
void RenderObject::SetPosition(const fVec3& pos)
{
	m_pos = pos;
}
void RenderObject::SetRotation(const fQuat& rot)
{
	m_rot = rot;
}
void RenderObject::SetRenderMesh(RenderMesh* mesh)
{
	m_mesh = mesh;
}
void RenderObject::SetShader(Shader* shader)
{
	m_shader = shader;
}
fMat44 RenderObject::CreateModelMatrix() const
{
	const fMat44 T(fHomogeneousTransformation::CreateTranslation(m_pos));
	const fMat44 R(fHomogeneousTransformation::CreateRotation(m_rot));
	return T*R;
}