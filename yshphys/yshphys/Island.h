#pragma once
#include "RigidBody.h"

class PhysicsScene;

class Island
{
	friend class PhysicsScene;
private:
	Island();
	~Island();

	void AddContact(const Contact& contact);

	Contact m_contacts[256];
	int m_nContacts;

	Island* m_prev;
	Island* m_next;

	void Merge(Island* island);
};

