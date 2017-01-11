#include "stdafx.h"
#include "Island.h"

#define MAX_CONTACTS 4 

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

void Island::PrependTo(Island* island)
{
	m_next = island;
	m_prev = island->m_prev;

	m_prev->m_next = this;
	m_next->m_prev = this;
}

void Island::Merge(Island* island)
{
	m_contacts.reserve(m_contacts.size() + island->m_contacts.size());
	for (Contact contact : island->m_contacts)
	{
		AddContact(contact);
	}
	assert(island->m_prev != nullptr);
	island->m_prev->m_next = island->m_next;
	assert(island->m_next != nullptr);
	island->m_next->m_prev = island->m_prev;
	delete island;
}

void Island::ResolveContacts() const
{
	// dynamic variables are v, w
	// constraint is ...
	//     (v0 + r0 x w0).n0 + (v1 + r1 x w1).n1 = 0
	// or rearranging via triple product rule...
	//     n0.v0 + n1.v1 + (r0 x n0).w0 + (r1 x n1).w1 = 0
	// hence the structure of a row of the constraint matrix J
	struct JRow
	{
		dVec3 n0;
		dVec3 n1;
		dVec3 r0xn0;
		dVec3 r1xn1;
	};

	JRow J[MAX_CONTACTS];
	JRow JMinv[MAX_CONTACTS];

	double JMJ[MAX_CONTACTS*MAX_CONTACTS];
	double b[MAX_CONTACTS];

	double minImpulse[MAX_CONTACTS];
	double maxImpulse[MAX_CONTACTS];
	double impulse[MAX_CONTACTS];
	std::memset(minImpulse, 0, sizeof(minImpulse));

	const int nContacts = (int)m_contacts.size();

	for (int i = 0; i < nContacts; ++i)
	{
		const Contact& contact = m_contacts[i];
		J[i].n0 = contact.n[0];
		J[i].n1 = contact.n[1];
		J[i].r0xn0 = (contact.x[0] - contact.body[0]->GetPosition()).Cross(contact.n[0]);
		J[i].r1xn1 = (contact.x[1] - contact.body[1]->GetPosition()).Cross(contact.n[1]);

		JMinv[i].n0 = J[i].n0.Scale(contact.body[0]->GetInverseMass());
		JMinv[i].n1 = J[i].n1.Scale(contact.body[1]->GetInverseMass());
		JMinv[i].r0xn0 = contact.body[0]->GetInverseInertia().Transform(J[i].r0xn0);
		JMinv[i].r1xn1 = contact.body[1]->GetInverseInertia().Transform(J[i].r1xn1);

		b[i] = -(
			J[i].n0.Dot(contact.body[0]->GetLinearVelocity()) +
			J[i].n1.Dot(contact.body[1]->GetLinearVelocity()) +
			J[i].r0xn0.Dot(contact.body[0]->GetAngularVelocity()) +
			J[i].r1xn1.Dot(contact.body[1]->GetAngularVelocity())
			);
		maxImpulse[i] = 88888888.0;
	}

	for (int i = 0; i < nContacts; ++i)
	{
		for (int j = i; j < nContacts; ++j)
		{
			double jmj =
				JMinv[i].n0.Dot(J[i].n0) +
				JMinv[i].n1.Dot(J[i].n1) +
				JMinv[i].r0xn0.Dot(J[j].r0xn0) +
				JMinv[i].r1xn1.Dot(J[j].r1xn1);

			JMJ[nContacts*i + j] = jmj;
			JMJ[nContacts*j + i] = jmj;
		}
	}

	MathUtils::GaussSeidel(JMJ, b, minImpulse, maxImpulse, nContacts, impulse);

	for (int i = 0; i < nContacts; ++i)
	{
		const Contact& contact = m_contacts[i];
		contact.body[0]->ApplyImpulse(contact.n[0].Scale(impulse[i]), contact.x[0]);
		contact.body[1]->ApplyImpulse(contact.n[1].Scale(impulse[i]), contact.x[1]);

		const double k = 4.0;

		const dVec3 d = contact.n[0].Scale((contact.x[1] - contact.x[0]).Dot(contact.n[0]));

		contact.body[0]->ApplyForce(d.Scale(contact.body[0]->GetMass()*k), contact.x[0]);
		contact.body[1]->ApplyForce(-d.Scale(contact.body[1]->GetMass()*k), contact.x[1]);
	}

	double* const minForce = minImpulse;
	double* const maxForce = maxImpulse;
	double* const force = impulse;

	for (int i = 0; i < nContacts; ++i)
	{
		const Contact& contact = m_contacts[i];
		J[i].n0 = contact.n[0];
		J[i].n1 = contact.n[1];
		J[i].r0xn0 = (contact.x[0] - contact.body[0]->GetPosition()).Cross(contact.n[0]);
		J[i].r1xn1 = (contact.x[1] - contact.body[1]->GetPosition()).Cross(contact.n[1]);

		JMinv[i].n0 = J[i].n0.Scale(contact.body[0]->GetInverseMass());
		JMinv[i].n1 = J[i].n1.Scale(contact.body[1]->GetInverseMass());
		JMinv[i].r0xn0 = contact.body[0]->GetInverseInertia().Transform(J[i].r0xn0);
		JMinv[i].r1xn1 = contact.body[1]->GetInverseInertia().Transform(J[i].r1xn1);

		const dVec3 g(0.0, 0.0, -10.0);
		const dVec3 mg0 = g.Scale(contact.body[0]->GetMass());
		const dVec3 mg1 = g.Scale(contact.body[1]->GetMass());

		b[i] = -(
			JMinv[i].n0.Dot(mg0) +
			JMinv[i].n1.Dot(mg1)
			);
	}

	MathUtils::GaussSeidel(JMJ, b, minForce, maxForce, nContacts, force);

	for (int i = 0; i < nContacts; ++i)
	{
		const Contact& contact = m_contacts[i];
		contact.body[0]->ApplyForce(contact.n[0].Scale(force[i]), contact.x[0]);
		contact.body[1]->ApplyForce(contact.n[1].Scale(force[i]), contact.x[1]);
	}
}