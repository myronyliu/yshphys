#include "stdafx.h"
#include "KeyHandler.h"


KeyHandler::KeyHandler()
{
}


KeyHandler::~KeyHandler()
{
}

unsigned int KeyHandler::GetMappedKeys(int* mappedKeys) const
{
	std::memcpy(mappedKeys, m_mappedKeys, MAX_KEYHOLD_ACTIONS_PER_HANDLER * sizeof(int));
	return GetNumMappedKeys();
}

unsigned int KeyHandler::GetNumMappedKeys() const
{
	return 0;
}

void KeyHandler::ProcessKeyStates(KeyState* keyStates)
{
}