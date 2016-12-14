#pragma once

class KeyHandler;
class MouseMotionHandler;

class KeyState
{
public:
	enum State
	{
		KEY_DOWN = 0,
		KEY_UP
	};

	KeyState() : m_state(State::KEY_UP), m_duration(0), m_prevDuration(0) {}

	State m_state;
	int m_duration; // This is sufficient for determining whether a state was toggled. If m_duration == 0 then we know that the key was just pressed

	// Duration of the previous (hence opposite) state
	// e.g. if m_state == KEY_DOWN, then m_prevDuration is the duration for which m_state == KEY_UP before this key was pressed
	int m_prevDuration;
};

class InputHandler
{
public:
	InputHandler();
	virtual ~InputHandler();

	void ProcessEvents(int dt_ms, bool& quitGameRequested);

	void AddMouseMotionHandler(MouseMotionHandler* mouseMotionHandler);
	void AddKeyHandler(KeyHandler* keyhandler);

protected:

	void DispatchKeyStates(int dt_ms) const;

	std::vector<MouseMotionHandler*> m_mouseMotionHandlers;
	std::vector<KeyHandler*> m_keyHandlers;

	std::map<int, KeyState> m_keyStates; // key: SDL keycode, value: duration for which the key was held down or up
};

