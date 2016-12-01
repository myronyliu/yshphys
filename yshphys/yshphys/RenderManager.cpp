#include "stdafx.h"
#include "RenderManager.h"

#define MAX_RENDER_MESHES 1024

RenderNode::RenderNode() : m_renderObject(nullptr), m_prev(nullptr), m_next(nullptr)
{
}
RenderNode::~RenderNode()
{
}
RenderObject* RenderNode::GetRenderObject() const
{
	return m_renderObject;
}
RenderNode* RenderNode::GetPrev() const
{
	return m_next;
}
RenderNode* RenderNode::GetNext() const
{
	return m_prev;
}
void RenderNode::BindRenderObject(RenderObject* renderObject)
{
	m_renderObject = renderObject;
	renderObject->m_node = this;
}
void RenderNode::Remove()
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
		m_prev = nullptr;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
		m_next = nullptr;
	}
}
void RenderNode::AppendTo(RenderNode* prev)
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
	if (prev)
	{
		if (RenderNode* next = prev->m_next)
		{
			next->m_prev = this;
		}
		prev->m_next = this;
	}
}
void RenderNode::PrependTo(RenderNode* next)
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
	if (next)
	{
		if (RenderNode* prev = next->m_prev)
		{
			prev->m_next = this;
		}
		next->m_prev = this;
	}
}
FreeRenderNode::FreeRenderNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreeRenderNode::~FreeRenderNode()
{
}

RenderManager::RenderManager()
	: m_rootNode(nullptr)
{
	m_freeNodeStack.reserve(MAX_RENDER_MESHES);

	RenderNode* buffer = new RenderNode[MAX_RENDER_MESHES];

	for (int i = 0; i < MAX_RENDER_MESHES - 1; ++i)
	{
		buffer[i].PrependTo(&buffer[i + 1]);
	}

	for (int i = MAX_RENDER_MESHES - 1; i > 0; ++i)
	{
		FreeRenderNode freeNode;
		freeNode.m_node = &buffer[i];
		freeNode.m_precedingNode = &buffer[i - 1];
		m_freeNodeStack.push_back(freeNode);
	}
	FreeRenderNode freeNode;
	freeNode.m_node = &buffer[0];
	freeNode.m_precedingNode = nullptr;
}


RenderManager::~RenderManager()
{
}

void RenderManager::AddRenderObject(RenderObject* renderObject)
{
	if (!m_freeNodeStack.empty())
	{
		FreeRenderNode& freeNode = m_freeNodeStack.back();
		freeNode.m_node->BindRenderObject(renderObject);

		if (freeNode.m_precedingNode)
		{
			freeNode.m_node->AppendTo(freeNode.m_precedingNode);
		}
		else
		{
			freeNode.m_node->PrependTo(m_rootNode);
			m_rootNode = freeNode.m_node;
		}

		m_freeNodeStack.pop_back();
	}
}

void RenderManager::RemoveRenderObject(RenderObject* renderObject)
{
	if (RenderNode* node = renderObject->GetRenderNode())
	{
		if (node->GetPrev() == nullptr && node->GetNext() != nullptr)
		{
			m_rootNode = node->GetNext();
		}
		node->BindRenderObject(nullptr);
		FreeRenderNode freeNode;
		freeNode.m_precedingNode = node->GetPrev();
		freeNode.m_node = node;
		node->Remove();
		m_freeNodeStack.push_back(freeNode);
	}
}