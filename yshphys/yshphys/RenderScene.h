#pragma once

#include "RenderObject.h"
#include "Shader.h"
#include "Shader_FlatUniformColor.h"
#include "Shader_DepthCube.h"
#include "Shader_DepthPerspective.h"
#include "Viewport.h"
#include "Window.h"
#include "Camera.h"
#include "DebugRenderer.h"
#include "Light.h"
#include "DepthMap.h"
#include <glew.h>

#define MAX_RENDER_NODES 1024

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

	bool Init();

	DebugRenderer& DebugDrawSystem();

	void AddRenderObject(RenderObject* renderObject);
	void RemoveRenderObject(RenderObject* renderObject);

	void AddPointLight(const PointLight& pointLight);

	void DrawScene(Window* window);

	void AttachCamera(Camera* camera);

	Viewport m_viewport;
protected:

	void ShadowPass();
	void RenderDepthFromEye(Window* window);
	void RenderPass(Window* window);

	std::stack<FreedRenderNode> m_freedNodeStack;
	RenderNode* m_renderNodes;
	RenderNode* m_firstNode;

	DebugRenderer m_debugRenderer;

	std::vector<PointLight> m_pointLights;

	Shader_DepthCube m_shadowCubeMapShader;
	Shader_DepthPerspective m_depthPerspectiveShader;

	DepthMap m_depthMap; // from the perspective of m_viewport
};

