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
	dVec3 sceneHalfDim = dVec3(4.0, 4.0, 4.0);
	dVec3 sceneMin = sceneCenter - sceneHalfDim;
	dVec3 sceneMax = sceneCenter + sceneHalfDim;

	for (int i = 0; i < 5; ++i)
	{
		RenderMesh* mesh = new RenderMesh;
		mesh->CreateCapsule(1.0f, 1.0f, fVec3(1.0f, 1.0f, 1.0f));
		RenderObject* renderObj = new RenderObject;
		renderObj->SetRenderMesh(mesh);
		renderObj->SetShader(shader);

		RigidBody* rigidBody = new RigidBody;
		rigidBody->SetGeometry(geometry);
//		rigidBody->m_F = dVec3(0.0, 0.0, 0.1);
//		rigidBody->m_T = dVec3(0.0, 0.1, 0.0);
		rigidBody->SetMass(1.0);
		rigidBody->SetInertia(dMat33::Identity());
		rigidBody->m_L = dVec3(0.0, 1.0, 0.0);

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