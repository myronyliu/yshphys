#include "stdafx.h"
#include "BVNode.h"
#include "RigidBody.h"

BVNode::BVNode()
	: m_parent(nullptr), m_left(nullptr), m_right(nullptr), m_content(nullptr)
{
}

BVNode::~BVNode()
{
}

AABB BVNode::GetAABB() const
{
	return m_AABB;
}

BVNode* BVNode::Root()
{
	BVNode* current = this;
	BVNode* parent = m_parent;
	while (parent != nullptr)
	{
		current = parent;
		parent = current->m_parent;
	}
	return current;
}

BVNode* BVNode::LeftMostLeaf()
{
	BVNode* current = this;
	BVNode* child = m_left;
	while (child != nullptr)
	{
		current = child;
		child = current->m_left;
	}
	return current;
}

std::vector<BVNode*> BVNode::FindLeftToRightLeafOrder()
{
	std::vector<BVNode*> leaves(0);

	std::stack<BVNode*> nodeStack;

	nodeStack.push(this);

	while (!nodeStack.empty())
	{
		BVNode* node = nodeStack.top();
		nodeStack.pop();

		BVNode* left = node->m_left;
		BVNode* right = node->m_right;

		if (left == nullptr) // no need to check right child, since this is a BVH
		{
			leaves.push_back(node);
		}
		else
		{
			nodeStack.push(right);
			nodeStack.push(left);
		}
	}
	return leaves;
}

std::vector<BVNodePair> BVNode::FindIntersectingLeaves()
{
	std::vector<BVNodePair> intersectingLeaves(0);

	std::vector<BVNode*> leaves = FindLeftToRightLeafOrder();

	for (std::vector<BVNode*>::iterator it = leaves.begin(); it != leaves.end(); ++it)
	{
		BVNode* leaf = *it;
		BVNode* pivot = leaf;
		const AABB& leafAABB = leaf->m_AABB;

		while (pivot != this)
		{
			pivot = pivot->m_parent;
			BVNode* subTree = pivot->m_right;

			if (leafAABB.Overlaps(subTree->m_AABB))
			{
				std::stack<BVNode*> nodes;
				nodes.push(subTree);
				while (!nodes.empty())
				{
					BVNode* node = nodes.top();
					nodes.pop();

					BVNode* left = node->m_left;
					BVNode* right = node->m_right;

					if (left == nullptr)
					{
						BVNodePair pair;
						pair.nodes[0] = leaf;
						pair.nodes[1] = node;
						intersectingLeaves.push_back(pair);
					}
					else
					{
						if (leafAABB.Overlaps(left->m_AABB))
						{
							nodes.push(left);
						}
						if (leafAABB.Overlaps(right->m_AABB))
						{
							nodes.push(right);
						}
					}
				}
			}
		}
	}
	return intersectingLeaves;
}
