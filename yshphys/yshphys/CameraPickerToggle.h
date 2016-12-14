#pragma once
#include "KeyHandler.h"
#include "Camera.h"
#include "Picker.h"

class CameraPickerToggle :
	public KeyHandler
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

	virtual unsigned int GetNumMappedKeys() const;

protected:

	virtual void ProcessKeyStates(KeyState* keyStates, int dt_ms);

	bool m_cameraEnabled;

	Camera* m_camera;
	Picker* m_picker;
};

