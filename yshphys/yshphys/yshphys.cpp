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
	window.CreateWindow(88, 88, 1200, 900);

	Camera camera;
	Picker picker;
	CameraPickerToggle toggle;
	toggle.SetCamera(&camera);
	toggle.SetPicker(&picker);

	Game game;
	picker.m_game = &game;
	toggle.game = &game;
	game.m_inputManager.AddInputHandler(&camera);
	game.m_inputManager.AddInputHandler(&picker);
	game.m_inputManager.AddInputHandler(&toggle);
	game.m_renderScene.AttachCamera(&camera);
	game.m_window = &window;

	PointLight pl;
	pl.intensity = fVec3(1.0f, 1.0f, 1.0f).Scale(2.0f);
	pl.position = fVec3(0.0f, 0.0f, 8.0f - 16.0f);
	pl.shadowCubeMap.Init(1024);
	game.m_renderScene.AddPointLight(pl);

	game.m_renderScene.DebugDrawSystem().m_picker = &picker;

	Tests::CreateBVTest(&game);
//	Tests::CreateGJKTest(&game);

	game.Run();

    return 0;
}

