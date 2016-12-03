#include "stdafx.h"
#include "RenderScene.h"
#include <freeglut.h>

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
			m_next = next;
		}
		prev->m_next = this;
		m_prev = prev;
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
			m_prev = prev;
		}
		next->m_prev = this;
		m_next = next;
	}
}

FreeRenderNode::FreeRenderNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreeRenderNode::~FreeRenderNode()
{
}

RenderScene::RenderScene()
	: m_rootNode(nullptr)
{
	m_freeNodeStack.reserve(MAX_RENDER_MESHES);

	RenderNode* buffer = new RenderNode[MAX_RENDER_MESHES];

	for (int i = 0; i < MAX_RENDER_MESHES - 1; ++i)
	{
		buffer[i].PrependTo(&buffer[i + 1]);
	}

	for (int i = MAX_RENDER_MESHES - 1; i > 0; --i)
	{
		FreeRenderNode freeNode;
		freeNode.m_node = &buffer[i];
		freeNode.m_precedingNode = &buffer[i - 1];
		m_freeNodeStack.push_back(freeNode);
	}
	FreeRenderNode freeNode;
	freeNode.m_node = &buffer[0];
	freeNode.m_precedingNode = nullptr;
	m_freeNodeStack.push_back(freeNode);
}


RenderScene::~RenderScene()
{
}

void RenderScene::AddRenderObject(RenderObject* renderObject)
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

void RenderScene::RemoveRenderObject(RenderObject* renderObject)
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

void RenderScene::DrawScene() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const fMat44 viewMatrix = m_viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = m_viewport.CreateProjectionMatrix();

	const RenderNode* node = m_rootNode;
	while (node)
	{
		const RenderObject* obj = node->GetRenderObject();
		const fMat44 modelMatrix = obj->CreateModelMatrix();
		const GLuint program = obj->GetShader()->GetProgram();
		const GLuint vao = obj->GetRenderMesh()->GetVAO();
		const unsigned int nTriangles = obj->GetRenderMesh()->GetNTriangles();
		glUseProgram(program);
		glBindVertexArray(vao);
		const GLint projectionLoc = glGetUniformLocation(program, "projectionMatrix");
		const GLint viewLoc = glGetUniformLocation(program, "viewMatrix");
		const GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
		// Pass in the transpose because OpenGL likes to be all edgy with its
		// column major matrices while we are row major like everybody else.
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &(projectionMatrix.Transpose()(0, 0)));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &(viewMatrix.Transpose()(0, 0)));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &(modelMatrix.Transpose()(0, 0)));
		glDrawArrays(GL_TRIANGLES, 0, 3 * nTriangles);

		node = node->GetNext();
	}

	glUseProgram(0);
	glBindVertexArray(0);

	m_window->UpdateGLRender();
}