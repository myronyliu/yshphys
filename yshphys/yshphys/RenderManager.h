#pragma once

#include "RenderObject.h"
#include "Renderer.h"
#include <glew.h>

class RenderNode
{
	// Only the render manager can instantiate RenderNodes. This way it is safe to assume that all nodes come from the RenderManager's free list.
	friend class RenderManager;
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

class FreeRenderNode
{
public:
	FreeRenderNode();
	virtual ~FreeRenderNode();

	RenderNode* m_node;

	// Keep track of which node m_freeNode proceeded before it was removed from the manager.
	// When we reuse m_freeNode, we will append it to m_appensionPoint.
	// As a consequence, observe that we must reuse free nodes in REVERSE order in which they were evicted
	// lest we attempt to append to a node that may have already been evicted.
	// (i.e. we must maintain free nodes in a STACK, as opposed to in a queue)
	RenderNode* m_precedingNode;
};

class RenderManager
{
public:
	RenderManager();
	virtual ~RenderManager();

	void AddRenderObject(RenderObject* renderObject);
	void RemoveRenderObject(RenderObject* renderObject);


protected:

	std::vector<FreeRenderNode> m_freeNodeStack;
	RenderNode* m_rootNode;

	Renderer* m_renderer;
};

