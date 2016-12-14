#include "stdafx.h"
#include "CameraPickerToggle.h"


CameraPickerToggle::CameraPickerToggle()
{
	m_mappedKeys[TOGGLE] = SDL_SCANCODE_LCTRL;
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

void CameraPickerToggle::ProcessKeyStates(KeyState* keyStates, int dt)
{
	if (m_camera == nullptr || m_picker == nullptr)
	{
		return;
	}

	if (keyStates[TOGGLE].m_state == KeyState::State::KEY_DOWN &&
		keyStates[TOGGLE].m_duration == 0)
	{
		if (m_cameraEnabled)
		{
			m_camera->DisableMouseMotionProcessing();
			m_picker->EnableMouseMotionProcessing();
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
		else
		{
			m_camera->EnableMouseMotionProcessing();
			m_picker->DisableMouseMotionProcessing();
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
		m_cameraEnabled = !m_cameraEnabled;
		
	}
}
