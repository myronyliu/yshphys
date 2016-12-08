#pragma once
#include "BoundingBox.h"
#include "RigidBody.h"

class BVNode;

struct BVNodePair
{
	BVNode* nodes[2];
};

// Bounding Volume Class
class BVNode
{
public:
	BVNode();
	virtual ~BVNode();

	AABB GetAABB() const;

	BVNode* Root();
	BVNode* LeftMostLeaf();
	BVNode* Sibling() const;

	std::vector<BVNode*> FindLeftToRightLeafOrder();
	std::vector<BVNodePair> FindIntersectingLeaves();

	bool SetAABB(const AABB& aabb);

protected:

	BVNode* m_parent;
	BVNode* m_left;
	BVNode* m_right;

	AABB m_AABB;

	RigidBody* m_content; // if not null then this BVNode is a leaf node
};

