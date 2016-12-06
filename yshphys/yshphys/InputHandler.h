#pragma once

#include "MouseMotionHandler.h"
#include "KeyHandler.h"

class KeyHandler;

class KeyState
{
public:
	enum State
	{
		KEY_DOWN = 0,
		KEY_UP
	};
	State m_state;
	unsigned int m_duration; // This is sufficient for determining whether a state was toggled. If m_duration == 0 then we know that the key was just pressed
};

class InputHandler
{
public:
	InputHandler();
	virtual ~InputHandler();

	void ProcessEvents(bool& quitGameRequested);

	void AddMouseMotionHandler(MouseMotionHandler* mouseMotionHandler);
	void AddKeyHandler(KeyHandler* keyhandler);

protected:
	std::vector<MouseMotionHandler*> m_mouseMotionHandlers;
	std::vector<KeyHandler*> m_keyHandlers;

	unsigned int m_tEventPoll;
	std::map<int, KeyState> m_keyStates; // key: SDL keycode, value: duration for which the key was held down or up
};

