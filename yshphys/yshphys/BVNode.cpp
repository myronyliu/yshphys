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

BVNode* BVNode::Sibling() const
{
	if (m_parent)
	{
		if (m_parent->m_left == this)
		{
			return m_parent->m_right;
		}
		else
		{
			return m_parent->m_left;
		}
	}
	return nullptr;
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

bool BVNode::SetAABB(const AABB& aabb)
{
	// Check that we aren't stupidly downsizing an internal (nonleaf) node such that the children are no longer contained
	if (m_left != nullptr)
	{
		const AABB childrenAABB = m_left->m_AABB.Aggregate(m_right->m_AABB);
		if (
			childrenAABB.min.x < aabb.min.x ||
			childrenAABB.min.y < aabb.min.y ||
			childrenAABB.min.z < aabb.min.z ||

			childrenAABB.max.x > aabb.max.x ||
			childrenAABB.max.y > aabb.max.y ||
			childrenAABB.max.z > aabb.max.z
			)
		{
			return false;
		}
	}
	m_AABB = aabb;

	if (m_parent == nullptr)
	{
		return true; // We validly resized the root. There is nothing else to do.
	}

	BVNode* d = m_parent;
	BVNode* root = Root();

	while (true)
	{
		BVNode* a = d->m_left;
		BVNode* b = d->m_right;
		BVNode* c = d->Sibling();
		BVNode* e = d->m_parent;

		//      ____e____
		//     |         |
		//   __d__       c
		//  |     |
		//  a     b

		const AABB ab = a->m_AABB.Aggregate(b->m_AABB);
		const AABB bc = b->m_AABB.Aggregate(c->m_AABB);
		const AABB ca = c->m_AABB.Aggregate(a->m_AABB);

		const double abArea = ab.Area();
		const double bcArea = bc.Area();
		const double caArea = ca.Area();

		if (abArea <= bcArea && abArea <= caArea)
		{
			d->m_AABB = ab;
		}
		else if (bcArea <= caArea && bcArea <= abArea)
		{
			d->m_AABB = bc;
			d->m_left = b;
			d->m_right = c;
			e->m_left = d;
			e->m_right = a;
		}
		else
		{
			d->m_AABB = ca;
			d->m_left = c;
			d->m_right = a;
			e->m_left = d;
			e->m_right = b;
		}

		const AABB abc = ab.Aggregate(bc);
		const AABB& eAABB = e->m_AABB;

		// If e's provisional new AABB exceeds its old AABB, then we have to do the tree rotation for the next level up as well.
		if (abc.min.x < eAABB.min.x || abc.min.y < eAABB.min.y || abc.min.z < eAABB.min.z ||
			abc.max.x > eAABB.max.x || abc.max.y > eAABB.max.z || abc.max.z > eAABB.max.z)
		{
			if (e == root)
			{
				e->m_AABB = abc;
				return true;
			}
			else
			{
				d = e;
			}
		}
		else
		{
			return true;
		}
	}
	return true;
}