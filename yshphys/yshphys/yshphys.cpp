// yshphys.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Box.h"
#include "Geometry.h"

#include <glew.h>
#include "Window.h"
#include "RenderScene.h"
#include "RigidBody.h"
#include "Shader_Default.h"
#include "Camera.h"
#include "InputManager.h"
#include "Game.h"
#include "Capsule.h"
#include "Tests.h"
#include "Picker.h"
#include "CameraPickerToggle.h"

int main(int argc, char *args[])
{
	Window window;
	window.CreateWindow();

	Camera camera;
	Picker picker;
	CameraPickerToggle toggle;
	toggle.SetCamera(&camera);
	toggle.SetPicker(&picker);

	Game game;
	picker.m_game = &game;
	game.m_inputManager.AddInputHandler(&camera);
	game.m_inputManager.AddInputHandler(&picker);
	game.m_inputManager.AddInputHandler(&toggle);
	game.m_renderScene.AttachCamera(&camera);
	game.m_window = &window;

	game.m_renderScene.DebugDrawSystem().m_picker = &picker;

//	Tests::CreateBVTest(&game);
	Tests::CreateGJKTest(&game);

	game.Run();

    return 0;
}

