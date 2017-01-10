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

	std::vector<Contact> m_contacts;

	Island* m_prev;
	Island* m_next;

	void PrependTo(Island* island);
	void Merge(Island* island);
};

