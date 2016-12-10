#pragma once
#include "BVNode.h"

#define MAX_BV_NODES (1<<12)

struct FreedBVNode
{
	unsigned int m_iNode;
	unsigned int m_iPrecedingNode;
};

class BVTree
{
	friend class BVNode;
public:
	BVTree();
	virtual ~BVTree();

	const BVNode* Root() const;

protected:

	// Adds a new root, adds a leaf as the sibling of the old root, and sets the old root and the new leaf as the children of the new root.
	// Return value is false if we are out of freeNodes
	bool LeftInsertNewLeaf(const AABB& aabb, BVNodeContent* content);
	bool DeepInsertNewLeaf(const AABB& aabb, BVNodeContent* content);

	void PopFreeNode();
	void PushFreeNode(unsigned int iNode);

	std::stack<FreedBVNode> m_iFreeNodes;
	unsigned int m_iRoot;
	unsigned int m_iFirst; // this is for iterating through the linked list. Has nothing to do with tree structure.

	BVNode m_nodes[MAX_BV_NODES];
};

