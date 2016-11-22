#include "stdafx.h"
#include "BV.h"


BV::BV()
	: m_parent(nullptr)
{
	m_children[0] = nullptr;
	m_children[1] = nullptr;
}


BV::~BV()
{
}
