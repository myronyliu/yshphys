#pragma once

#include "KeyButtonRenamings.h"
#include "Window.h"
#include <SDL.h>

class InputHandler;

struct KeyState
{
	enum State
	{
		PRESSED = 0,
		RELEASED	
	};

	KeyState() : m_state(KeyState::State::RELEASED), m_duration(0), m_prevDuration(0) {}

	State m_state;
	int m_duration; // This is sufficient for determining whether a state was toggled. If m_duration == 0 then we know that the key was just pressed

	// Duration of the previous (hence opposite) state
	// e.g. if m_state == KEY_DOWN, then m_prevDuration is the duration for which m_state == KEY_UP before this key was pressed
	int m_prevDuration;
};

struct MouseState
{
	MouseState() : m_x(0), m_y(0), m_windowSpanX(0), m_windowSpanY(0), m_leftButtonState(), m_rightButtonState() {}

	KeyState m_leftButtonState;
	KeyState m_rightButtonState;

	int m_x;
	int m_y;

	int m_windowSpanX;
	int m_windowSpanY;

	int m_xRel;
	int m_yRel;
};

class InputManager
{
public:
	InputManager();
	virtual ~InputManager();

	void ProcessEvents(int dt_ms);

	void AddInputHandler(InputHandler* inputHandler);

	bool QuitRequested() const;

	Window* m_window;

protected:

	void UpdateKeyboardState(int dt_ms);
	void UpdateMouseState(int dt_ms);

	void DispatchInputState(int dt_ms) const;

	std::vector<InputHandler*> m_inputHandlers;

	std::map<Uint8, KeyState> m_keyboardState; // key: SDL keycode, value: duration for which the key was held down or up
	MouseState m_mouseState;

	bool m_quitRequested;
};

