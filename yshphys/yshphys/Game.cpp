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
		m_inputHandler.ProcessEvents(quit);
		m_renderScene->DrawScene();
		SDL_Delay(16);
	}
}