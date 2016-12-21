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

	RigidBody* GetPickedObject() const;
	dVec3 GetGrabOffset() const;
	dVec3 GetPosition() const;

	Game* m_game;

private:

	enum KeyActions
	{
		PICK = 0,

		N_KEY_ACTIONS
	};

	virtual unsigned int GetNumMappedKeys() const;

	float m_depth;

	DampedOscillatorCoefficients m_springCoeff; // assuming a unit mass

	RigidBody* m_pickedObject;
	dVec3 m_grabOffset;

	dVec3 m_pos;
};

