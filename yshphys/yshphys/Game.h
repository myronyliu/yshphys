#pragma once
#include "InputManager.h"
#include "RenderScene.h"
#include "PhysicsScene.h"
#include "GameNode.h"

#define MAX_GAME_NODES 1024

class Game
{
public:
	Game();
	virtual ~Game();

	void Run();

	void AddGameObject(GameObject* gameObject);
	void RemoveGameObject(GameObject* gameObject);

	InputManager m_inputHandler;

	PhysicsScene m_physicsScene;
	RenderScene m_renderScene;

protected:

	unsigned int m_dtPhys;
	unsigned int m_dtInput;

	unsigned int m_tRender;
	unsigned int m_tPhysics;

	unsigned int m_tInput;

	std::stack<FreedGameNode> m_freedNodeStack;
	GameNode m_gameNodes[MAX_PHYSICS_NODES];
	GameNode* m_firstNode;
};

