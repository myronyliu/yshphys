#include "stdafx.h"
#include "Picker.h"


Picker::Picker() : m_depth(1.0f)
{
	m_mappedKeys[PICK] = YSH_INPUT_RMOUSEBUTTON;
}

Picker::~Picker()
{
}

unsigned int Picker::GetNumMappedKeys() const
{
	return KeyActions::N_KEY_ACTIONS;
}

void Picker::ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms)
{
	const KeyState& keyState = keyStates[PICK];

	if (keyState.m_state == KeyState::State::RELEASED)
	{
		if (m_pickedObject != nullptr)
		{
			m_pickedObject->m_F = dVec3(0.0, 0.0, 0.0);
			m_pickedObject = nullptr;
		}
	}
	if (keyState.m_state == KeyState::State::PRESSED)
	{
		const Viewport& viewport = m_game->m_renderScene.m_viewport;
		const fVec3 viewDir = viewport.GetViewDir();
		const Ray ray = viewport.UnProject(mouseState.m_x, mouseState.m_y, mouseState.m_windowSpanX, mouseState.m_windowSpanY);

		if (keyState.m_duration == 0)
		{

		}
		else if (m_pickedObject != nullptr)
		{
			double mInv;
			double m = m_pickedObject->GetMass(mInv);

			dVec3 x = m_pickedObject->GetPosition();
			dVec3 v = m_pickedObject->GetLinearVelocity();

			const fVec3 rayDir = ray.GetDirection();

			dVec3 pickerPos = ray.GetOrigin() + rayDir.Scale(m_depth / rayDir.Dot(viewDir));

			m_pickedObject->m_F = ((pickerPos - x) - v).Scale(m);
		}
	}
}