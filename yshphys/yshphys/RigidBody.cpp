#include "stdafx.h"
#include "RigidBody.h"
#include "Force.h"

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
	m_nForces(0),
	m_island(nullptr)
{
	m_geometry.geom = nullptr;
	m_geometry.pos = dVec3(0.0, 0.0, 0.0);
	m_geometry.rot = dQuat::Identity();

	m_state.P = dVec3(0.0, 0.0, 0.0);
	m_state.x = dVec3(0.0, 0.0, 0.0);

	m_state.L = dVec3(0.0, 0.0, 0.0);
	m_state.q = dQuat::Identity();

	m_v = dVec3(0.0, 0.0, 0.0);
	m_w = dVec3(0.0, 0.0, 0.0);

	m_inertia.m = 0.0;
	m_inertia.minv = 0.0;

	m_inertia.Ibody.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_inertia.Ibody.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_inertia.Ibody.SetRow(2, dVec3(0.0, 0.0, 0.0));

	m_inertia.Ibodyinv.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_inertia.Ibodyinv.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_inertia.Ibodyinv.SetRow(2, dVec3(0.0, 0.0, 0.0));

	m_Iinv.SetRow(0, dVec3(0.0, 0.0, 0.0));
	m_Iinv.SetRow(1, dVec3(0.0, 0.0, 0.0));
	m_Iinv.SetRow(2, dVec3(0.0, 0.0, 0.0));
}

RigidBody::~RigidBody()
{
}

double RigidBody::GetMass() const
{
	return m_inertia.m;
}

double RigidBody::GetInverseMass() const
{
	return m_inertia.minv;
}

dMat33 RigidBody::GetInverseInertia() const
{
	return m_Iinv;
}

bool RigidBody::IsStatic() const
{
	const dMat33& I = m_inertia.Ibodyinv;
	return m_inertia.minv + I(0, 0) + I(1, 1) + I(2, 2) == 0.0;
}

dVec3 RigidBody::GetPosition() const
{
	return m_state.x;
}
dQuat RigidBody::GetRotation() const
{
	return m_state.q;
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
	return m_geometry.geom;
}
void RigidBody::GetGeometryLocalTransform(dVec3& pos, dQuat& rot) const
{
	pos = m_geometry.pos;
	rot = m_geometry.rot;
}
void RigidBody::GetGeometryGlobalTransform(dVec3& pos, dQuat& rot) const
{
	rot = m_state.q*m_geometry.rot;
	pos = m_state.x + rot.Transform(m_geometry.pos);
}

Material::Type RigidBody::GetMaterial(const dVec3& xWorldFrame) const
{
	const dVec3 xBodyFrame = (-m_state.q).Transform(xWorldFrame - m_state.x);
	const dVec3 xGeomFrame = (-m_geometry.rot).Transform(xBodyFrame - m_geometry.pos);
	return m_geometry.geom->GetMaterialLocal(xGeomFrame);
}

void RigidBody::SetPosition(const dVec3& x)
{
	m_state.x = x;
	UpdateAABB();
}
void RigidBody::SetRotation(const dQuat& q)
{
	m_state.q = q;
	UpdateDependentStateVariables();
	UpdateAABB();
}
void RigidBody::SetGeometry(Geometry* geometry, const dVec3& pos, const dQuat& rot)
{
	m_geometry.geom = geometry;
	m_geometry.pos = pos;
	m_geometry.rot = rot;
	UpdateAABB();
}
void RigidBody::SetMass(double m)
{
	m_inertia.m = m;
	m_inertia.minv = (m == 0.0) ? 0.0 : 1.0 / m_inertia.m;
}
void RigidBody::SetInertia(const dMat33& Ibody)
{
	m_inertia.Ibody = Ibody;

	int dim[3];
	int n = 0;

	for (int i = 0; i < 3; ++i)
	{
		if (Ibody(i, i) == 0.0)
		{
			m_inertia.Ibody.SetRow(i, dVec3(0.0, 0.0, 0.0));
			m_inertia.Ibody.SetColumn(i, dVec3(0.0, 0.0, 0.0));
			m_inertia.Ibodyinv.SetRow(i, dVec3(0.0, 0.0, 0.0));
			m_inertia.Ibodyinv.SetColumn(i, dVec3(0.0, 0.0, 0.0));
		}
		else
		{
			dim[n++] = 0;
		}
	}

	m_inertia.Ibody(1, 0) = m_inertia.Ibody(0, 1);
	m_inertia.Ibody(2, 0) = m_inertia.Ibody(0, 2);
	m_inertia.Ibody(2, 1) = m_inertia.Ibody(1, 2);

	switch (n)
	{
	case 1:
	{
		m_inertia.Ibodyinv(dim[0], dim[0]) = 1.0 / m_inertia.Ibody(dim[0], dim[0]);
		break;
	}
	case 2:
	{
		dMat22 I2;
		I2(dim[0], dim[0]) = m_inertia.Ibody(dim[0], dim[0]);
		I2(dim[0], dim[1]) = m_inertia.Ibody(dim[0], dim[1]);
		I2(dim[1], dim[0]) = m_inertia.Ibody(dim[1], dim[0]);
		I2(dim[1], dim[1]) = m_inertia.Ibody(dim[1], dim[1]);
		dMat22 I2inv = I2.Inverse();
		m_inertia.Ibodyinv(dim[0], dim[0]) = I2inv(dim[0], dim[0]);
		m_inertia.Ibodyinv(dim[0], dim[1]) = I2inv(dim[0], dim[1]);
		m_inertia.Ibodyinv(dim[1], dim[0]) = I2inv(dim[1], dim[0]);
		m_inertia.Ibodyinv(dim[1], dim[1]) = I2inv(dim[1], dim[1]);
		break;
	}
	case 3:
	{
		m_inertia.Ibodyinv = m_inertia.Ibody.Inverse();
		break;
	}
	}

	UpdateDependentStateVariables();
}
void RigidBody::SetInertia(const dQuat& principleAxes, const dVec3& inertia)
{
	const dMat33 R(principleAxes);
	dMat33 I;
	I.SetRow(0, dVec3(inertia.x, 0.0, 0.0));
	I.SetRow(1, dVec3(0.0, inertia.y, 0.0));
	I.SetRow(2, dVec3(0.0, 0.0, inertia.z));
	SetInertia(R.Transpose()*I*R);
}
void RigidBody::SetIsland(Island* island)
{
	m_island = island;
}
Island* RigidBody::GetIsland() const
{
	return m_island;
}

void RigidBody::ApplyBruteForce(Force* force)
{
	m_forces[m_nForces++] = force;
}
void RigidBody::ApplyForce(const dVec3& force, const dVec3& worldPos)
{
	m_F = m_F + force;
	m_T = m_T + (worldPos - m_state.x).Cross(force);

	assert(abs(m_F.x) < 100000.0);
	assert(abs(m_F.y) < 100000.0);
	assert(abs(m_F.z) < 100000.0);
	assert(abs(m_T.x) < 100000.0);
	assert(abs(m_T.y) < 100000.0);
	assert(abs(m_T.z) < 100000.0);
}
void RigidBody::ApplyImpulse(const dVec3& impulse, const dVec3& worldPos)
{
	m_dP = m_dP + impulse;
	m_dL = m_dL + (worldPos - m_state.x).Cross(impulse);

	assert(abs(m_dP.x) < 100000.0);
	assert(abs(m_dP.y) < 100000.0);
	assert(abs(m_dP.z) < 100000.0);
	assert(abs(m_dL.x) < 100000.0);
	assert(abs(m_dL.y) < 100000.0);
	assert(abs(m_dL.z) < 100000.0);
}

void RigidBody::ApplyLinImpulse_Immediate(const dVec3& linImpulse)
{
	if (m_inertia.minv > 0.0)
	{
		m_state.P = m_state.P + linImpulse;
		m_v = m_state.P.Scale(m_inertia.minv);
	}
}
void RigidBody::ApplyAngImpulse_Immediate(const dVec3& angImpulse)
{
	const dMat33& I = m_inertia.Ibodyinv;
	if (I(0, 0) + I(1, 1) + I(2, 2) > 0)
	{
		m_state.L = m_state.L + angImpulse;
		m_w = m_Iinv.Transform(m_state.L);
	}
}

void RigidBody::UpdateDependentStateVariables()
{
	dMat33 R(m_state.q);
	m_Iinv = R*m_inertia.Ibodyinv*R.Transpose();
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			const double x = (m_Iinv(i, j) + m_Iinv(j, i))*0.5;
			m_Iinv(i, j) = x;
			m_Iinv(j, i) = x;
		}
	}
	m_w = m_Iinv.Transform(m_state.L);
	m_v = m_state.P.Scale(m_inertia.minv);
}

void RigidBody::UpdateAABB()
{
	const dVec3 x = m_state.x;
	const dQuat q = m_state.q;

	const BoundingBox oobb = m_geometry.geom->GetLocalOOBB();
	const dVec3 oobbCenter = (oobb.min + oobb.max).Scale(0.5); // center of the OOBB in geom's local frame
	const dVec3 oobbSpan = (oobb.max - oobb.min).Scale(0.5); // span of the OOBB

	const dVec3 aabbCenter = x + q.Transform(m_geometry.pos + oobbCenter);
	const dQuat geomRot = q*m_geometry.rot;

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
	const dMat33 Iinv = R*m_inertia.Ibodyinv*R.Transpose();
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
void RigidBody::Compute_xDot(const dVec3& P, dVec3& xDot) const
{
	xDot = P.Scale(m_inertia.minv);
}

void RigidBody::ResolveImpulses()
{
	m_state.P = m_state.P + m_dP;
	m_state.L = m_state.L + m_dL;

	m_dP = dVec3(0.0, 0.0, 0.0);
	m_dL = dVec3(0.0, 0.0, 0.0);
}

void RigidBody::ResolveForces(double dt)
{
	auto NormalizeQuat = [](dQuat& q)
	{
		if (abs(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z - 1.0) > FLT_EPSILON)
		{
			q = q.Normalize();
		}
	};

	auto ZeroState = [](RigidBody::State& state)
	{
		state.x = dVec3(0.0, 0.0, 0.0);
		state.P = dVec3(0.0, 0.0, 0.0);
		state.L = dVec3(0.0, 0.0, 0.0);
		state.q = dQuat(0.0, 0.0, 0.0, 0.0);
	};
	auto ScaleState = [](const RigidBody::State& state, double scale)
	{
		RigidBody::State out = state;

		out.q.x = state.q.x * scale;
		out.q.y = state.q.y * scale;
		out.q.z = state.q.z * scale;
		out.q.w = state.q.w * scale;

		out.L = out.L.Scale(scale);

		out.x = out.x.Scale(scale);
		out.P = out.P.Scale(scale);

		return out;
	};
	auto AddStates = [](const RigidBody::State& state0, const RigidBody::State& state1)
	{
		auto AddQuats = [](const dQuat& q0, const dQuat& q1)
		{
			dQuat sum;
			sum.x = q0.x + q1.x;
			sum.y = q0.y + q1.y;
			sum.z = q0.z + q1.z;
			sum.w = q0.w + q1.w;
			return sum;
		};

		RigidBody::State state;
		state.q = AddQuats(state0.q, state1.q);
		state.L = state0.L + state1.L;
		state.x = state0.x + state1.x;
		state.P = state0.P + state1.P;

		return state;
	};

	RigidBody::State state = m_state;

	RigidBody::State stateDerivatives[4];
	const double dtRK4[3] = { 0.5*dt, 0.5*dt, 1.0*dt };

	int i = 0;

	while (true)
	{
		RigidBody::State& stateDerivative = stateDerivatives[i];
		ZeroState(stateDerivative);

		stateDerivative.P = m_F;
		stateDerivative.L = m_T;

		Compute_xDot(state.P, stateDerivative.x);
		Compute_qDot(state.q, state.L, stateDerivative.q);

		for (int j = 0; j < m_nForces; ++j)
		{
			dVec3 F, T;
			m_forces[j]->ComputeForceAndTorque(m_inertia, m_state, F, T);
			stateDerivative.P = stateDerivative.P + F;
			stateDerivative.L = stateDerivative.L + T;
		}

		if (i == 3)
		{
			break;
		}

		state = AddStates(m_state, ScaleState(stateDerivative, dtRK4[i]));
		NormalizeQuat(state.q);

		i++;
	}

	const double cRK4[4] = { 1.0 / 6.0, 2.0 / 6.0, 2.0 / 6.0, 1.0 / 6.0 };
	RigidBody::State dState;
	ZeroState(dState);
	for (int i = 0; i < 4; ++i)
	{
		dState = AddStates(dState, ScaleState(stateDerivatives[i], cRK4[i]));
	}
	m_state = AddStates(m_state, ScaleState(dState, dt));

	NormalizeQuat(m_state.q);

	UpdateDependentStateVariables();

	UpdateAABB();

	for (int i = 0; i < m_nForces; ++i)
	{
		delete m_forces[i];
	}
	m_nForces = 0;

	m_F = dVec3(0.0, 0.0, 0.0);
	m_T = dVec3(0.0, 0.0, 0.0);
}

void RigidBody::Damp(double dt)
{
	const double attenuationPerSec_P = 0.2;
	const double attenuationPerSec_L = 0.2;

	m_state.P = m_state.P.Scale(pow(1.0 - attenuationPerSec_P, dt));
	m_state.L = m_state.L.Scale(pow(1.0 - attenuationPerSec_L, dt));

	UpdateDependentStateVariables();
}

void RigidBody::Step(double dt)
{
	if (!m_awake)
	{
		return;
	}
	ResolveImpulses();
	ResolveForces(dt);
	Damp(dt);
}


