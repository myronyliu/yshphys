#include "stdafx.h"
#include "GameNode.h"
#include "GameObject.h"

GameNode::GameNode() : m_gameObject(nullptr), m_prev(nullptr), m_next(nullptr)
{
}
GameNode::~GameNode()
{
}
GameObject* GameNode::GetGameObject() const
{
	return m_gameObject;
}
GameNode* GameNode::GetPrev() const
{
	return m_next;
}
GameNode* GameNode::GetNext() const
{
	return m_prev;
}
void GameNode::BindGameObject(GameObject* gameObject)
{
	m_gameObject = gameObject;
	gameObject->m_node = this;
}
void GameNode::Remove()
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
		m_prev = nullptr;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
		m_next = nullptr;
	}
}
void GameNode::AppendTo(GameNode* prev)
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
	if (prev)
	{
		if (GameNode* next = prev->m_next)
		{
			next->m_prev = this;
			m_next = next;
		}
		prev->m_next = this;
		m_prev = prev;
	}
}
void GameNode::PrependTo(GameNode* next)
{
	if (m_prev)
	{
		m_prev->m_next = m_next;
	}
	if (m_next)
	{
		m_next->m_prev = m_prev;
	}
	if (next)
	{
		if (GameNode* prev = next->m_prev)
		{
			prev->m_next = this;
			m_prev = prev;
		}
		next->m_prev = this;
		m_next = next;
	}
}

FreedGameNode::FreedGameNode() : m_node(nullptr), m_precedingNode(nullptr)
{
}
FreedGameNode::~FreedGameNode()
{
}
