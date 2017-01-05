#include "stdafx.h"
#include "DebugRenderer.h"

#include "Picker.h"

DebugRenderer::DebugRenderer()
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_xVBO);
	glGenBuffers(1, &m_nVBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_xVBO);
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		{
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}
	glBindVertexArray(0);
}


DebugRenderer::~DebugRenderer()
{
	glBindVertexArray(m_VAO);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_xVBO);
	glDeleteBuffers(1, &m_nVBO);
	glDeleteBuffers(1, &m_IBO);
}

void DebugRenderer::DrawObjects(const Viewport& viewport) const
{
	const fMat44 viewMatrix = viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = viewport.CreateProjectionMatrix();

	const GLuint program = m_shader.GetProgram();
	glUseProgram(program);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	for (int i = 0; i < m_objects.size(); ++i)
	{
		const DebugDrawData& data = m_objects[i];

		const fMat44 modelMatrix =
			HomogeneousTransformation_t<float>::CreateTranslation(data.pos)*
			HomogeneousTransformation_t<float>::CreateRotation(data.rot);

		glBindBuffer(GL_ARRAY_BUFFER, m_xVBO);
		glBufferData(GL_ARRAY_BUFFER, 3 * data.nVertices * sizeof(GL_FLOAT), data.vertices, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_nVBO);
		glBufferData(GL_ARRAY_BUFFER, 3 * data.nVertices * sizeof(GL_FLOAT), data.normals, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.nIndices * sizeof(GL_UNSIGNED_INT), data.indices, GL_DYNAMIC_DRAW);

		const GLint useNormalsLoc = glGetUniformLocation(program, "useNormals");
		const GLint colorLoc = glGetUniformLocation(program, "color");
		const GLint projectionLoc = glGetUniformLocation(program, "projectionMatrix");
		const GLint viewLoc = glGetUniformLocation(program, "viewMatrix");
		const GLint modelLoc = glGetUniformLocation(program, "modelMatrix");
		// Pass in the transpose because OpenGL likes to be all edgy with its
		// column major matrices while we are row major like everybody else.
		bool useNormals = (data.polygonType == GL_LINES) ? false : data.shaded;
		glUniform1i(useNormalsLoc, useNormals);
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

	data.nVertices = 2;

	data.vertices[0][0] = start.x;
	data.vertices[0][1] = start.y;
	data.vertices[0][2] = start.z;

	data.vertices[1][0] = end.x;
	data.vertices[1][1] = end.y;
	data.vertices[1][2] = end.z;

	data.indices[0] = 0;
	data.indices[1] = 1;

	data.polygonType = GL_LINES;
	data.nIndices = 2;

	m_objects.push_back(data);
}

void DebugRenderer::DrawCone(const fVec3& tip, const fVec3& base, const float& radius, const fVec3& color, bool shaded)
{
	DebugDrawData data;
	data.pos = base;
	data.color[0] = color.x;
	data.color[1] = color.y;
	data.color[2] = color.z;
	data.shaded = shaded;

	const fVec3 axis = tip - base;
	const float h = sqrtf(axis.Dot(axis));
	const fVec3 cross = fVec3(0.0f, 0.0f, 1.0f).Cross(axis.Scale(1.0f / h));
	const float sinAngSqr = cross.Dot(cross);
	if (sinAngSqr < 0.000001f)
	{
		data.rot = fQuat::Identity();
	}
	else
	{
		const float sinAng = sqrtf(sinAngSqr);
		const float cosAng = fVec3(0.0f, 0.0f, 1.0f).Dot(axis.Scale(1.0f / h));
		data.rot = fQuat(cross.Scale(1.0f / sinAng), atan2(sinAng, cosAng));
	}
	
	const int nBase = 32;
	data.nVertices = 1 + nBase;
	for (int i = 0; i < nBase; ++i)
	{
		float phi = (float)i * 2.0f * fPI / (float)nBase;
		data.vertices[i][0] = radius*cos(phi);
		data.vertices[i][1] = radius*sin(phi);
		data.vertices[i][2] = 0.0;

		data.indices[3 * i + 0] = nBase;
		data.indices[3 * i + 1] = i;
		data.indices[3 * i + 2] = (i + 1) % nBase;
	}
	data.vertices[nBase][0] = 0.0f;
	data.vertices[nBase][1] = 0.0f;
	data.vertices[nBase][2] = h;

	data.polygonType = GL_TRIANGLES;
	data.nIndices = 3 * nBase;

	m_objects.push_back(data);
}

void DebugRenderer::DrawTriangle(const fVec3& A, const fVec3& B, const fVec3& C, const fVec3& color, bool wireFrame, bool shaded)
{
	DebugDrawData data;
	data.pos = fVec3(0.0, 0.0, 0.0);
	data.rot = fQuat::Identity();
	data.color[0] = color.x;
	data.color[1] = color.y;
	data.color[2] = color.z;
	data.shaded = shaded;

	data.nVertices = 3;

	data.vertices[0][0] = A[0];
	data.vertices[0][1] = A[1];
	data.vertices[0][2] = A[2];

	data.vertices[1][0] = B[0];
	data.vertices[1][1] = B[1];
	data.vertices[1][2] = B[2];

	data.vertices[2][0] = C[0];
	data.vertices[2][1] = C[1];
	data.vertices[2][2] = C[2];

	if (wireFrame)
	{
		data.polygonType = GL_LINES;
		data.nIndices = 6;

		data.indices[0] = 0;
		data.indices[1] = 1;

		data.indices[2] = 1;
		data.indices[3] = 2;

		data.indices[4] = 2;
		data.indices[5] = 0;
	}
	else
	{
		data.polygonType = GL_TRIANGLES;
		data.nIndices = 3;
		data.indices[0] = 0;
		data.indices[1] = 1;
		data.indices[2] = 2;
	}
	m_objects.push_back(data);
}

void DebugRenderer::DrawPolygon(const fVec3* verts, int nVerts, const fVec3& color, bool wireFrame, bool shaded)
{
	DrawPolygon(verts, nVerts, fVec3(0.0, 0.0, 0.0), fQuat::Identity(), color, wireFrame, shaded);
}

void DebugRenderer::DrawPolygon(const fVec3* verts, int nVerts, const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame, bool shaded)
{
	DebugDrawData data;
	data.pos = pos;
	data.rot = rot;
	data.color[0] = color.x;
	data.color[1] = color.y;
	data.color[2] = color.z;
	data.shaded = shaded;

	data.nVertices = nVerts;

	for (int i = 0; i < nVerts; ++i)
	{
		data.vertices[i][0] = verts[i][0];
		data.vertices[i][1] = verts[i][1];
		data.vertices[i][2] = verts[i][2];
	}

	if (wireFrame)
	{
		data.polygonType = GL_LINES;
		data.nIndices = 2 * nVerts;

		for (int i = 0; i < nVerts; ++i)
		{
			data.indices[2 * i + 0] = (i + 0) % nVerts;
			data.indices[2 * i + 1] = (i + 1) % nVerts;
		}
	}
	else
	{
		data.polygonType = GL_TRIANGLES;
		data.nIndices = 3 * (nVerts - 2);

		int iA = 0;
		int iD = nVerts - 1;

		int iB = 1;
		int iC = nVerts - 2;

		int iTriangle = 0;

		while (iB < iC)
		{
			data.indices[3 * iTriangle + 0] = iA;
			data.indices[3 * iTriangle + 1] = iB;
			data.indices[3 * iTriangle + 2] = iC;
			iTriangle++;

			data.indices[3 * iTriangle + 0] = iA;
			data.indices[3 * iTriangle + 1] = iC;
			data.indices[3 * iTriangle + 2] = iD;
			iTriangle++;

			iA++;
			iB++;
			iC--;
			iD--;
		}

		if (iB == iC)
		{
			data.indices[3 * iTriangle + 0] = iA;
			data.indices[3 * iTriangle + 1] = iC;
			data.indices[3 * iTriangle + 2] = iD;
		}

		fVec3 n(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < nVerts; ++i)
		{
			const fVec3& u = verts[(i + 0) % nVerts];
			const fVec3& v = verts[(i + 1) % nVerts];

			n.x += (u.y - v.y)*(u.z + v.z);
			n.y += (u.z - v.z)*(u.x + v.x);
			n.z += (u.x - v.x)*(u.y + v.y);
		}
		n = n.Scale(1.0f / sqrtf(n.Dot(n)));

		for (int i = 0; i < nVerts; ++i)
		{
			data.normals[i][0] = n[0];
			data.normals[i][1] = n[1];
			data.normals[i][2] = n[2];
		}
	}
	m_objects.push_back(data);

}
void DebugRenderer::DrawBox(float halfDimX, float halfDimY, float halfDimZ, const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame, bool shaded)
{
	if (wireFrame)
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

		data.polygonType = GL_LINES;
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

		m_objects.push_back(data);
	}
	else
	{
		fVec3 corners[2][2][2];

		for (int k = 0; k < 2; ++k)
		{
			for (int j = 0; j < 2; ++j)
			{
				for (int i = 0; i < 2; ++i)
				{
					corners[i][j][k].x = float(2 * i - 1)*halfDimX;
					corners[i][j][k].y = float(2 * j - 1)*halfDimY;
					corners[i][j][k].z = float(2 * k - 1)*halfDimZ;
				}
			}
		}

		fVec3 rect[4];

		rect[0] = corners[0][0][0];
		rect[1] = corners[0][0][1];
		rect[2] = corners[0][1][1];
		rect[3] = corners[0][1][0];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);
		rect[0] = corners[1][0][0];
		rect[1] = corners[1][1][0];
		rect[2] = corners[1][1][1];
		rect[3] = corners[1][0][1];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);

		rect[0] = corners[0][0][0];
		rect[3] = corners[1][0][0];
		rect[2] = corners[1][0][1];
		rect[1] = corners[0][0][1];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);
		rect[0] = corners[0][1][0];
		rect[3] = corners[0][1][1];
		rect[2] = corners[1][1][1];
		rect[1] = corners[1][1][0];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);

		rect[0] = corners[0][0][0];
		rect[1] = corners[0][1][0];
		rect[2] = corners[1][1][0];
		rect[3] = corners[1][0][0];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);
		rect[0] = corners[0][0][1];
		rect[1] = corners[1][0][1];
		rect[2] = corners[1][1][1];
		rect[3] = corners[0][1][1];
		DrawPolygon(rect, 4, pos, rot, color, false, shaded);
	}
}

void DebugRenderer::DrawBVTree(const BVTree& tree, const fVec3& color)
{
	const BVNode* root = tree.Root();
	assert(root->GetParent() == nullptr);
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

		const fVec3 aabbCenter(
			float(aabb.max.x + aabb.min.x)*0.5f,
			float(aabb.max.y + aabb.min.y)*0.5f,
			float(aabb.max.z + aabb.min.z)*0.5f);

		DrawBox(
			float(aabb.max.x - aabb.min.x)*0.5f,
			float(aabb.max.y - aabb.min.y)*0.5f,
			float(aabb.max.z - aabb.min.z)*0.5f,
			aabbCenter, fQuat::Identity(), color, true
		);

		if (!node->IsLeaf())
		{
			nodeStack.push(node->GetLeftChild());
			nodeStack.push(node->GetRightChild());
		}
	}
}

void DebugRenderer::DrawPicker(const Picker& picker, const fVec3& color)
{
	if (const RigidBody* obj = picker.GetPickedObject())
	{
		const dVec3 x = obj->GetPosition();
		const dQuat q = obj->GetRotation();

		const dVec3 grabPos = x + q.Transform(picker.GetGrabOffset());

		const dVec3 pickerPos = picker.GetPosition();

		DrawLine(grabPos, pickerPos, color);
		DrawBox(0.2f, 0.2f, 0.2f, grabPos, fQuat::Identity(), color, false, false);
//		const dVec3 coneAxis = pickerPos - grabPos;
//		const double coneRadius = sqrtf(coneAxis.Dot(coneAxis)) / 16.0f;
//		DrawCone(grabPos, pickerPos, coneRadius, color);
	}
}