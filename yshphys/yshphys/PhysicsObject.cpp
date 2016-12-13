#include "stdafx.h"
#include "PhysicsObject.h"


PhysicsObject::PhysicsObject() : m_node(nullptr)
{
	m_AABB.min = dVec3(0.0, 0.0, 0.0);
	m_AABB.max = dVec3(0.0, 0.0, 0.0);
}


PhysicsObject::~PhysicsObject()
{
}

PhysicsNode* PhysicsObject::GetPhysicsNode() const
{
	return m_node;
}

AABB PhysicsObject::GetAABB() const
{
	return m_AABB;
}
