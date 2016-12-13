#include "stdafx.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "PhysicsObject.h"


GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}
void GameObject::SetRenderObject(RenderObject* renderObject)
{
	m_renderObject = renderObject;
}
void GameObject::SetPhysicsObject(PhysicsObject* physicsObject)
{
	m_physicsObject = physicsObject;
}
RenderObject* GameObject::GetRenderObject() const
{
	return m_renderObject;
}
PhysicsObject* GameObject::GetPhysicsObject() const
{
	return m_physicsObject;
}

void GameObject::PropagatePhysicsTransform() const
{
	if (m_physicsObject && m_renderObject)
	{
		const dQuat q0 = m_physicsObject->GetRotation();
		const dVec3 x0 = m_physicsObject->GetPosition();
		const fQuat q1 = m_renderObject->GetRotation();
		const fVec3 x1 = m_renderObject->GetPosition();
		m_renderObject->SetRotation(q0*q1);
		m_renderObject->SetPosition(x0 + q0.Transform(x1));
	}
}