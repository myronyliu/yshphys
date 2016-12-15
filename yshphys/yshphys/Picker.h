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

	Game* m_game;

private:

	enum KeyActions
	{
		PICK = 0,

		N_KEY_ACTIONS
	};

	virtual unsigned int GetNumMappedKeys() const;

	float m_depth;

	struct DampedOscillatorCoefficients
	{
		float kOverM;
		float b;
	};

	DampedOscillatorCoefficients m_springCoeff;

	RigidBody* m_pickedObject;
};

