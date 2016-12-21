#include "stdafx.h"
#include "Picker.h"


Picker::Picker() : m_depth(1.0f)
{
	m_mappedKeys[PICK] = YSH_INPUT_LMOUSEBUTTON;

	m_springCoeff.k = 10.0f;
	m_springCoeff.b = 2 * sqrt(m_springCoeff.k);
}

Picker::~Picker()
{
}

unsigned int Picker::GetNumMappedKeys() const
{
	return KeyActions::N_KEY_ACTIONS;
}

RigidBody* Picker::GetPickedObject() const
{
	return m_pickedObject;
}

dVec3 Picker::GetGrabOffset() const
{
	return m_grabOffset;
}

dVec3 Picker::GetPosition() const
{
	return m_pos;
}


void Picker::ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms)
{
	const KeyState& keyState = keyStates[PICK];

	if (keyState.m_state == KeyState::State::RELEASED)
	{
		m_pickedObject = nullptr;
	}
	if (keyState.m_state == KeyState::State::PRESSED)
	{
		const Viewport& viewport = m_game->m_renderScene.m_viewport;
		const fVec3 viewDir = viewport.GetViewDir();
		const Ray ray = viewport.UnProject(mouseState.m_x, mouseState.m_y, mouseState.m_windowSpanX, mouseState.m_windowSpanY);

		const fVec3 rayDir = ray.GetDirection();

		if (keyState.m_duration == 0)
		{
			PhysicsRayCastHit hit = m_game->m_physicsScene.RayCast(ray);
			if (m_pickedObject = hit.body)
			{
				m_grabOffset = hit.offset;
				dVec3 x = m_pickedObject->GetPosition() + m_pickedObject->GetRotation().Transform(m_grabOffset);
				m_depth = fVec3(x - ray.GetOrigin()).Dot(viewDir);

				m_pos = ray.GetOrigin() + rayDir.Scale(m_depth / rayDir.Dot(viewDir));
			}
		}
		else if (m_pickedObject != nullptr)
		{
			double mInv;
			double m = m_pickedObject->GetMass(mInv);

			const dVec3 aaGrabOffset = m_pickedObject->GetRotation().Transform(m_grabOffset);
			const dVec3 x = m_pickedObject->GetPosition() + aaGrabOffset;
			const dVec3 v = m_pickedObject->GetLinearVelocity() + m_pickedObject->GetAngularVelocity().Cross(aaGrabOffset);

			m_pos = ray.GetOrigin() + rayDir.Scale(m_depth / rayDir.Dot(viewDir));

			const dVec3 compression = m_pos - x;
			const double compressionSqr = compression.Dot(compression);
			const dVec3 compressionRate = (compressionSqr < 0.0001) ? dVec3(0.0, 0.0, 0.0) : compression.Scale(v.Dot(compression) / compressionSqr);

//			dVec3 force = (compression.Scale(m_springCoeff.k) - compressionRate.Scale(m_springCoeff.b)).Scale(m);
			dVec3 force = (compression.Scale(m_springCoeff.k) - v.Scale(m_springCoeff.b)).Scale(m);

			m_pickedObject->ApplyForce(force, x);
//			m_pickedObject->ApplyForceAtCenterOfMass(force);
		}
	}
}