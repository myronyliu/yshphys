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
		m_renderScene->DebugDrawSystem().DrawBox(2.0f, 2.0f, 2.0f, fVec3(0.0f, 0.0f, 0.0f), fQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), true);
		m_inputHandler.ProcessEvents(quit);
		m_renderScene->DrawScene();
		SDL_Delay(16);
	}
}