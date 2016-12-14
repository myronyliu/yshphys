#include "stdafx.h"
#include "KeyHandler.h"


KeyHandler::KeyHandler() : m_keyProcessingEnabled (true)
{
}

KeyHandler::~KeyHandler()
{
}

void KeyHandler::EnableKeyProcessing()
{
	m_keyProcessingEnabled = true;
}
void KeyHandler::DisableKeyProcessing()
{
	m_keyProcessingEnabled = false;
}

bool KeyHandler::KeyProcessingEnabled() const
{
	return m_keyProcessingEnabled;
}

unsigned int KeyHandler::GetMappedKeys(int* mappedKeys) const
{
	std::memcpy(mappedKeys, m_mappedKeys, MAX_KEY_ACTIONS_PER_HANDLER * sizeof(int));
	return GetNumMappedKeys();
}

unsigned int KeyHandler::GetNumMappedKeys() const
{
	return 0;
}

void KeyHandler::ConditionalProcessKeyStates(KeyState* keyStates, int dt_ms)
{
	if (m_keyProcessingEnabled)
	{
		ProcessKeyStates(keyStates, dt_ms);
	}
}

void KeyHandler::ProcessKeyStates(KeyState* keyStates, int dt_ms)
{
}