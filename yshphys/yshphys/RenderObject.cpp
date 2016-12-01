#include "stdafx.h"
#include "RenderObject.h"


RenderObject::RenderObject() : m_node(nullptr)
{
}

RenderObject::~RenderObject()
{
}

RenderMesh* RenderObject::GetRenderMesh() const
{
	return m_mesh;
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

