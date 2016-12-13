#pragma once
#include "BoundingBox.h"

#define INVALID_BVNODEINDEX -1

//////////////////////////////////////////////////////////////////////////
////  For consistency, we will use POINTERS for the tree connectivity
////  In contrast, we will use INDICES for active/freed node maintenance
//////////////////////////////////////////////////////////////////////////

class BVTree;
class BVNode;

struct BVNodePair
{
	BVNode* nodes[2];
};

class BVNodeContent
{
	friend class BVNode;
public:
	BVNodeContent();
	BVNode* GetBVNode() const;
protected:
	
	BVNode* m_bvNode;
};

// Bounding Volume Class
class BVNode
{
	friend class BVTree;
public:
	AABB GetAABB() const;
	BVNodeContent* GetContent() const;

	// Our getters should return const pointers. A BVNode should be able to traverse the tree and query data. However, it should not be able to "directly" manipulate the data
	// on any node other than itself, for instance, by traversing the tree and calling SetAABB. SetAABB does manipulate the tree, but "indirectly" under the hood, which is okay.
	const BVNode* GetParent() const;
	const BVNode* GetLeftChild() const;
	const BVNode* GetRightChild() const;
	
	bool IsLeaf() const;

	const BVNode* Root() const;
	BVNode* LeftMostLeaf() const;
	BVNode* Sibling() const;

	std::vector<BVNode*> FindLeftToRightLeafOrder() const;
	std::vector<BVNodePair> FindIntersectingLeaves() const;

	void RefitAndRotateTree();
	bool SetAABB(const AABB& aabb);

	// returns false if this is not a leaf. Internal nodes cannot be detached because we would just have to create a new one anyways.
	bool Detach();

protected:
	//
	// ALL FUNCTIONS THAT CHANGE THE ROOT OR REMOVE NODES (I.E. LEAF NODES, SINCE WE CANNOT REMOVE INTERNAL NODES FROM A BVH)
	// MUST BE MADE TO ONLY BE ACCESSABLE FROM BVTREE. OTHERWISE, BVTREE LOSES ITS MANAGERIAL ABILITIES.
	//

	BVNode();
	virtual ~BVNode();

	// If the content is already linked to a BVNode, then the previously linked BVNode is cleared of content.
	// After all each BVNodeContent object only has a pointer to a single BVNode.
	bool SetContent(BVNodeContent*  content);

	BVNode* m_parent;

	BVNode* m_left;

	union
	{
		// Only leaf nodes can have bound content. Therefore, we can share this memory, since IsLeaf() is conditioned on the existence of m_left
		BVNode* m_right;
		BVNodeContent* m_content;
	};

	AABB m_AABB;

	BVTree* m_tree;
	unsigned int m_index; // index in the array of the tree

	// These two indices are used to keep track of all "active" BVNodes in the tree, so that we can iterate through them as a linked list
	unsigned int m_iNext;
	unsigned int m_iPrev;
};

