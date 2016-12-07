#pragma once
#include "BoundingBox.h"

// Bounding Volume Class
template <class T> class BVNode_t
{
public:
	BVNode_t();
	virtual ~BVNode_t();

	BoundingBox GetAABB() const;

	BVNode_t<T>* Root();
	BVNode_t<T>* LeftMostLeaf();

	std::vector<std::pair<BVNode_t<T>*, BVNode_t<T>*>> IntersectingLeaves() const;

protected:

	BVNode_t<T>* m_parent;
	BVNode_t<T>* m_children[2];

	BoundingBox m_AABB;

	T* m_content; // if not null then this BVNode is a leaf node
};

