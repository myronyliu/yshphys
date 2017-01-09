#include "stdafx.h"
#include "BVNode.h"
#include "BVTree.h"
#include "RigidBody.h"

BVNodeContent::BVNodeContent() : m_bvNode(nullptr)
{
}

BVNode* BVNodeContent::GetBVNode() const
{
	return m_bvNode;
}

BVNode::BVNode()
	: m_parent(nullptr), m_left(nullptr), m_right(nullptr), m_content(nullptr), m_tree(nullptr), m_index(INVALID_BVNODEINDEX)
{
	m_AABB.min = dVec3(0.0, 0.0, 0.0);
	m_AABB.max = dVec3(0.0, 0.0, 0.0);
}

BVNode::~BVNode()
{
}

AABB BVNode::GetAABB() const
{
	return m_AABB;
}

bool BVNode::IsLeaf() const
{
	return m_left == nullptr;
}

BVNodeContent* BVNode::GetContent() const
{
	return IsLeaf() ? m_content : nullptr;
}

const BVNode* BVNode::GetParent() const
{
	return m_parent;
}
const BVNode* BVNode::GetLeftChild() const
{
	return m_left;
}
const BVNode* BVNode::GetRightChild() const
{
	return m_right;
}

const BVNode* BVNode::Root() const
{
	return &m_tree->m_nodes[m_tree->m_iRoot];
}

BVNode* BVNode::LeftMostLeaf() const
{
	if (IsLeaf())
	{
		return nullptr;
	}
	else
	{
		BVNode* current = m_left;
		BVNode* child = current->m_left;

		while (child != nullptr)
		{
			current = child;
			child = current->m_left;
		}
		return current;
	}
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
std::vector<BVNode*> BVNode::FindLeftToRightLeafOrder() const
{
	std::vector<BVNode*> leaves(0);

	if (!IsLeaf())
	{
		std::stack<BVNode*> nodeStack;

		nodeStack.push(m_right);
		nodeStack.push(m_left);

		while (!nodeStack.empty())
		{
			BVNode* node = nodeStack.top();
			nodeStack.pop();

			if (node->IsLeaf())
			{
				leaves.push_back(node);
			}
			else
			{
				nodeStack.push(node->m_right);
				nodeStack.push(node->m_left);
			}
		}
	}
	return leaves;
}

std::vector<BVNodePair> BVNode::FindIntersectingLeaves() const
{
	std::vector<BVNodePair> intersectingLeaves(0);

	std::vector<BVNode*> leaves = FindLeftToRightLeafOrder();

	const int iMax = (int)leaves.size() - 1;

	for (int i = 0; i < iMax; ++i)
	{
		BVNode* leaf = leaves[i];
		const AABB& leafAABB = leaf->m_AABB;

		BVNode* pivotPrev = leaf;
		BVNode* pivot = leaf->m_parent;

		while (pivot != m_parent)
		{
			if (pivot->m_right != pivotPrev)
			{
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

						if (node->IsLeaf())
						{
							assert(leaf != node);
							if (leafAABB.Overlaps(node->m_AABB))
							{
								BVNodePair pair;
								pair.nodes[0] = leaf;
								pair.nodes[1] = node;
								intersectingLeaves.push_back(pair);
							}
						}
						else
						{
							if (leafAABB.Overlaps(right->m_AABB))
							{
								nodes.push(right);
							}
							if (leafAABB.Overlaps(left->m_AABB))
							{
								nodes.push(left);
							}
						}
					}
				}
			}
			pivotPrev = pivot;
			pivot = pivot->m_parent;
		}
	}
	return intersectingLeaves;
}

void BVNode::RefitAndRotateTree()
{
	if (m_parent != nullptr)
	{
		BVNode* d = m_parent;

		BVNode* root = &m_tree->m_nodes[m_tree->m_iRoot];

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

			if (d->m_parent == nullptr) // d is the root. Aggregate child AABBs and we are done.
			{
				d->m_AABB = a->m_AABB.Aggregate(b->m_AABB);
				return;
			}

			const AABB ab = a->m_AABB.Aggregate(b->m_AABB);
			const AABB bc = b->m_AABB.Aggregate(c->m_AABB);
			const AABB ca = c->m_AABB.Aggregate(a->m_AABB);

			const double abArea = ab.Area() + c->m_AABB.Area(); // Minimize the sum of surface areas
			const double bcArea = bc.Area() + a->m_AABB.Area();
			const double caArea = ca.Area() + b->m_AABB.Area();

			if (abArea <= bcArea && abArea <= caArea)
			{
				d->m_AABB = ab;
			}
			else if (bcArea <= caArea && bcArea <= abArea)
			{
				d->m_AABB = bc;

				d->m_left = b;
				d->m_parent = d;

				d->m_right = c;
				c->m_parent = d;

				e->m_left = d;
				d->m_parent = e;

				e->m_right = a;
				a->m_parent = e;
			}
			else
			{
				d->m_AABB = ca;

				d->m_left = c;
				c->m_parent = d;

				d->m_right = a;
				a->m_parent = d;

				e->m_left = d;
				d->m_parent = e;

				e->m_right = b;
				b->m_parent = e;
			}

			const AABB abc = ab.Aggregate(bc);
			const AABB& eAABB = e->m_AABB;

			// If e's provisional new AABB exceeds its old AABB, then we have to do the tree rotation for the next level up as well.
			if (abc.min.x < eAABB.min.x || abc.min.y < eAABB.min.y || abc.min.z < eAABB.min.z ||
				abc.max.x > eAABB.max.x || abc.max.y > eAABB.max.z || abc.max.z > eAABB.max.z)
			{
				d = e;
			}
			else
			{
				d->m_AABB = abc;
				return;
			}
		}
	}
}

bool BVNode::SetAABB(const AABB& aabb)
{
	// Check that we aren't stupidly downsizing an internal (nonleaf) node such that the children are no longer contained
	if (!IsLeaf())
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
	AABB oldAABB = m_AABB;
	m_AABB = aabb;

	if (m_parent != nullptr)
	{
		if (m_AABB.min.x < oldAABB.min.x ||
			m_AABB.min.y < oldAABB.min.y ||
			m_AABB.min.z < oldAABB.min.z ||
			m_AABB.max.x > oldAABB.max.x ||
			m_AABB.max.y > oldAABB.max.y ||
			m_AABB.max.z > oldAABB.max.z)
		{
			RefitAndRotateTree();
		}
	}
	return true;
}

bool BVNode::Detach()
{
	if (IsLeaf() && m_parent != nullptr)
	{
		// Add this node and the parent on the free list
		m_tree->PushFreeNode(m_index);
		m_tree->PushFreeNode(m_parent->m_index);

		BVNode* sibling = (this == m_parent->m_left) ? m_parent->m_right : m_parent->m_left;

		// merge the sibling with the parent (if there is no grandparent set the root to the sibling)
		BVNode* grandparent = m_parent->m_parent;
		sibling->m_parent = grandparent;
		if (grandparent == nullptr)
		{
			m_tree->m_iRoot = sibling->m_index;
		}

		// remove the parent from the tree
		m_parent->m_left = nullptr;
		m_parent->m_right = nullptr;
		m_parent->m_parent = nullptr;

		// detach this node
		m_parent = nullptr;
		m_content->m_bvNode = nullptr;
		m_content = nullptr;

		return true;
	}
	return false;
}

bool BVNode::SetContent(BVNodeContent* content)
{
	if (IsLeaf())
	{
		if (content->m_bvNode != nullptr)
		{
			content->m_bvNode->Detach(); // content no longer has an associated node
		}
		if (m_content != nullptr)
		{
			m_content->m_bvNode = nullptr;
		}
		m_content = content;
		content->m_bvNode = this;

		return true;
	}
	else
	{
		return false;
	}
}
