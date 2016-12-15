#pragma once
#include "InputHandler.h"
#include "RigidBody.h"
#include "Game.h"

class Picker :
	public InputHandler
{
public:
	Picker();
	virtual ~Picker();

	virtual void ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms);

private:

	enum KeyActions
	{
		PICK = 0,

		N_KEY_ACTIONS
	};

	virtual unsigned int GetNumMappedKeys() const;

	float m_depth;

	Game* m_game;
	RigidBody* m_pickedObject;
};

