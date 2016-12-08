#pragma once
#include "BVNode.h"

#define MAX_BV_NODES 1<<12

class BVTree
{
public:
	BVTree();
	virtual ~BVTree();
protected:
	std::stack<unsigned int> m_freeNodes;

	BVNode m_nodes[MAX_BV_NODES];
};

