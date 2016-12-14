#include "stdafx.h"

#include "InputHandler.h"
#include "MouseMotionHandler.h"
#include "KeyHandler.h"

#include <SDL.h>

InputHandler::InputHandler()
{
}


InputHandler::~InputHandler()
{
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
			defaultKeyState.m_state = KeyState::State::KEY_DOWN;
			defaultKeyState.m_duration = 0;
			defaultKeyState.m_prevDuration = 0;
			m_keyStates[mappedKeys[i]] = defaultKeyState;
		}
	}
	m_keyHandlers.push_back(keyHandler);
}

static int xxx = 0;

void InputHandler::ProcessEvents(int dt, bool& quitGameRequested)
{
	quitGameRequested = false;

	SDL_PumpEvents();
	const Uint8* keyboard = SDL_GetKeyboardState(nullptr);

	for (std::map<int, KeyState>::iterator it = m_keyStates.begin(); it != m_keyStates.end(); ++it)
	{
		const int& scanCode = it->first;
		KeyState& keyState = it->second;

		keyState.m_duration += dt;

		if (keyboard[scanCode])
		{
			if (keyState.m_state == KeyState::State::KEY_UP)
			{
				keyState.m_state = KeyState::State::KEY_DOWN;
				keyState.m_prevDuration = keyState.m_duration;
				keyState.m_duration = 0;
			}
		}
		else
		{
			if (keyState.m_state == KeyState::State::KEY_DOWN)
			{
				keyState.m_state = KeyState::State::KEY_UP;
				keyState.m_prevDuration = keyState.m_duration;
				keyState.m_duration = 0;
			}
		}
	}

	SDL_Event evt;
	while (SDL_PollEvent(&evt))
	{
		if (evt.type == SDL_QUIT)
		{
			quitGameRequested = true;
		}
		else if (evt.type == SDL_MOUSEMOTION)
		{
			for (std::vector<MouseMotionHandler*>::iterator it = m_mouseMotionHandlers.begin(); it != m_mouseMotionHandlers.end(); ++it)
			{
				(*it)->ConditionalProcessMouseRelativeMotion((float)evt.motion.xrel, (float)evt.motion.yrel);
			}
		}
		else if (evt.type == SDL_MOUSEBUTTONDOWN)
		{
		}
		else if (evt.type == SDL_MOUSEBUTTONUP)
		{
		}
	}

	// Now that we have populated the keystates, we can process the key and mouse-button presses/holds
	DispatchKeyStates(dt);
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
				defaultKeyState.m_state = KeyState::State::KEY_UP;
				requestedStates[i] = defaultKeyState;
			}
		}

		handler->ConditionalProcessKeyStates(requestedStates, dt);
	}
}