#include "stdafx.h"
#include "PhysicsObject.h"
#include "PhysicsScene.h"


PhysicsObject::PhysicsObject() : m_node(nullptr), m_awake(true)
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