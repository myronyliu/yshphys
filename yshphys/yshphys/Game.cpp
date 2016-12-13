#include "stdafx.h"
#include "Game.h"

#include <SDL.h>

Game::Game() :
	m_dtPhys(33), m_tPhysics(0),
	m_dtInput(33), m_tInput(0)
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
		Uint32 t = SDL_GetTicks();

		if (t - m_tInput > m_dtInput)
		{
			m_inputHandler.ProcessEvents(quit);
			m_tInput = t;
		}
		if (t - m_tPhysics > m_dtPhys)
		{
			m_physicsScene.Step((double)m_dtPhys*0.001);

			m_renderScene.DebugDrawSystem().DrawBox(2.0f, 2.0f, 2.0f, fVec3(0.0f, 0.0f, 0.0f), fQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), true);

			// RenderScene and PhysicsScene should know nothing about one another. All the coupling should be handled by Game and GameObject
			GameNode* node = m_firstNode;
			while (node != nullptr)
			{
				node->GetGameObject()->PropagatePhysicsTransform();
				node = node->GetNext();
			}
			m_renderScene.DrawScene();

			m_tPhysics = t;
		}
	}
}