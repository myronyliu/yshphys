#include "stdafx.h"
#include "RenderScene.h"
#include "YshMath.h"
#include <freeglut.h>

struct CubeMapFace
{
	CubeMapFace(GLenum iFace_, const fVec3& viewDir_, const fVec3& viewUp_) : iFace(iFace_), viewDir(viewDir_), viewUp(viewUp_) {}

	GLenum iFace;
	fVec3 viewDir;
	fVec3 viewUp;
};
static const CubeMapFace gCubeMapFaces[6] =
{
	CubeMapFace( GL_TEXTURE_CUBE_MAP_POSITIVE_X, fVec3( 1.0f,  0.0f,  0.0f), fVec3(0.0f, -1.0f,  0.0f) ),
	CubeMapFace( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, fVec3(-1.0f,  0.0f,  0.0f), fVec3(0.0f, -1.0f,  0.0f) ),
	CubeMapFace( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, fVec3( 0.0f,  1.0f,  0.0f), fVec3(0.0f,  0.0f,  1.0f) ),
	CubeMapFace( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, fVec3( 0.0f, -1.0f,  0.0f), fVec3(0.0f,  0.0f, -1.0f) ),
	CubeMapFace( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, fVec3( 0.0f,  0.0f,  1.0f), fVec3(0.0f, -1.0f,  0.0f) ),
	CubeMapFace( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, fVec3( 0.0f,  0.0f, -1.0f), fVec3(0.0f, -1.0f,  0.0f) )
};

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
	m_renderNodes = new RenderNode[MAX_RENDER_NODES];

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
	delete[] m_renderNodes;
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

void RenderScene::AddPointLight(const PointLight& pointLight)
{
	m_pointLights.push_back(pointLight);
}

void RenderScene::AttachCamera(Camera* camera)
{
	camera->SetViewport(&m_viewport);
}

void RenderScene::ShadowPass()
{
//	glCullFace(GL_FRONT);

	// Damn graphics library mumbo jumbo...
	// https://www.opengl.org/discussion_boards/showthread.php/189389-Shadow-Mapping-not-working-for-textures-not-size-of-screen
	// http://www.idevgames.com/forums/thread-2744.html

	GLuint shadowMapShaderProgram = m_shadowCubeMapShader.GetProgram();
	glUseProgram(shadowMapShaderProgram);

	const GLint projectionViewLoc = glGetUniformLocation(shadowMapShaderProgram, "projectionViewMatrices");
	const GLint modelLoc = glGetUniformLocation(shadowMapShaderProgram, "modelMatrix");
	const GLint pointLightPosLoc = glGetUniformLocation(shadowMapShaderProgram, "pointLightPos");
	const GLint pointLightFarLoc = glGetUniformLocation(shadowMapShaderProgram, "pointLightFarPlane");

	for (PointLight& pointLight : m_pointLights)
	{
		const fMat44 projMat = pointLight.shadowCubeMap.CreateProjectionMatrix();

		float projViewMats[16 * 6];
		float* ptr = projViewMats;

		DepthCubeMap& cubeMap = pointLight.shadowCubeMap;
		cubeMap.BindForWriting();
		glViewport(0, 0, pointLight.shadowCubeMap.m_width, pointLight.shadowCubeMap.m_width);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (unsigned int i = 0; i < 6; i++)
		{
			const fMat44 viewMat = fHomogeneousTransformation::CreateViewMatrix
			(
				pointLight.position,
				gCubeMapFaces[i].viewDir,
				gCubeMapFaces[i].viewUp
			);
			const fMat44 projViewMat = projMat*viewMat;
			projViewMat.Transpose().GetData(ptr);
			ptr += 16;
		}
		glUniformMatrix4fv(projectionViewLoc, 6, GL_FALSE, projViewMats);
		glUniform3f(pointLightPosLoc, pointLight.position.x, pointLight.position.y, pointLight.position.z);
		glUniform1f(pointLightFarLoc, pointLight.shadowCubeMap.m_far);

		const RenderNode* node = m_firstNode;
		while (node)
		{
			if (RenderObject* obj = node->GetRenderObject())
			{
				RenderMesh* mesh = obj->GetRenderMesh();
				glBindVertexArray(mesh->GetVAO());
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIBO());
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &(obj->CreateModelMatrix().Transpose()(0, 0)));
				glDrawElements(GL_TRIANGLES, 3 * mesh->GetNTriangles(), GL_UNSIGNED_INT, 0);
			}

			node = node->GetNext();
		}
	}
}

void RenderScene::RenderPass(Window* window)
{
	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int w, h;
	window->GetDimensions(w, h);

	glViewport(0, 0, w, h);

	const fMat44 viewMatrix = m_viewport.CreateViewMatrix();
	const fMat44 projectionMatrix = m_viewport.CreateProjectionMatrix();

	const RenderNode* node = m_firstNode;
	while (node)
	{
		if (RenderObject* obj = node->GetRenderObject())
		{
			RenderMesh* mesh = obj->GetRenderMesh();
			Shader* shader = obj->GetShader();

			for (PointLight pointLight : m_pointLights)
			{
				GLuint program = shader->GetProgram();
				glUseProgram(program);
				GLint shadowCubeMapTex = glGetUniformLocation(program, "shadowCubeMap");
				GLint pointLightPos = glGetUniformLocation(program, "pointLightPos");
				const GLint pointLightInt = glGetUniformLocation(program , "pointLightInt");
				GLint pointLightFar = glGetUniformLocation(program, "pointLightFarPlane");
				glUniform1i(shadowCubeMapTex, 0);
				glUniform3f(pointLightPos, pointLight.position.x, pointLight.position.y, pointLight.position.z);
				glUniform3f(pointLightInt, pointLight.intensity.x, pointLight.intensity.y, pointLight.intensity.z);
				glUniform1f(pointLightFar, pointLight.shadowCubeMap.m_far);
				pointLight.shadowCubeMap.BindForReading(GL_TEXTURE0);
			}

			obj->Draw(projectionMatrix, viewMatrix);
		}

		node = node->GetNext();
	}

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_debugRenderer.DrawObjects(m_viewport);
	m_debugRenderer.EvictObjects();
}

void RenderScene::DrawScene(Window* window)
{
	for (PointLight pointLight : m_pointLights)
	{
		m_debugRenderer.DrawBox(0.25f, 0.25f, 0.25f, pointLight.position, fQuat::Identity(), fVec3(1.0f, 1.0f, 0.0f), false, false);
	}

	ShadowPass();
	RenderPass(window);
}
