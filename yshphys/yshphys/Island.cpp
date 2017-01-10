#include "stdafx.h"
#include "Island.h"


Island::Island() :
	m_nContacts(0)
{
}


Island::~Island()
{
}

void Island::AddContact(const Contact& contact)
{
	m_contacts[m_nContacts++] = contact;
	contact.body[0]->SetIsland(this);
	contact.body[1]->SetIsland(this);
}

void Island::Merge(Island* island)
{
	for (int i = 0; i < island->m_nContacts; ++i)
	{
		AddContact(island->m_contacts[i]);
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