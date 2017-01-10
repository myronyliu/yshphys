#include "stdafx.h"
#include "Island.h"


Island::Island() :
	m_prev(this),
	m_next(this)
{
}


Island::~Island()
{
}

void Island::AddContact(const Contact& contact)
{
	m_contacts.push_back(contact);
	contact.body[0]->SetIsland(this);
	contact.body[1]->SetIsland(this);
}

void Island::Merge(Island* island)
{
	m_contacts.reserve(m_contacts.size() + island->m_contacts.size());
	for (Contact contact : island->m_contacts)
	{
		AddContact(contact);
	}
	if (island->m_prev != nullptr)
	{
		island->m_prev->m_next = island->m_next;
	}
	if (island->m_next != nullptr)
	{
		island->m_next->m_prev = island->m_prev;
	}
	delete island;
}