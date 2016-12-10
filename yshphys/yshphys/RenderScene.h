#pragma once

#include "RenderObject.h"
#include "Shader.h"
#include "Shader_FlatUniformColor.h"
#include "Viewport.h"
#include "Window.h"
#include "Camera.h"
#include <glew.h>

#define MAX_RENDER_NODES 1024
#define SINGLE_DRAW_VERTEX_BUFFER_SIZE 64
#define SINGLE_DRAW_INDEX_BUFFER_SIZE 256

class RenderNode
{
	// Only the render manager can instantiate RenderNodes. This way it is safe to assume that all nodes come from the RenderScene's free list.
	friend class RenderScene;
public:
	RenderObject* GetRenderObject() const;
	RenderNode* GetPrev() const;
	RenderNode* GetNext() const;

private:
	RenderNode();
	virtual ~RenderNode();

	void BindRenderObject(RenderObject* renderObject);
	void Remove();
	void AppendTo(RenderNode* prev);
	void PrependTo(RenderNode* next);

	RenderObject* m_renderObject;

	RenderNode* m_next;
	RenderNode* m_prev;
};

class FreedRenderNode
{
public:
	FreedRenderNode();
	virtual ~FreedRenderNode();

	RenderNode* m_node;

	// Keep track of which node m_freeNode proceeded before it was removed from the manager.
	// When we reuse m_freeNode, we will append it to m_appensionPoint.
	// As a consequence, observe that we must reuse free nodes in REVERSE order in which they were evicted
	// lest we attempt to append to a node that may have already been evicted.
	// (i.e. we must maintain free nodes in a STACK, as opposed to in a queue)
	RenderNode* m_precedingNode;
};

class RenderScene
{
public:
	RenderScene();
	virtual ~RenderScene();

	void AddRenderObject(RenderObject* renderObject);
	void RemoveRenderObject(RenderObject* renderObject);

	void DrawScene();

	void AttachCamera(Camera* camera);

	// The following functions are for one time draws. The caller has no control over the shader. Everything is drawn with flat colors.
	// Intended for debugging and should be used sparingly as these shuttle memory to the GPU each time they are called.
	void DrawBox(const fVec3& pos, const fQuat& rot, const fVec3& color, bool wireFrame, float halfDimX, float halfDimY, float halfDimZ);

	Window* m_window; // The window into which the rendered scene is displayed
protected:

	void DrawAndEvictSingles();

	struct SingleDrawData
	{
		float vertices[SINGLE_DRAW_VERTEX_BUFFER_SIZE][3];
		unsigned int indices[SINGLE_DRAW_INDEX_BUFFER_SIZE];
		unsigned int nVertices = 0;
		unsigned int nIndices= 0;

		unsigned int nVertsPerPoly = 0;

		GLenum polygonType = GL_TRIANGLES;

		fVec3 pos = fVec3(0.0f, 0.0f, 0.0f);
		fQuat rot = fQuat(0.0f, 0.0f, 0.0f, 1.0f);
		float color[3] = { 1.0f, 1.0f, 1.0f };
	};

	std::vector<SingleDrawData> m_singleDraws;
	GLuint m_singleDrawVAO;
	GLuint m_singleDrawVBO;
	GLuint m_singleDrawIBO;
	Shader_FlatUniformColor m_singleDrawShader;

	std::stack<FreedRenderNode> m_freedNodeStack;
	RenderNode m_renderNodes[MAX_RENDER_NODES];
	RenderNode* m_rootNode;

	Viewport m_viewport;
};

