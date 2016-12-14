#pragma once
#include "InputHandler.h"
#include "Camera.h"
#include "Picker.h"

class CameraPickerToggle :
	public InputHandler
{
public:
	enum KeyActions
	{
		TOGGLE = 0,

		N_KEY_ACTIONS
	};
	CameraPickerToggle();
	virtual ~CameraPickerToggle();

	void SetCamera(Camera* camera);
	void SetPicker(Picker* picker);

	virtual void ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms);

protected:
	virtual unsigned int GetNumMappedKeys() const;

	bool m_cameraEnabled;

	Camera* m_camera;
	Picker* m_picker;
};

