#include "stdafx.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "RigidBody.h"


GameObject::GameObject() : m_renderPosOffset(0.0, 0.0, 0.0), m_renderRotOffset(fQuat::Identity())
{
}
GameObject::~GameObject()
{
}
GameNode* GameObject::GetGameNode() const
{
	return m_node;
}
void GameObject::SetRenderObject(RenderObject* renderObject)
{
	m_renderObject = renderObject;
}
void GameObject::SetPhysicsObject(RigidBody* physicsObject)
{
	m_physicsObject = physicsObject;
}
RenderObject* GameObject::GetRenderObject() const
{
	return m_renderObject;
}
RigidBody* GameObject::GetPhysicsObject() const
{
	return m_physicsObject;
}

void GameObject::PropagatePhysicsTransform() const
{
	if (m_physicsObject && m_renderObject)
	{
		const dQuat q0 = m_physicsObject->GetRotation();
		const dVec3 x0 = m_physicsObject->GetPosition();
		const fQuat& q1 = m_renderRotOffset;
		const fVec3& x1 = m_renderPosOffset;
		m_renderObject->SetRotation(q0*q1);
		m_renderObject->SetPosition(x0 + q0.Transform(x1));
	}
}