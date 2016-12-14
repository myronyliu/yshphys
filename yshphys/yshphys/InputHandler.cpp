#include "stdafx.h"

#include "InputHandler.h"
#include "MouseMotionHandler.h"
#include "KeyHandler.h"

#include <SDL.h>

InputHandler::InputHandler() : m_quitRequested(false)
{
}


InputHandler::~InputHandler()
{
}

bool InputHandler::QuitRequested() const
{
	return m_quitRequested;
}

void InputHandler::AddMouseMotionHandler(MouseMotionHandler* mouseMotionHandler)
{
	m_mouseMotionHandlers.push_back(mouseMotionHandler);
}
void InputHandler::AddKeyHandler(KeyHandler* keyHandler)
{
	int mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];
	const int nMappedKeys = keyHandler->GetMappedKeys(mappedKeys);
	for (int i = 0; i < nMappedKeys; ++i)
	{
		if (m_keyStates.find(mappedKeys[i]) == m_keyStates.end())
		{
			KeyState defaultKeyState;
			defaultKeyState.m_state = KeyState::State::RELEASED;
			defaultKeyState.m_duration = 0;
			defaultKeyState.m_prevDuration = 0;
			m_keyStates[mappedKeys[i]] = defaultKeyState;
		}
	}
	m_keyHandlers.push_back(keyHandler);
}

void InputHandler::ProcessEvents(int dt)
{
	SDL_PumpEvents();

	UpdateKeyStates(dt);
	UpdateMouseState(dt);
	UpdateRelativeMouseMotion();

	// Now that we have populated the keystates, we can process the key and mouse-button presses/holds
	DispatchKeyStates(dt);
	DispatchMouseMotion(dt);
}

void InputHandler::UpdateRelativeMouseMotion()
{
	m_xRel = 0;
	m_yRel = 0;

	SDL_Event evt;

	while (SDL_PollEvent(&evt))
	{
		if (evt.type == SDL_QUIT)
		{
			m_quitRequested = true;
		}
		else if (evt.type == SDL_MOUSEMOTION)
		{
			m_xRel += evt.motion.xrel;
			m_yRel += evt.motion.yrel;
		}
	}
}
void InputHandler::UpdateKeyStates(int dt)
{
	const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
	for (std::map<int, KeyState>::iterator it = m_keyStates.begin(); it != m_keyStates.end(); ++it)
	{
		const int& scanCode = it->first;
		KeyState& keyState = it->second;

		keyState.m_duration += dt;

		if (keyboard[scanCode])
		{
			if (keyState.m_state == KeyState::State::RELEASED)
			{
				keyState.m_state = KeyState::State::PRESSED;
				keyState.m_prevDuration = keyState.m_duration;
				keyState.m_duration = 0;
			}
		}
		else
		{
			if (keyState.m_state == KeyState::State::PRESSED)
			{
				keyState.m_state = KeyState::State::RELEASED;
				keyState.m_prevDuration = keyState.m_duration;
				keyState.m_duration = 0;
			}
		}
	}
}
void InputHandler::UpdateMouseState(int dt)
{
	int x, y;
	const Uint32 buttonMask = SDL_GetMouseState(&x, &y);

	KeyState& buttonState = m_mouseState.m_leftButtonState;
	buttonState.m_duration += dt;

	if (buttonMask & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (buttonState.m_state == KeyState::State::RELEASED)
		{
			buttonState.m_state = KeyState::State::PRESSED;
			buttonState.m_prevDuration = buttonState.m_duration;
			buttonState.m_duration = 0;
		}
	}
	else
	{
		if (buttonState.m_state == KeyState::State::PRESSED)
		{
			buttonState.m_state = KeyState::State::RELEASED;
			buttonState.m_prevDuration = buttonState.m_duration;
			buttonState.m_duration = 0;
		}

	}

	buttonState = m_mouseState.m_rightButtonState;
	buttonState.m_duration += dt;

	if (buttonMask & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		if (buttonState.m_state == KeyState::State::RELEASED)
		{
			buttonState.m_state = KeyState::State::PRESSED;
			buttonState.m_prevDuration = buttonState.m_duration;
			buttonState.m_duration = 0;
		}
	}
	else
	{
		if (buttonState.m_state == KeyState::State::PRESSED)
		{
			buttonState.m_state = KeyState::State::RELEASED;
			buttonState.m_prevDuration = buttonState.m_duration;
			buttonState.m_duration = 0;
		}

	}
}

void InputHandler::DispatchMouseMotion(int dt) const
{
	for (std::vector<MouseMotionHandler*>::const_iterator it = m_mouseMotionHandlers.begin(); it != m_mouseMotionHandlers.end(); ++it)
	{
		(*it)->ConditionalProcessMouseRelativeMotion((float)m_xRel, (float)m_yRel);
	}

}
void InputHandler::DispatchKeyStates(int dt) const
{
	for (std::vector<KeyHandler*>::const_iterator it = m_keyHandlers.begin(); it != m_keyHandlers.end(); ++it)
	{
		KeyHandler* handler = *it;
		int mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];
		const unsigned int nMappedKeys = handler->GetMappedKeys(mappedKeys);

		KeyState requestedStates[MAX_KEY_ACTIONS_PER_HANDLER];

		for (unsigned int i = 0; i < nMappedKeys; ++i)
		{
			std::map<int, KeyState>::const_iterator keyStateIt = m_keyStates.find(mappedKeys[i]);
			if (keyStateIt != m_keyStates.end())
			{
				requestedStates[i] = keyStateIt->second;
			}
			else
			{
				KeyState defaultKeyState;
				defaultKeyState.m_duration = 0;
				defaultKeyState.m_state = KeyState::State::RELEASED;
				requestedStates[i] = defaultKeyState;
			}
		}

		handler->ConditionalProcessKeyStates(requestedStates, dt);
	}
}