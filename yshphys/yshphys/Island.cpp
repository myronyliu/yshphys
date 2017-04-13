#include "stdafx.h"
#include "Island.h"

#define MAX_CONTACTS 64

Island::Island() :
	m_prev(this),
	m_next(this)
{
}


Island::~Island()
{
}

void Island::AddContact(const PositionConstraint_Contact& contact)
{
	m_contacts.push_back(contact);

	int nBodies;
	RigidBody*const* bodies;
	(&contact)->GetBodies(bodies, nBodies);

	for (int i = 0; i < nBodies; ++i)
	{
		RigidBody* body = bodies[i];

		if (!body->IsStatic())
		{
			body->SetIsland(this);
		}
	}
}

void Island::PrependTo(Island* island)
{
	m_next = island;
	m_prev = island->m_prev;

	m_prev->m_next = this;
	m_next->m_prev = this;
}

Island* Island::Merge(Island* island)
{
	m_contacts.reserve(m_contacts.size() + island->m_contacts.size());
	for (PositionConstraint_Contact contact : island->m_contacts)
	{
		AddContact(contact);
	}
	assert(island->m_prev != nullptr);
	island->m_prev->m_next = island->m_next;
	assert(island->m_next != nullptr);
	island->m_next->m_prev = island->m_prev;
	delete island;

	return this;
}

#if USE_SEQUENTIAL_IMPULSE_SOLVER

void Island::ResolveContacts(double dt)
{
	const int nContacts = (int)m_contacts.size();
	for (int i = 0; i < nContacts; ++i)
	{
		PositionConstraint_Contact& contact = m_contacts[i];
		contact.BuildFixedTerms(dt);
	}

	for (int iters = 0; iters < 16; ++iters)
	{
		for (int i = 0; i < nContacts; ++i)
		{
			PositionConstraint_Contact& contact = m_contacts[i];
			contact.Resolve();
		}
	}
}

#else

void Island::ResolveContacts(double dt) const
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
	dVec3 vSlip[MAX_CONTACTS];
	std::memset(minImpulse, 0, sizeof(minImpulse));

	const int nContacts = (int)m_contacts.size();

	for (int i = 0; i < nContacts; ++i)
	{
		const PositionConstraint_Contact& contact = m_contacts[i];

		const dVec3 pos[2] = { contact.body[0]->GetPosition(), contact.body[1]->GetPosition() };

		J[i].n0 = contact.n[0];
		J[i].n1 = contact.n[1];
		J[i].r0xn0 = (contact.x[0] - pos[0]).Cross(contact.n[0]);
		J[i].r1xn1 = (contact.x[1] - pos[1]).Cross(contact.n[1]);

		JMinv[i].n0 = J[i].n0.Scale(contact.body[0]->GetInverseMass());
		JMinv[i].n1 = J[i].n1.Scale(contact.body[1]->GetInverseMass());
		JMinv[i].r0xn0 = contact.body[0]->GetInverseInertia().Transform(J[i].r0xn0);
		JMinv[i].r1xn1 = contact.body[1]->GetInverseInertia().Transform(J[i].r1xn1);

		const double restitution = Material::Restitution(
			contact.body[0]->GetMaterial(contact.x[0]),
			contact.body[1]->GetMaterial(contact.x[1]));

		const dVec3 vLin[2] =
		{
			contact.body[0]->GetLinearVelocity(),
			contact.body[1]->GetLinearVelocity()
		};
		const dVec3 vAng[2] =
		{
			contact.body[0]->GetAngularVelocity(),
			contact.body[1]->GetAngularVelocity()
		};

		const double vImpact =
			J[i].n0.Dot(vLin[0]) +
			J[i].n1.Dot(vLin[1]) +
			J[i].r0xn0.Dot(vAng[0]) +
			J[i].r1xn1.Dot(vAng[1]);

		b[i] = -vImpact*(1.0 + restitution);
		maxImpulse[i] = 88888888.0;

		// Check for slipping so that we can apply friction

		const dVec3 v[2] =
		{
			vLin[0] + vAng[0].Cross(contact.x[0] - pos[0]),
			vLin[1] + vAng[1].Cross(contact.x[1] - pos[1])
		};
		const dVec3 v1_v0 = v[1] - v[0];
		vSlip[i] = v1_v0 - contact.n[0].Scale(v1_v0.Dot(contact.n[0]));
	}

	for (int i = 0; i < nContacts; ++i)
	{
		JMJ[nContacts*i + i] =
			JMinv[i].n0.Dot(J[i].n0) +
			JMinv[i].n1.Dot(J[i].n1) +
			JMinv[i].r0xn0.Dot(J[i].r0xn0) +
			JMinv[i].r1xn1.Dot(J[i].r1xn1);

		for (int j = i + 1; j < nContacts; ++j)
		{
			double jmj = 0.0;

			if (m_contacts[i].body[0] == m_contacts[j].body[0])
			{
				jmj += JMinv[i].n0.Dot(J[j].n0);
				jmj += JMinv[i].r0xn0.Dot(J[j].r0xn0);
			}
			else if (m_contacts[i].body[0] == m_contacts[j].body[1])
			{
				jmj += JMinv[i].n0.Dot(J[j].n1);
				jmj += JMinv[i].r0xn0.Dot(J[j].r1xn1);
			}

			if (m_contacts[i].body[1] == m_contacts[j].body[1])
			{
				jmj += JMinv[i].n1.Dot(J[j].n1);
				jmj += JMinv[i].r1xn1.Dot(J[j].r1xn1);
			}
			else if (m_contacts[i].body[1] == m_contacts[j].body[0])
			{
				jmj += JMinv[i].n1.Dot(J[j].n0);
				jmj += JMinv[i].r1xn1.Dot(J[j].r0xn0);
			}

			JMJ[nContacts*i + j] = jmj;
			JMJ[nContacts*j + i] = jmj;
		}
	}

	double JMJ_diag[MAX_CONTACTS];
	for (int i = 0; i < nContacts; ++i)
	{
		JMJ_diag[i] = JMJ[nContacts*i + i];
		assert(JMJ_diag[i] >= 0.0);
	}

	std::memset(impulse, 0, sizeof(impulse));
	MathUtils::GaussSeidel(JMJ, b, minImpulse, maxImpulse, nContacts, impulse);

	for (int i = 0; i < nContacts; ++i)
	{
		assert(abs(impulse[i]) < 100000.0);

		const PositionConstraint_Contact& contact = m_contacts[i];
		contact.body[0]->ApplyLinImpulse(J[i].n0.Scale(impulse[i]));
		contact.body[1]->ApplyLinImpulse(J[i].n1.Scale(impulse[i]));
		contact.body[0]->ApplyAngImpulse(J[i].r0xn0.Scale(impulse[i]));
		contact.body[1]->ApplyAngImpulse(J[i].r1xn1.Scale(impulse[i]));
	}

	double* const minForce = minImpulse;
	double* const maxForce = maxImpulse;
	double* const force = impulse;

	for (int i = 0; i < nContacts; ++i)
	{
		const PositionConstraint_Contact& contact = m_contacts[i];

		b[i] = -(
			JMinv[i].n0.Dot(contact.body[0]->GetForce()) +
			JMinv[i].n1.Dot(contact.body[1]->GetForce()) +
			JMinv[i].r0xn0.Dot(contact.body[0]->GetTorque()) +
			JMinv[i].r1xn1.Dot(contact.body[1]->GetTorque())
			);
	}

	std::memset(force, 0, sizeof(impulse));
	MathUtils::GaussSeidel(JMJ, b, minForce, maxForce, nContacts, force);

	for (int i = 0; i < nContacts; ++i)
	{
		const PositionConstraint_Contact& contact = m_contacts[i];

		dVec3 F[2] =
		{
			contact.n[0].Scale(force[i]),
			contact.n[1].Scale(force[i])
		};

		if ((float)vSlip[i].Dot(vSlip[i]) > 0.0f)
		{
			const FrictionCoefficients mu = Material::Friction(
				contact.body[0]->GetMaterial(contact.x[0]),
				contact.body[1]->GetMaterial(contact.x[1]));

			const dVec3 friction0 = vSlip[i].Scale(mu.uKinetic * force[i] / sqrt(vSlip[i].Dot(vSlip[i])));
			F[0] = F[0] + friction0;
			F[1] = F[1] - friction0;
		}

		contact.body[0]->ApplyForce(F[0], contact.x[0]);
		contact.body[1]->ApplyForce(F[1], contact.x[1]);
	}
}

#endif