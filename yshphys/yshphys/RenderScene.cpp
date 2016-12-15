#include "stdafx.h"
#include "RenderScene.h"
#include "YshMath.h"
#include <freeglut.h>

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
	return m_prev;
}
RenderNode* RenderNode::GetNext() const
{
	return m_next;
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

FreedRenderNode::FreedRenderNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreedRenderNode::~FreedRenderNode()
{
}

RenderScene::RenderScene()
	: m_firstNode(nullptr)
{
	for (int i = 0; i < MAX_RENDER_NODES- 1; ++i)
	{
		m_renderNodes[i].PrependTo(&m_renderNodes[i + 1]);
	}

	for (int i = MAX_RENDER_NODES- 1; i > 0; --i)
	{
		FreedRenderNode freeNode;
		freeNode.m_node = &m_renderNodes[i];
		freeNode.m_precedingNode = &m_renderNodes[i - 1];
		m_freedNodeStack.push(freeNode);
	}
	FreedRenderNode freeNode;
	freeNode.m_node = &m_renderNodes[0];
	freeNode.m_precedingNode = nullptr;
	m_freedNodeStack.push(freeNode);
}


RenderScene::~RenderScene()
{
}

DebugRenderer& RenderScene::DebugDrawSystem()
{
	return m_debugRenderer;
}

void RenderScene::AddRenderObject(RenderObject* renderObject)
{
	if (!m_freedNodeStack.empty())
	{
		FreedRenderNode& freeNode = m_freedNodeStack.top();
		freeNode.m_node->BindRenderObject(renderObject);

		if (freeNode.m_precedingNode)
		{
			freeNode.m_node->AppendTo(freeNode.m_precedingNode);
		}
		else
		{
			freeNode.m_node->PrependTo(m_firstNode);
			m_firstNode = freeNode.m_node;
		}

		m_freedNodeStack.pop();
	}
}

void RenderScene::RemoveRenderObject(RenderObject* renderObject)
{
	if (RenderNode* node = renderObject->GetRenderNode())
	{
		if (node->GetPrev() == nullptr && node->GetNext() != nullptr)
		{
			m_firstNode = node->GetNext();
		}
		node->BindRenderObject(nullptr);
		FreedRenderNode freeNode;
		freeNode.m_precedingNode = node->GetPrev();
		freeNode.m_node = node;
		node->Remove();
		m_freedNodeStack.push(freeNode);
	}
}

void RenderScene::AttachCamera(Camera* camera)
{
	camera->SetViewport(&m_viewport);
}
void RenderScene::DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const fMat44 viewMatrix = m_viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = m_viewport.CreateProjectionMatrix();
	
	const RenderNode* node = m_firstNode;
	while (node)
	{
		if (RenderObject* obj = node->GetRenderObject())
		{
			RenderMesh* mesh = obj->GetRenderMesh();
			Shader* shader = obj->GetShader();
			if (mesh != nullptr && shader != nullptr)
			{
				const RenderObject* obj = node->GetRenderObject();
				const fMat44 modelMatrix = obj->CreateModelMatrix();
				const GLuint program = obj->GetShader()->GetProgram();
				const GLuint vao = mesh->GetVAO();
				const GLuint ibo = mesh->GetIBO();
				const unsigned int nTriangles = obj->GetRenderMesh()->GetNTriangles();
				glUseProgram(program);
				glBindVertexArray(vao);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
				const GLint projectionLoc = glGetUniformLocation(program, "projectionMatrix");
				const GLint viewLoc = glGetUniformLocation(program, "viewMatrix");
				const GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
				// Pass in the transpose because OpenGL likes to be all edgy with its
				// column major matrices while we are row major like everybody else.
				glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &(projectionMatrix.Transpose()(0, 0)));
				glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &(viewMatrix.Transpose()(0, 0)));
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &(modelMatrix.Transpose()(0, 0)));
				glDrawElements(GL_TRIANGLES, 3 * nTriangles, GL_UNSIGNED_INT, 0);
			}
		}

		node = node->GetNext();
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_debugRenderer.DrawObjects(m_viewport);
	m_debugRenderer.EvictObjects();
}
