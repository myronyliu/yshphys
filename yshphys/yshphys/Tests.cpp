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
	Capsule* geometry = new Capsule();
//	Cylinder* geometry = new Cylinder();
	geometry->SetRadius(1.0);
	geometry->SetHalfHeight(1.0);
//	Box* geometry = new Box();
//	geometry->SetDimensions(1.0, 1.0, 1.0);

	dVec3 sceneCenter = dVec3(0.0, 32.0, 0.0);
	dVec3 sceneHalfDim = dVec3(1.0, 1.0, 1.0).Scale(8.0);
	dVec3 sceneMin = sceneCenter - sceneHalfDim;
	dVec3 sceneMax = sceneCenter + sceneHalfDim;

	for (int i = 0; i < 16; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
//		mesh->CreateBox(1.0f, 1.0f, 1.0f, 8, 8, 8, fVec3(1.0f, 1.0f, 1.0f));
		mesh->CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
//		mesh->CreateCylinder(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
		rigidBody->SetMass(1.0);
		rigidBody->SetInertia(dMat33::Identity().Scale(100.0));

		dVec3 alpha(
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX
		);

		dVec3 pos = alpha.Times(sceneMin) + (dVec3(1.0, 1.0, 1.0) - alpha).Times(sceneMax);

		rigidBody->SetPosition(pos);

		GameObject* gameObject = new GameObject;
		gameObject->SetPhysicsObject(rigidBody);
		gameObject->SetRenderObject(renderObj);

		game->AddGameObject(gameObject);
	}
}

void Tests::CreateGJKTest(Game* game)
{
	Shader_Default* shader = new Shader_Default;
	Capsule* geometry = new Capsule;
//	Cylinder* geometry = new Cylinder;
	geometry->SetRadius(1.0);
	geometry->SetHalfHeight(1.0);

	dVec3 pos[2] = { dVec3(-1.1,16.0,0.0),dVec3(1.1,16.0,0.0) };

	dQuat rot[2] = { dQuat::Identity(), dQuat::Identity() };
	
	for (int i = 0; i < 2; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
		mesh->CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
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
		gameObject->SetRenderObject(renderObj);

		game->AddGameObject(gameObject);
	}

//	GJKSimplex simplex;

//	Geometry::ComputeSeparation(
//		game->rb[0]->GetGeometry(), game->rb[0]->GetPosition(), game->rb[0]->GetRotation(), dVec3(),
//		game->rb[1]->GetGeometry(), game->rb[1]->GetPosition(), game->rb[1]->GetRotation(), dVec3(),
//		simplex);

//	game->epa = new EPAHull(
//		game->rb[0]->GetGeometry(), game->rb[0]->GetPosition(), game->rb[0]->GetRotation(),
//		game->rb[1]->GetGeometry(), game->rb[1]->GetPosition(), game->rb[1]->GetRotation(),
//		simplex);
}
