#include "stdafx.h"
#include "Tests.h"
#include "RigidBody.h"
#include "Capsule.h"
#include "Shader.h"
#include "Shader_Default.h"
#include "Cylinder.h"
#include "Box.h"

void Tests::CreateBVTest(Game* game)
{
	Shader_Default* shader = new Shader_Default;
//	Capsule* geometry = new Capsule();
	Cylinder* geometry = new Cylinder();
	const double r = 1.0;
	const double h = 2.0;
	geometry->SetRadius(r);
	geometry->SetHalfHeight(h);

//	Box* geometry = new Box();
//	geometry->SetDimensions(1.0, 1.0, 1.0);

//	dVec3 sceneCenter = dVec3(8.0, 0.0, 0.0);
	dVec3 sceneCenter = dVec3(0.0, 0.0, 0.0);
	dVec3 sceneHalfDim = dVec3(1.0, 1.0, 1.0).Scale(8.0);
	dVec3 sceneMin = sceneCenter - sceneHalfDim;
	dVec3 sceneMax = sceneCenter + sceneHalfDim;

	for (int i = 0; i < 8; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
//		mesh->CreateBox(1.0f, 1.0f, 1.0f, 8, 8, 8, fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
//		mesh->CreateCapsule((float)r, (float)h, fVec3(1.0f, 1.0f, 1.0f));
		mesh->CreateCylinder((float)r, (float)h, fVec3(0.0f, 1.0f, 1.0f), fVec3(0.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
		const double m = 1.0;
		rigidBody->SetMass(m);
		dMat33 I = dMat33::Identity();
//		I(0, 0) = m*(3.0*r*r + 4.0*h*h) / 12.0;
//		I(1, 1) = m*(3.0*r*r + 4.0*h*h) / 12.0;
//		I(2, 2) = m*r*r / 2.0;
		rigidBody->SetInertia(I);

		dVec3 alpha(
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX
		);

		dVec3 pos = alpha.Times(sceneMin) + (dVec3(1.0, 1.0, 1.0) - alpha).Times(sceneMax);

		rigidBody->SetPosition(pos);
		rigidBody->SetRotation(dQuat(dVec3(0.0, 1.0, 0.0), dPI*0.0));

		GameObject* gameObject = new GameObject;
		gameObject->SetPhysicsObject(rigidBody);
		gameObject->SetRenderObject(renderObj);

		game->AddGameObject(gameObject);
	}

	RigidBody* rigidBody = new RigidBody;
	Box* ground = new Box();
	const double k = 32.0;
	ground->SetDimensions(k, k, 1.0);
	rigidBody->SetGeometry(ground, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
	rigidBody->SetPosition(dVec3(0.0, 0.0, -16.0));
	rigidBody->SetMass(0.0);
	rigidBody->SetInertia(dMat33::Identity().Scale(0.0));
	RenderMesh* mesh = new RenderMesh;
	mesh->CreateBox(float(k), (float)k, 1.0f, 64, 64, 64, fVec3(1.0f, 1.0f, 1.0f), fVec3(0.0f, 0.0f, 0.0f));
	RenderObject* renderObj = new RenderObject;
	renderObj->SetRenderMesh(mesh);
	renderObj->SetShader(shader);
	GameObject* gameObject = new GameObject;
	gameObject->SetPhysicsObject(rigidBody);
	gameObject->SetRenderObject(renderObj);

	game->AddGameObject(gameObject);
}

void Tests::CreateGJKTest(Game* game)
{
	Shader_Default* shader = new Shader_Default;
	Cylinder* geometry = new Cylinder;
	geometry->SetRadius(1.0);
	geometry->SetHalfHeight(2.0);
//	Box* geometry = new Box;
//	geometry->SetDimensions(1.0, 1.0, 1.0);

//	dVec3 pos[2] = { dVec3(-0.9,1.0,0.0),dVec3(0.9,1.0,0.2) };
//	dVec3 pos[2] = { dVec3(0.0,2.0,-1.9),dVec3(0.5,2.5,1.9) };
	dVec3 pos[2] = { dVec3(0.0,0.0,-1.9),dVec3(0.0,0.0,1.9) };

//	dQuat rot[2] = { dQuat::Identity(), dQuat(dVec3(1.0,0.0,0.0), dPI*0.0) };
	dQuat rot[2] = { dQuat::Identity(), dQuat::Identity() };
	
	for (int i = 0; i < 2; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
		mesh->CreateCylinder(1.0f, 2.0f, fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
//		mesh->CreateBox(1.0f, 1.0f, 1.0f, 0, 0, 0, fVec3(1.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry, dVec3(0.0,0.0,0.0), dQuat::Identity());
		rigidBody->SetMass(1.0);
		rigidBody->SetInertia(dMat33::Identity().Scale(10.0));

		rigidBody->SetPosition(pos[i]);
		rigidBody->SetRotation(rot[i]);

		game->rb[i] = rigidBody;

		GameObject* gameObject = new GameObject;
		gameObject->SetPhysicsObject(rigidBody);
//		gameObject->SetRenderObject(renderObj);

		game->AddGameObject(gameObject);
	}
}
