#pragma once
#include "InputHandler.h"
#include "RenderScene.h"
class Game
{
public:
	Game();
	virtual ~Game();

	void Run();

	InputHandler m_inputHandler;
	RenderScene* m_renderScene;

protected:
};

