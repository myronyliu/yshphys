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

void RenderObject::Draw(const fMat44& projectionMatrix, const fMat44& viewMatrix) const
{
	if (m_mesh != nullptr && m_shader != nullptr)
	{
		const GLuint program = GetShader()->GetProgram();
		const fMat44 modelMatrix = CreateModelMatrix();
		const GLuint vao = m_mesh->GetVAO();
		const GLuint ibo = m_mesh->GetIBO();
		const unsigned int nTriangles = GetRenderMesh()->GetNTriangles();
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
