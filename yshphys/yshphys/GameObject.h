#pragma once
//#include "RenderObject.h"
//#include "PhysicsObject.h"

#include "YshMath.h"

class RenderObject;
class PhysicsObject;

class GameObject
{
	friend class GameNode;
public:
	GameObject();
	virtual ~GameObject();

	GameNode* GetGameNode() const;

	void SetRenderObject(RenderObject* renderObject);
	void SetPhysicsObject(PhysicsObject* physicsObject);

	RenderObject* GetRenderObject() const;
	PhysicsObject* GetPhysicsObject() const;

	void PropagatePhysicsTransform() const;

	fVec3 m_renderPosOffset;
	fQuat m_renderRotOffset;

protected:
	RenderObject* m_renderObject;
	PhysicsObject* m_physicsObject;

	GameNode* m_node;
};

