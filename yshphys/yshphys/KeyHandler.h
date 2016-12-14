#pragma once
#include "InputHandler.h"
#include <SDL.h>

#define MAX_KEY_ACTIONS_PER_HANDLER 8

struct KeyState;

class KeyHandler
{
public:
	KeyHandler();
	virtual ~KeyHandler();

	void EnableKeyProcessing();
	void DisableKeyProcessing();
	
	bool KeyProcessingEnabled() const;

	void ConditionalProcessKeyStates(KeyState* keyStates, int dt_ms);

	// pointer argument is filled with SDL_keycodes
	// return value is the number of keyhold actions
	unsigned int GetMappedKeys(int* mappedKeys) const;

protected:

	virtual void ProcessKeyStates(KeyState* keyStates, int dt_ms);
	virtual unsigned int GetNumMappedKeys() const;
	
	int m_mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];

	bool m_keyProcessingEnabled;
};