#pragma once
#include "InputManager.h"
#include <SDL.h>

#define MAX_KEY_ACTIONS_PER_HANDLER 8

struct KeyState;

class InputHandler
{
public:
	InputHandler();
	virtual ~InputHandler();

	void EnableInput();
	void DisableInput();
	
	bool InputEnabled() const;

	// pointer argument is filled with SDL_keycodes
	// return value is the number of keyhold actions
	unsigned int GetMappedKeys(int* mappedKeys) const;

	virtual void ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms);

protected:

	virtual unsigned int GetNumMappedKeys() const;
	
	int m_mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];

	float m_xSens;
	float m_ySens;

	bool m_inputEnabled;
};