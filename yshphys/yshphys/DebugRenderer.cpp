#include "stdafx.h"
#include "DebugRenderer.h"


DebugRenderer::DebugRenderer()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0);
}


DebugRenderer::~DebugRenderer()
{
	glBindVertexArray(m_VAO);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);
}

void DebugRenderer::DrawObjects(const Viewport& viewport) const
{
	const fMat44 viewMatrix = viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = viewport.CreateProjectionMatrix();

	const GLuint program = m_shader.GetProgram();
	glUseProgram(program);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	for (int i = 0; i < m_objects.size(); ++i)
	{
		const DebugDrawData& data = m_objects[i];

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
}

void DebugRenderer::EvictObjects()
{
	m_objects.clear();
}

void DebugRenderer::DrawLine(const fVec3& start, const fVec3& end, const fVec3& color)
{
	DebugDrawData data;
	data.pos = fVec3(0.0f, 0.0f, 0.0f);
	data.rot = fQuat::Identity();
	data.color[0] = color.x;
	data.color[1] = color.y;
	data.color[2] = color.z;

	data.vertices[0][0] = start.x;
	data.vertices[0][1] = start.y;
	data.vertices[0][2] = start.z;

	data.vertices[1][0] = end.x;
	data.vertices[1][1] = end.y;
	data.vertices[1][2] = end.z;

	data.indices[0] = 0;
	data.indices[1] = 1;

	data.polygonType = GL_LINES;
	data.nVertsPerPoly = 2;
	data.nIndices = 2;
}

void DebugRenderer::DrawBox(float halfDimX, float halfDimY, float halfDimZ, const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame)
{
	DebugDrawData data;
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
	m_objects.push_back(data);
}

void DebugRenderer::DrawBVTree(const BVTree& tree, const fVec3& color)
{
	const BVNode* root = tree.Root();
	if (!root)
	{
		return;
	}

	std::stack<const BVNode*> nodeStack;
	nodeStack.push(root);
	while (!nodeStack.empty())
	{
		const BVNode* node = nodeStack.top();
		nodeStack.pop();

		const AABB aabb = node->GetAABB();
		DrawBox(
			float(aabb.max.x - aabb.min.x),
			float(aabb.max.y - aabb.min.y),
			float(aabb.max.z - aabb.min.z),
			fVec3(0.0f, 0.0f, 0.0f), fQuat::Identity(), color, true
		);

		nodeStack.push(node->GetLeftChild());
		nodeStack.push(node->GetRightChild());
	}
}