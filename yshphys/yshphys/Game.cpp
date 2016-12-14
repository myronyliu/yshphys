#include "stdafx.h"
#include "Game.h"

#include <SDL.h>

Game::Game() :
	m_dtPhys(33), m_tPhysics(0),
	m_dtInput(33), m_tInput(0),
	m_firstNode(nullptr)
{
	for (int i = 0; i < MAX_GAME_NODES - 1; ++i)
	{
		m_gameNodes[i].PrependTo(&m_gameNodes[i + 1]);
	}

	for (int i = MAX_PHYSICS_NODES- 1; i > 0; --i)
	{
		FreedGameNode freeNode;
		freeNode.m_node = &m_gameNodes[i];
		freeNode.m_precedingNode = &m_gameNodes[i - 1];
		m_freedNodeStack.push(freeNode);
	}
	FreedGameNode freeNode;
	freeNode.m_node = &m_gameNodes[0];
	freeNode.m_precedingNode = nullptr;
	m_freedNodeStack.push(freeNode);
}


Game::~Game()
{
}

void Game::AddGameObject(GameObject* gameObject)
{
	if (!m_freedNodeStack.empty())
	{
		FreedGameNode& freeNode = m_freedNodeStack.top();
		freeNode.m_node->BindGameObject(gameObject);

		if (freeNode.m_precedingNode)
		{
			freeNode.m_node->AppendTo(freeNode.m_precedingNode);
		}
		else
		{
			freeNode.m_node->PrependTo(m_firstNode);
			m_firstNode = freeNode.m_node;
		}

		m_freedNodeStack.pop();

		if (PhysicsObject* physicsObject = gameObject->GetPhysicsObject())
		{
			m_physicsScene.AddPhysicsObject(physicsObject);
		}
		if (RenderObject* renderObject = gameObject->GetRenderObject())
		{
			m_renderScene.AddRenderObject(renderObject);
		}
	}
}

void Game::RemoveGameObject(GameObject* gameObject)
{
	if (GameNode* node = gameObject->GetGameNode())
	{
		if (node->GetPrev() == nullptr && node->GetNext() != nullptr)
		{
			m_firstNode = node->GetNext();
		}
		node->BindGameObject(nullptr);
		FreedGameNode freeNode;
		freeNode.m_precedingNode = node->GetPrev();
		freeNode.m_node = node;
		node->Remove();
		m_freedNodeStack.push(freeNode);
	}
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

//			m_renderScene.DebugDrawSystem().DrawBox(2.0f, 2.0f, 2.0f, fVec3(0.0f, 0.0f, 0.0f), fQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), true);

			// RenderScene and PhysicsScene should know nothing about one another. All the coupling should be handled by Game and GameObject
			GameNode* node = m_firstNode;
			while (node != nullptr)
			{
				node->GetGameObject()->PropagatePhysicsTransform();
				node = node->GetNext();
			}
			m_renderScene.DebugDrawSystem().DrawBVTree(m_physicsScene.GetBVTree(), fVec3(1.0f, 1.0f, 1.0f));
			
			m_renderScene.DrawScene();

			m_tPhysics = t;
		}
	}
}