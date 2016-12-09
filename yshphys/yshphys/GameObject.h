#pragma once
#include "RenderObject.h"
#include "PhysicsObject.h"
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	void SetRenderObject(RenderObject* renderObject);
	void SetPhysicsObject(PhysicsObject* physicsObject);

protected:
	RenderObject* m_renderObject;
	PhysicsObject* m_physicsObject;
};

