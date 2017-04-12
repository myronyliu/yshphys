#pragma once
#include "RigidBody.h"

#define USE_SEQUENTIAL_IMPULSE_SOLVER 1

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
#if USE_SEQUENTIAL_IMPULSE_SOLVER
	void ResolveContacts(double dt);
#else
	void ResolveContacts(double dt) const;
#endif
};

