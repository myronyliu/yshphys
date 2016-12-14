#include "stdafx.h"
#include "InputHandler.h"


InputHandler::InputHandler() : m_inputEnabled (true), m_xSens(0.001f), m_ySens(0.001f)
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::EnableInput()
{
	m_inputEnabled = true;
}
void InputHandler::DisableInput()
{
	m_inputEnabled = false;
}
bool InputHandler::InputEnabled() const
{
	return m_inputEnabled;
}
unsigned int InputHandler::GetMappedKeys(int* mappedKeys) const
{
	std::memcpy(mappedKeys, m_mappedKeys, MAX_KEY_ACTIONS_PER_HANDLER * sizeof(int));
	return GetNumMappedKeys();
}

unsigned int InputHandler::GetNumMappedKeys() const
{
	return 0;
}

void InputHandler::ProcessInput(const MouseState& mouseState, KeyState* keyStates, int dt_ms)
{
}