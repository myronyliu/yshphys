#include "stdafx.h"
#include "Game.h"

#include <SDL.h>

Game::Game()
{
}


Game::~Game()
{
}


void Game::Run()
{
	bool quit = 0;

	while (!quit)
	{
		m_renderScene->DrawBox(fVec3(0.0f, 0.0f, 0.0f), fQuat(0.0f, 0.0f, 0.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f), true, 2.0f, 2.0f, 2.0f);
		m_inputHandler.ProcessEvents(quit);
		m_renderScene->DrawScene();
		SDL_Delay(16);
	}
}