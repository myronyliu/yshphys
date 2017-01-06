#include "stdafx.h"
#include "Tests.h"
#include "RigidBody.h"
#include "Capsule.h"
#include "Shader.h"
#include "Shader_Default.h"

void Tests::CreateBVTest(Game* game)
{
	Shader_Default* shader = new Shader_Default;
	Capsule* geometry = new Capsule;
	geometry->SetRadius(1.0);
	geometry->SetHalfHeight(1.0);

	dVec3 sceneCenter = dVec3(0.0, 32.0, 0.0);
	dVec3 sceneHalfDim = dVec3(1.0, 1.0, 1.0).Scale(8.0);
	dVec3 sceneMin = sceneCenter - sceneHalfDim;
	dVec3 sceneMax = sceneCenter + sceneHalfDim;

	for (int i = 0; i < 16; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
		mesh->CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry);
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
	geometry->SetRadius(1.0);
	geometry->SetHalfHeight(1.0);

	dVec3 pos[2] = { dVec3(0.0,16.0,-1.5),dVec3(0.3,16.0,1.5) };

//	dVec3 pos[2] = { dVec3(0.0,16.0,-1.9),dVec3(0.0,16.0,1.9) };
//	dVec3 pos[2] = { dVec3(-1.2,16.0,0.0),dVec3(1.2,16.0,0.00001) };
//	dQuat rot[2] = { dQuat::Identity(), dQuat(dVec3(1.0,0.0,0.0), dPI / 6.0) };
	dQuat rot[2] = { dQuat::Identity(), dQuat::Identity() };

	pos[1].x = -0.79866617714921184;
	pos[1].y = 16.152170115377011;
	pos[1].z = -0.79866617714921184;
	rot[1].x = 0.34889840206991107;
	rot[1].y = 0.012641419504320241;
	rot[1].z = 0.019273762916590956;
	rot[1].w = 0.93687630227467378;
	
	for (int i = 0; i < 2; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
		mesh->CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry);
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

	GJKSimplex simplex;

	Geometry::ComputeSeparation(
		game->rb[0]->GetGeometry(), game->rb[0]->GetPosition(), game->rb[0]->GetRotation(), dVec3(),
		game->rb[1]->GetGeometry(), game->rb[1]->GetPosition(), game->rb[1]->GetRotation(), dVec3(),
		simplex);

	game->epa = new EPAHull(
		game->rb[0]->GetGeometry(), game->rb[0]->GetPosition(), game->rb[0]->GetRotation(),
		game->rb[1]->GetGeometry(), game->rb[1]->GetPosition(), game->rb[1]->GetRotation(),
		simplex);
}
