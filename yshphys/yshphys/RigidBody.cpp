#include "stdafx.h"
#include "RigidBody.h"

#define AABB_QUANTIZATION 0.25

void QuantizeAABB(AABB& aabb)
{
	if (AABB_QUANTIZATION != 0.0)
	{
		aabb.min.x = std::floor(aabb.min.x / AABB_QUANTIZATION) * AABB_QUANTIZATION;
		aabb.min.y = std::floor(aabb.min.y / AABB_QUANTIZATION) * AABB_QUANTIZATION;
		aabb.min.z = std::floor(aabb.min.z / AABB_QUANTIZATION) * AABB_QUANTIZATION;
		aabb.max.x = std::ceil(aabb.max.x / AABB_QUANTIZATION) * AABB_QUANTIZATION;
		aabb.max.y = std::ceil(aabb.max.y / AABB_QUANTIZATION) * AABB_QUANTIZATION;
		aabb.max.z = std::ceil(aabb.max.z / AABB_QUANTIZATION) * AABB_QUANTIZATION;
	}
}

RigidBody::RigidBody() :
	m_F(0.0,0.0,0.0),
	m_T(0.0,0.0,0.0),
	m_P(0.0,0.0,0.0),
	m_L(0.0,0.0,0.0),
	m_x(0.0,0.0,0.0),
	m_v(0.0,0.0,0.0),
	m_w(0.0,0.0,0.0),
	m_q(dQuat::Identity()),
	m_m(0.0),
	m_minv(0.0),
	m_geometry(nullptr),
	m_nSpringPenalties(0)
{
	m_Ibody.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_Ibody.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_Ibody.SetRow(2, dVec3(0.0, 0.0, 0.0));

	m_Ibodyinv.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_Ibodyinv.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_Ibodyinv.SetRow(2, dVec3(0.0, 0.0, 0.0));

	m_Iinv.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_Iinv.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_Iinv.SetRow(2, dVec3(0.0, 0.0, 0.0));
}

RigidBody::~RigidBody()
{
}

double RigidBody::GetMass(double& inverseMass) const
{
	inverseMass = m_minv;
	return m_m;
}

dVec3 RigidBody::GetPosition() const
{
	return m_x;
}
dQuat RigidBody::GetRotation() const
{
	return m_q;
}
dVec3 RigidBody::GetLinearVelocity() const
{
	return m_v;
}
dVec3 RigidBody::GetAngularVelocity() const
{
	return m_w;
}
Geometry* RigidBody::GetGeometry() const
{
	return m_geometry;
}
void RigidBody::SetPosition(const dVec3& x)
{
	m_x = x;
	UpdateAABB();
}
void RigidBody::SetRotation(const dQuat& q)
{
	m_q = q;
	UpdateAABB();
}
void RigidBody::SetGeometry(Geometry* geometry)
{
	m_geometry = geometry;
	UpdateAABB();
}
void RigidBody::SetMass(double m)
{
	if (m < 0.000001)
	{
		m_m = 0.0;
		m_minv = 0.0;
	}
	else
	{
		m_m = m;
		m_minv = 1.0 / m_m;
	}
}
void RigidBody::SetInertia(dMat33 Ibody)
{
	m_Ibody = Ibody;
	m_Ibodyinv = Ibody.Inverse();

	dMat33 R(m_q);
	m_Iinv = R*m_Ibodyinv*R.Transpose();
}

void RigidBody::ApplyForce(const dVec3& F, const dVec3& applicationPt)
{
	m_F = m_F + F;
	m_T = m_T + (applicationPt - m_x).Cross(F);
}

void RigidBody::ApplyForceAtCenterOfMass(const dVec3& F)
{
	m_F = m_F + F;
}

void RigidBody::ApplySpringPenalty(const dVec3& worldAnchor, const dVec3& localBodyAnchor, double kOverM, double bOverM)
{
	SpringPenalty& penalty = m_springPenalties[m_nSpringPenalties];
	penalty.worldAnchor = worldAnchor;
	penalty.localBodyAnchor = localBodyAnchor;
	penalty.kOverM = kOverM;
	penalty.bOverM = bOverM;
	m_nSpringPenalties++;
}

void RigidBody::UpdateAABB()
{
	const BoundingBox oobb = m_geometry->GetLocalOOBB();
	const dVec3 oobbCenter = (oobb.min + oobb.max).Scale(0.5); // center of the OOBB in geom's local frame
	const dVec3 oobbSpan = (oobb.max - oobb.min).Scale(0.5); // span of the OOBB

	const dVec3 aabbCenter = m_x + m_q.Transform(m_geometry->GetPosition() + oobbCenter);
	const dQuat geomRot = m_q*m_geometry->GetRotation();

	const dVec3 aabbSpan = dMat33(geomRot).Abs().Transform(oobbSpan);

	m_AABB.min = aabbCenter - aabbSpan;
	m_AABB.max = aabbCenter + aabbSpan;

	QuantizeAABB(m_AABB);

	if (m_bvNode != nullptr)
	{
		m_bvNode->SetAABB(m_AABB);
	}
}

void RigidBody::Compute_qDot(const dQuat& q, const dVec3& L, dQuat& qDot) const
{
	const dMat33 R(q);
	const dMat33 Iinv = R*m_Ibodyinv*R.Transpose();
	const dVec3 w = Iinv.Transform(L);

	dQuat wQuat;
	wQuat.x = w.x;
	wQuat.y = w.y;
	wQuat.z = w.z;
	wQuat.w = 0.0;

	qDot = wQuat*q;
	qDot.x *= 0.5;
	qDot.y *= 0.5;
	qDot.z *= 0.5;
	qDot.w *= 0.5;
}

void RigidBody::Step(double dt)
{
	if (!m_awake)
	{
		return;
	}

	if (m_nSpringPenalties == 0)
	{
		////////////
		// LINEAR //
		////////////

		dVec3 a = m_F.Scale(m_minv);
		m_x = m_x + m_v.Scale(dt) + a.Scale(0.5*dt*dt);
		m_P = m_P + m_F.Scale(dt);
		m_v = m_P.Scale(m_minv);

		/////////////
		// ANGULAR //
		/////////////

		auto ScaleQuat = [](const dQuat& quat, double scale)
		{
			dQuat scaled;
			scaled.x = quat.x * scale;
			scaled.y = quat.y * scale;
			scaled.z = quat.z * scale;
			scaled.w = quat.w * scale;
			return scaled;
		};
		auto AddQuats = [](const dQuat& q0, const dQuat& q1)
		{
			dQuat sum;
			sum.x = q0.x + q1.x;
			sum.y = q0.y + q1.y;
			sum.z = q0.z + q1.z;
			sum.w = q0.w + q1.w;
			return sum;
		};

		dQuat qDot[4];

		dQuat q(m_q);
		dVec3 L(m_L);
		Compute_qDot(q, L, qDot[0]);

		q = AddQuats(m_q, ScaleQuat(qDot[0], 0.5*dt)).Normalize(); // Must normalize, since we construct orthogonal R to transform Ibodyinv at each step
		L = m_L + m_T.Scale(0.5*dt);
		Compute_qDot(q, L, qDot[1]);

		q = AddQuats(m_q, ScaleQuat(qDot[1], 0.5*dt)).Normalize();
		L = m_L + m_T.Scale(0.5*dt);
		Compute_qDot(q, L, qDot[2]);

		q = AddQuats(m_q, ScaleQuat(qDot[2], dt)).Normalize();
		L = m_w + m_T.Scale(dt);
		Compute_qDot(q, L, qDot[3]);

		dQuat dq = qDot[0];
		dq = AddQuats(dq, ScaleQuat(qDot[1], 2.0));
		dq = AddQuats(dq, ScaleQuat(qDot[2], 2.0));
		dq = AddQuats(dq, qDot[3]);
		dq = ScaleQuat(dq, dt / 6.0);
		m_q = AddQuats(m_q, dq).Normalize();

		m_L = m_L + m_T.Scale(dt);
	}
	else
	{

	}

	// UPDATE DERIVED QUANTITIES

	dMat33 R(m_q);
	m_Iinv = R*m_Ibodyinv*R.Transpose();
	m_w = m_Iinv.Transform(m_L);

	// SAVE THE HISTORY AND ZERO THE DRIVERS OF DYNAMICS
	m_Fprev = m_F;
	m_Tprev = m_T;
	m_F = dVec3(0.0, 0.0, 0.0);
	m_T = dVec3(0.0, 0.0, 0.0);

	UpdateAABB();
}
