#include "stdafx.h"
#include "PositionConstraint_Contact.h"
#include "RigidBody.h"

PositionConstraint_Contact::PositionConstraint_Contact() : m_lambda(0.0)
{
}


PositionConstraint_Contact::~PositionConstraint_Contact()
{
}

void PositionConstraint_Contact::EvaluateC()
{
	m_C = (x[0] - x[1]).Dot(n[0]);
}

void PositionConstraint_Contact::EvaluateJMJ()
{
	const dVec3 pos[2] = { body[0]->GetPosition(), body[1]->GetPosition() };

	m_J[0].J_v = n[0];
	m_J[1].J_v = n[1];
	m_J[0].J_w = (x[0] - pos[0]).Cross(n[0]);
	m_J[1].J_w = (x[1] - pos[1]).Cross(n[1]);

	m_JM[0].J_v = m_J[0].J_v.Scale(body[0]->GetInverseMass());
	m_JM[1].J_v = m_J[1].J_v.Scale(body[1]->GetInverseMass());
	m_JM[0].J_w = body[0]->GetInverseInertia().Transform(m_J[0].J_w);
	m_JM[1].J_w = body[1]->GetInverseInertia().Transform(m_J[1].J_w);

	m_JMJ =
		m_JM[0].J_v.Dot(m_J[0].J_v) +
		m_JM[0].J_w.Dot(m_J[0].J_w) +
		m_JM[1].J_v.Dot(m_J[1].J_v) +
		m_JM[1].J_w.Dot(m_J[1].J_w);
}

void PositionConstraint_Contact::BuildFixedTerms()
{
	PositionConstraint::BuildFixedTerms();

	m_bRestitution =
		m_J[0].J_v.Dot(body[0]->GetLinearVelocity()) +
		m_J[0].J_w.Dot(body[0]->GetAngularVelocity()) +
		m_J[1].J_v.Dot(body[1]->GetLinearVelocity()) +
		m_J[1].J_w.Dot(body[1]->GetAngularVelocity());

	m_bRestitution *= Material::Restitution(
		body[0]->GetMaterial(x[0]),
		body[1]->GetMaterial(x[1]));
}

void PositionConstraint_Contact::Resolve()
{
	const double Jv =
		m_J[0].J_v.Dot(body[0]->GetLinearVelocity()) +
		m_J[0].J_w.Dot(body[0]->GetAngularVelocity()) +
		m_J[1].J_v.Dot(body[1]->GetLinearVelocity()) +
		m_J[1].J_w.Dot(body[1]->GetAngularVelocity());

	const double b = m_bRestitution;

	const double Jvb = Jv + b;

	const double dLambda = -Jvb / m_JMJ;
	const double lambda_clamped = std::min(std::max(0.0, m_lambda + dLambda), DBL_MAX);
	const double dLambda_clamped = lambda_clamped - m_lambda;
	m_lambda = lambda_clamped;

	if (!body[0]->IsStatic())
	{
		body[0]->ApplyLinImpulse_Immediate(m_J[0].J_v.Scale(dLambda_clamped));
		body[0]->ApplyAngImpulse_Immediate(m_J[0].J_w.Scale(dLambda_clamped));
	}

	if (!body[1]->IsStatic())
	{
		body[1]->ApplyLinImpulse_Immediate(m_J[1].J_v.Scale(dLambda_clamped));
		body[1]->ApplyAngImpulse_Immediate(m_J[1].J_w.Scale(dLambda_clamped));
	}
}