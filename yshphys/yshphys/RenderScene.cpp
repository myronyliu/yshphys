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

FreedRenderNode::FreedRenderNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreedRenderNode::~FreedRenderNode()
{
}

RenderScene::RenderScene()
	: m_rootNode(nullptr)
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

	// Do some Single Draw mumbo jumbo
	glGenVertexArrays(1, &m_singleDrawVAO);
	glGenBuffers(1, &m_singleDrawVBO);
	glGenBuffers(1, &m_singleDrawIBO);

	glBindVertexArray(m_singleDrawVAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_singleDrawVBO);
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0);
}


RenderScene::~RenderScene()
{
	glBindVertexArray(m_singleDrawVAO);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &m_singleDrawVAO);
	glDeleteBuffers(1, &m_singleDrawVBO);
	glDeleteBuffers(1, &m_singleDrawIBO);
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
			freeNode.m_node->PrependTo(m_rootNode);
			m_rootNode = freeNode.m_node;
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
			m_rootNode = node->GetNext();
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

void RenderScene::DrawAndEvictSingles()
{
	const fMat44 viewMatrix = m_viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = m_viewport.CreateProjectionMatrix();

	const GLuint program = m_singleDrawShader.GetProgram();
	glUseProgram(program);
	glBindVertexArray(m_singleDrawVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_singleDrawVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_singleDrawIBO);

	for (int i = 0; i < m_singleDraws.size(); ++i)
	{
		SingleDrawData& data = m_singleDraws[i];

		const fMat44 modelMatrix =
			HomogeneousTransformation_t<float>::CreateTranslation(data.pos)*
			HomogeneousTransformation_t<float>::CreateRotation(data.rot);

		glBufferData(GL_ARRAY_BUFFER, 3 * data.nVertices * sizeof(GL_FLOAT), data.vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.nIndices * sizeof(GL_UNSIGNED_INT), data.indices, GL_DYNAMIC_DRAW);

		const GLint colorLoc = glGetUniformLocation(program, "color");
		const GLint projectionLoc = glGetUniformLocation(program, "projectionMatrix");
		const GLint viewLoc = glGetUniformLocation(program, "viewMatrix");
		const GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
		// Pass in the transpose because OpenGL likes to be all edgy with its
		// column major matrices while we are row major like everybody else.
		glUniform3fv(colorLoc, 1, data.color);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &(projectionMatrix.Transpose()(0, 0)));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &(viewMatrix.Transpose()(0, 0)));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &(modelMatrix.Transpose()(0, 0)));
		glDrawElements(data.polygonType, data.nIndices, GL_UNSIGNED_INT, 0);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	m_singleDraws.clear();
}

void RenderScene::DrawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const fMat44 viewMatrix = m_viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = m_viewport.CreateProjectionMatrix();
	
	const RenderNode* node = m_rootNode;
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

	DrawAndEvictSingles();

	m_window->UpdateGLRender();
}

void RenderScene::DrawBox(const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame, float halfDimX, float halfDimY, float halfDimZ)
{
	SingleDrawData data;
	data.pos = pos;
	data.rot = rot;
	data.color[0] = color.x;
	data.color[1] = color.y;
	data.color[2] = color.z;

	auto iVertex = [](int i, int j, int k)
	{
		return i + 2 * j + 4 * k;
	};

	data.nVertices = 8;

	for (int k = 0; k < 2; ++k)
	{
		for (int j = 0; j < 2; ++j)
		{
			for (int i = 0; i < 2; ++i)
			{
				const int idx = iVertex(i, j, k);
				data.vertices[idx][0] = float(2 * i - 1)*halfDimX;
				data.vertices[idx][1] = float(2 * j - 1)*halfDimY;
				data.vertices[idx][2] = float(2 * k - 1)*halfDimZ;
			}
		}
	}

	if (wireFrame)
	{
		data.polygonType = GL_LINES;
		data.nVertsPerPoly = 2;
		data.nIndices = 24;

		data.indices[0] = iVertex(0, 0, 0);
		data.indices[1] = iVertex(1, 0, 0);

		data.indices[2] = iVertex(1, 0, 0);
		data.indices[3] = iVertex(1, 1, 0);

		data.indices[4] = iVertex(1, 1, 0);
		data.indices[5] = iVertex(0, 1, 0);

		data.indices[6] = iVertex(0, 1, 0);
		data.indices[7] = iVertex(0, 0, 0);

		///////////////////////////////////

		data.indices[8] = iVertex(0, 0, 1);
		data.indices[9] = iVertex(1, 0, 1);

		data.indices[10] = iVertex(1, 0, 1);
		data.indices[11] = iVertex(1, 1, 1);

		data.indices[12] = iVertex(1, 1, 1);
		data.indices[13] = iVertex(0, 1, 1);

		data.indices[14] = iVertex(0, 1, 1);
		data.indices[15] = iVertex(0, 0, 1);

		////////////////////////////////////

		data.indices[16] = iVertex(0, 0, 0);
		data.indices[17] = iVertex(0, 0, 1);

		data.indices[18] = iVertex(1, 0, 0);
		data.indices[19] = iVertex(1, 0, 1);

		data.indices[20] = iVertex(1, 1, 0);
		data.indices[21] = iVertex(1, 1, 1);

		data.indices[22] = iVertex(0, 1, 0);
		data.indices[23] = iVertex(0, 1, 1);
	}
	else
	{
		data.polygonType = GL_TRIANGLES;
		data.nVertsPerPoly = 3;
		data.nIndices = 36;
	}
	m_singleDraws.push_back(data);
}