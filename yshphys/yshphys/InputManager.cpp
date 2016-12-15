#include "stdafx.h"

#include "InputManager.h"
#include "InputHandler.h"

#include <SDL.h>

InputManager::InputManager() : m_quitRequested(false)
{
}


InputManager::~InputManager()
{
}

bool InputManager::QuitRequested() const
{
	return m_quitRequested;
}
void InputManager::AddInputHandler(InputHandler* inputHandler)
{
	unsigned short mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];
	const int nMappedKeys = inputHandler->GetMappedKeys(mappedKeys);
	for (int i = 0; i < nMappedKeys; ++i)
	{
		if (IsKeyboardInput(mappedKeys[i]))
		{
			if (m_keyboardState.find((Uint8)mappedKeys[i]) == m_keyboardState.end())
			{
				KeyState defaultKeyState;
				defaultKeyState.m_state = KeyState::State::RELEASED;
				defaultKeyState.m_duration = 0;
				defaultKeyState.m_prevDuration = 0;
				m_keyboardState[(Uint8)mappedKeys[i]] = defaultKeyState;
			}
		}
	}
	m_inputHandlers.push_back(inputHandler);
}

void InputManager::ProcessEvents(int dt)
{
	SDL_PumpEvents();

	UpdateKeyboardState(dt);
	UpdateMouseState(dt);

	// Now that we have populated the keystates, we can process the key and mouse-button presses/holds
	DispatchInputState(dt);
}
void InputManager::UpdateKeyboardState(int dt)
{
	const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
	for (std::map<Uint8, KeyState>::iterator it = m_keyboardState.begin(); it != m_keyboardState.end(); ++it)
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
void InputManager::UpdateMouseState(int dt)
{
	const Uint32 buttonMask = SDL_GetMouseState(&m_mouseState.m_x, &m_mouseState.m_y);

	SDL_GetWindowSize(m_window->m_window, &m_mouseState.m_windowSpanX, &m_mouseState.m_windowSpanY);

	KeyState* buttonState = &m_mouseState.m_leftButtonState;
	buttonState->m_duration += dt;

	if (buttonMask & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		if (buttonState->m_state == KeyState::State::RELEASED)
		{
			buttonState->m_state = KeyState::State::PRESSED;
			buttonState->m_prevDuration = buttonState->m_duration;
			buttonState->m_duration = 0;
		}
	}
	else
	{
		if (buttonState->m_state == KeyState::State::PRESSED)
		{
			buttonState->m_state = KeyState::State::RELEASED;
			buttonState->m_prevDuration = buttonState->m_duration;
			buttonState->m_duration = 0;
		}

	}

	buttonState = &m_mouseState.m_rightButtonState;
	buttonState->m_duration += dt;

	if (buttonMask & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		if (buttonState->m_state == KeyState::State::RELEASED)
		{
			buttonState->m_state = KeyState::State::PRESSED;
			buttonState->m_prevDuration = buttonState->m_duration;
			buttonState->m_duration = 0;
		}
	}
	else
	{
		if (buttonState->m_state == KeyState::State::PRESSED)
		{
			buttonState->m_state = KeyState::State::RELEASED;
			buttonState->m_prevDuration = buttonState->m_duration;
			buttonState->m_duration = 0;
		}

	}

	m_mouseState.m_xRel = 0;
	m_mouseState.m_yRel = 0;

	SDL_Event evt;

	while (SDL_PollEvent(&evt))
	{
		if (evt.type == SDL_QUIT)
		{
			m_quitRequested = true;
		}
		else if (evt.type == SDL_MOUSEMOTION)
		{
			m_mouseState.m_xRel += evt.motion.xrel;
			m_mouseState.m_yRel += evt.motion.yrel;
		}
	}
}

void InputManager::DispatchInputState(int dt) const
{
	for (std::vector<InputHandler*>::const_iterator it = m_inputHandlers.begin(); it != m_inputHandlers.end(); ++it)
	{
		InputHandler* handler = *it;
		unsigned short mappedKeys[MAX_KEY_ACTIONS_PER_HANDLER];
		const unsigned int nMappedKeys = handler->GetMappedKeys(mappedKeys);

		KeyState requestedStates[MAX_KEY_ACTIONS_PER_HANDLER];

		for (unsigned int i = 0; i < nMappedKeys; ++i)
		{
			if (IsKeyboardInput(mappedKeys[i]))
			{
				std::map<Uint8, KeyState>::const_iterator keyStateIt = m_keyboardState.find((Uint8)mappedKeys[i]);
				if (keyStateIt != m_keyboardState.end())
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
			else
			{
				switch (mappedKeys[i])
				{
				case YSH_INPUT_LMOUSEBUTTON:
					requestedStates[i] = m_mouseState.m_leftButtonState;
					break;
				case YSH_INPUT_RMOUSEBUTTON:
					requestedStates[i] = m_mouseState.m_rightButtonState;
					break;
				default:
					KeyState defaultKeyState;
					defaultKeyState.m_duration = 0;
					defaultKeyState.m_state = KeyState::State::RELEASED;
					requestedStates[i] = defaultKeyState;
					break;
				}
			}
		}

		if (handler->InputEnabled())
		{
			handler->ProcessInput(m_mouseState, requestedStates, dt);
		}
	}
}