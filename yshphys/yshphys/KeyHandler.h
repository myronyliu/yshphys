#pragma once
#include "InputHandler.h"
#include <SDL.h>

#define MAX_KEYHOLD_ACTIONS_PER_HANDLER 8

class KeyState;

class KeyHandler
{
public:
	KeyHandler();
	virtual ~KeyHandler();

	// pointer argument is filled with SDL_keycodes
	// return value is the number of keyhold actions
	unsigned int GetMappedKeys(int* mappedKeys) const;
	virtual void ProcessKeyStates(KeyState* keyStates);
	
	virtual unsigned int GetNumMappedKeys() const;

protected:
	int m_mappedKeys[MAX_KEYHOLD_ACTIONS_PER_HANDLER];
};