#include "stdafx.h"
#include "BVNode.h"
#include "RigidBody.h"

template <class T>
BVNode_t<T>::BVNode_t()
	: m_parent(nullptr), m_content(nullptr)
{
	m_children[0] = nullptr;
	m_children[1] = nullptr;
}

template <class T>
BVNode_t<T>::~BVNode_t()
{
}

template <class T>
BoundingBox BVNode_t<T>::GetAABB() const
{
	return m_AABB;
}

template <class T>
BVNode_t<T>* BVNode_t<T>::Root()
{
	BVNode_t<T>* current = this;
	BVNode_t<T>* parent = m_parent;
	while (parent != nullptr)
	{
		current = parent;
		parent = current->m_parent;
	}
	return current;
}

template <class T>
BVNode_t<T>* BVNode_t<T>::LeftMostLeaf()
{
	BVNode_t<T>* current = this;
	BVNode_t<T>* child = m_children[0];
	while (child != nullptr)
	{
		current = child;
		child = current->m_children[0];
	}
	return current;
}

template <class T>
std::vector<std::pair<BVNode_t<T>*, BVNode_t<T>*>> BVNode_t<T>::IntersectingLeaves() const
{
	return std::vector<std::pair<BVNode_t<T>*, BVNode_t<T>*>>();
}

template class BVNode_t<RigidBody>;