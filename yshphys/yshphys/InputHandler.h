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
	int GetMappedKeys(unsigned short* mappedKeys) const;

	virtual void ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms);

protected:

	virtual unsigned int GetNumMappedKeys() const;
	
	// For mappedKeys we use "short" since we don't want the InputHandler to distinguish between mouse button clicks and keyboard presses.
	// Since SDL uses 8 bits to encode the keyboard, using 16 bits ensures that we can assign a unique "short" to each of the mouse buttons.
	unsigned short m_mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];

	float m_xSens;
	float m_ySens;

	bool m_inputEnabled;
};