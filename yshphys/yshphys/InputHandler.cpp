#include "stdafx.h"
#include "InputHandler.h"
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
	m_keyHandlers.push_back(keyHandler);
}

void InputHandler::ProcessEvents(bool& quitGameRequested)
{
	Uint32 tLastEventPoll = m_tEventPoll;

	m_tEventPoll = SDL_GetTicks();

	Uint32 dt = m_tEventPoll - tLastEventPoll;

	quitGameRequested = false;

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
				(*it)->ProcessMouseRelativeMotion((float)evt.motion.xrel, (float)evt.motion.yrel);
			}
		}
		else if (evt.type == SDL_KEYDOWN)
		{
			KeyState& keyState = m_keyStates[evt.key.keysym.sym];
			if (keyState.m_state == KeyState::State::KEY_DOWN)
			{
				keyState.m_duration += dt;
			}
			else
			{
				keyState.m_state = KeyState::State::KEY_DOWN;
				keyState.m_duration = 0;
			}
		}
		else if (evt.type == SDL_KEYUP)
		{
			KeyState& keyState = m_keyStates[evt.key.keysym.sym];
			if (keyState.m_state == KeyState::State::KEY_UP)
			{
				keyState.m_duration += dt;
			}
			else
			{
				keyState.m_state = KeyState::State::KEY_UP;
				keyState.m_duration = 0;
			}
		}
	}

	// Now that we have populated the keystates, we can process the key holds

	for (std::vector<KeyHandler*>::iterator it = m_keyHandlers.begin(); it != m_keyHandlers.end(); ++it)
	{
		KeyHandler* handler = *it;
		int mappedKeys[MAX_KEYHOLD_ACTIONS_PER_HANDLER];
		const unsigned int nMappedKeys = handler->GetMappedKeys(mappedKeys);

		KeyState requestedStates[MAX_KEYHOLD_ACTIONS_PER_HANDLER];

		for (unsigned int i = 0; i < nMappedKeys; ++i)
		{
			std::map<int, KeyState>::iterator keyStateIt = m_keyStates.find(mappedKeys[i]);
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

		handler->ProcessKeyStates(requestedStates);
	}
}