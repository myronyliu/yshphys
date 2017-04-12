#pragma once
#include "RigidBody.h"

class PhysicsScene;

class Island
{
	friend class PhysicsScene;
private:
	Island();
	~Island();

	void AddContact(const PositionConstraint_Contact& contact);

	std::vector<PositionConstraint_Contact> m_contacts;

	Island* m_prev;
	Island* m_next;

	void PrependTo(Island* island);
	Island* Merge(Island* island);
	void ResolveContacts() const;
};

