#include "stdafx.h"
#include "CameraPickerToggle.h"


CameraPickerToggle::CameraPickerToggle() : m_cameraEnabled(true)
{
	m_mappedKeys[TOGGLE] = YSH_INPUT_LCTRL;
}


CameraPickerToggle::~CameraPickerToggle()
{
}

void CameraPickerToggle::SetCamera(Camera* camera)
{
	m_camera = camera;
}
void CameraPickerToggle::SetPicker(Picker* picker)
{
	m_picker = picker;
}

unsigned int CameraPickerToggle::GetNumMappedKeys() const
{
	return CameraPickerToggle::KeyActions::N_KEY_ACTIONS;
}

void CameraPickerToggle::ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt)
{
	if (m_camera == nullptr || m_picker == nullptr)
	{
		return;
	}

	if (keyStates[TOGGLE].m_state == KeyState::State::PRESSED &&
		keyStates[TOGGLE].m_duration == 0)
	{
		if (m_cameraEnabled)
		{
			m_camera->DisableInput();
			m_picker->EnableInput();
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		else
		{
			m_camera->EnableInput();
			m_picker->DisableInput();
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		m_cameraEnabled = !m_cameraEnabled;

//		game->epa->Expand();
	}
}
