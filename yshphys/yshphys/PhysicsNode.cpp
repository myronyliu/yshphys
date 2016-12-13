#include "stdafx.h"
#include "PhysicsNode.h"
#include "PhysicsObject.h"
#include "PhysicsScene.h"

PhysicsNode::PhysicsNode() : m_physicsObject(nullptr), m_prev(nullptr), m_next(nullptr)
{
}
PhysicsNode::~PhysicsNode()
{
}
PhysicsObject* PhysicsNode::GetPhysicsObject() const
{
	return m_physicsObject;
}
PhysicsNode* PhysicsNode::GetPrev() const
{
	return m_next;
}
PhysicsNode* PhysicsNode::GetNext() const
{
	return m_prev;
}
void PhysicsNode::BindPhysicsObject(PhysicsObject* physicsObject)
{
	m_physicsObject = physicsObject;
	physicsObject->m_node = this;
}
void PhysicsNode::Remove()
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
void PhysicsNode::AppendTo(PhysicsNode* prev)
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
		if (PhysicsNode* next = prev->m_next)
		{
			next->m_prev = this;
			m_next = next;
		}
		prev->m_next = this;
		m_prev = prev;
	}
}
void PhysicsNode::PrependTo(PhysicsNode* next)
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
		if (PhysicsNode* prev = next->m_prev)
		{
			prev->m_next = this;
			m_prev = prev;
		}
		next->m_prev = this;
		m_next = next;
	}
}

FreedPhysicsNode::FreedPhysicsNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreedPhysicsNode::~FreedPhysicsNode()
{
}
