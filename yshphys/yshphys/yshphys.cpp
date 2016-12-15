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
	game.m_inputHandler.m_window = &window;
	game.m_inputHandler.AddInputHandler(&camera);
	game.m_inputHandler.AddInputHandler(&picker);
	game.m_inputHandler.AddInputHandler(&toggle);
	game.m_renderScene.AttachCamera(&camera);
	game.m_renderScene.m_window = &window;

	RenderMesh mesh;
	mesh.CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
	Shader_Default shader;
	RenderObject obj;
	obj.SetRenderMesh(&mesh);
	obj.SetShader(&shader);

//	RigidBody rigidBody;
//	Capsule capsule;
//	capsule.SetRadius(1.0);
//	capsule.SetHalfHeight(1.0);
//	rigidBody.SetGeometry(&capsule);
//	rigidBody.m_F = dVec3(0.0, 0.0, 0.1);
//	rigidBody.m_T = dVec3(0.0, 0.1, 0.0);
//	rigidBody.SetMass(1.0);
//	rigidBody.SetInertia(dMat33::Identity());
//	
//	GameObject gameObject;
//	gameObject.SetPhysicsObject(&rigidBody);
//	gameObject.SetRenderObject(&obj);
//
//	game.AddGameObject(&gameObject);

	Tests::CreateBVTest(&game);


	game.Run();

	Box box1;
	Box box2;
	box1.SetDimensions(1, 1, 1);
	box2.SetDimensions(1, 1, 1);
	box1.SetPosition(dVec3(0.0, 0.0, 0.0));
	box2.SetPosition(dVec3(3.0, 0.0, 0.0));
	box1.SetRotation(dQuat(0.0, 0.0, 0.0, 1.0));
	box2.SetRotation(dQuat(0.0, 0.0, 0.0, 1.0));
	dVec3 pt1;
	dVec3 pt2;
	dVec3 pt3;
	dVec3 pt4;
	double asdf = box1.ComputeSeparation(&box2, pt1, pt2);
	double qwer = box2.ComputeSeparation(&box1, pt3, pt4);
	
	SDL_Quit();

    return 0;
}

