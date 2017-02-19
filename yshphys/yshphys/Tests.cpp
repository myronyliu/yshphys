#include "stdafx.h"
#include "Tests.h"
#include "RigidBody.h"
#include "Capsule.h"
#include "Shader.h"
#include "Shader_Default.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Cone.h"
#include "Box.h"

void Tests::CreateCylinder(Game* game, double r, double h, double m, const dVec3& pos, const dQuat& rot, const fVec3& diffuse, const fVec3& specular)
{
	Cylinder* geom = new Cylinder;
	geom->SetRadius(r);
	geom->SetHalfHeight(h);
	RenderMesh* mesh = new RenderMesh;
	mesh->CreateCylinder((float)r, (float)h, diffuse, specular);
	RenderObject* renderObj = new RenderObject;
	renderObj->SetRenderMesh(mesh);
	renderObj->SetShader(nullptr);

	RigidBody* body = new RigidBody;
	body->SetGeometry(geom, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
	body->SetMass(m);
	dMat33 I = dMat33::Identity();
	I(0, 0) = m*(3.0*r*r + 4.0*h*h) / 12.0;
	I(1, 1) = m*(3.0*r*r + 4.0*h*h) / 12.0;
	I(2, 2) = m*r*r / 2.0;
	body->SetInertia(I);
	body->SetPosition(pos);
	body->SetRotation(rot);

	GameObject* gameObject = new GameObject;
	gameObject->SetPhysicsObject(body);
	gameObject->SetRenderObject(renderObj);
	game->AddGameObject(gameObject);
}
void Tests::CreateCone(Game* game, double r, double h, double m, const dVec3& pos, const dQuat& rot, const fVec3& diffuse, const fVec3& specular)
{
	Cone* geom = new Cone;
	geom->SetRadius(r);
	geom->SetHeight(h);
	RenderMesh* mesh = new RenderMesh;
	mesh->CreateCone((float)r, (float)h, diffuse, specular);
	RenderObject* renderObj = new RenderObject;
	renderObj->SetRenderMesh(mesh);
	renderObj->SetShader(nullptr);

	RigidBody* body = new RigidBody;
	body->SetGeometry(geom, dVec3(0.0, 0.0, -h*0.25), dQuat::Identity());
	body->SetMass(m);
	dMat33 I = dMat33::Identity();
	I(0, 0) = (3.0 / 20.0)*m*(r*r + 4.0*h*h);
	I(1, 1) = (3.0 / 20.0)*m*(r*r + 4.0*h*h);
	I(2, 2) = m*r*r / 10.0;
	body->SetInertia(I);
	body->SetPosition(pos);
	body->SetRotation(rot);

	GameObject* gameObject = new GameObject;
	gameObject->SetPhysicsObject(body);
	gameObject->SetRenderObject(renderObj);
	gameObject->m_renderPosOffset = fVec3(0.0f, 0.0f, -h*0.25f);
	game->AddGameObject(gameObject);
}
void Tests::CreateSphere(Game* game, double r, double m, const dVec3& pos, const dQuat& rot, const fVec3& diffuse, const fVec3& specular)
{
	Sphere* geom = new Sphere;
	geom->SetRadius(r);
	RenderMesh* mesh = new RenderMesh;
	mesh->CreateSphere((float)r, diffuse, specular);
	RenderObject* renderObj = new RenderObject;
	renderObj->SetRenderMesh(mesh);
	renderObj->SetShader(nullptr);

	RigidBody* body = new RigidBody;
	body->SetGeometry(geom, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
	body->SetMass(m);
	dMat33 I = dMat33::Identity().Scale(2.0*m*r*r / 3.0);
	body->SetInertia(I);
	body->SetPosition(pos);
	body->SetRotation(rot);

	GameObject* gameObject = new GameObject;
	gameObject->SetPhysicsObject(body);
	gameObject->SetRenderObject(renderObj);
	game->AddGameObject(gameObject);
}
void Tests::CreateBox(Game* game, const fVec3& halfDim, double m, const dVec3& pos, const dQuat& rot, const fVec3& diffuse, const fVec3& specular)
{
	Box* geom = new Box;
	geom->SetDimensions(halfDim.x, halfDim.y, halfDim.z);
	RenderMesh* mesh = new RenderMesh;
	mesh->CreateBox(halfDim.x, halfDim.y, halfDim.z, 16, 16, 16, diffuse, specular);
	RenderObject* renderObj = new RenderObject;
	renderObj->SetRenderMesh(mesh);
	renderObj->SetShader(nullptr);

	RigidBody* body = new RigidBody;
	body->SetGeometry(geom, dVec3(0.0, 0.0, 0.0), dQuat::Identity());
	body->SetMass(m);
	dMat33 I = dMat33::Identity();
	I(0, 0) = m*(halfDim.y*halfDim.y + halfDim.z*halfDim.z) / 3.0;
	I(1, 1) = m*(halfDim.z*halfDim.z + halfDim.x*halfDim.x) / 3.0;
	I(2, 2) = m*(halfDim.x*halfDim.x + halfDim.y*halfDim.y) / 3.0;
	body->SetInertia(I);
	body->SetPosition(pos);
	body->SetRotation(rot);

	GameObject* gameObject = new GameObject;
	gameObject->SetPhysicsObject(body);
	gameObject->SetRenderObject(renderObj);
	game->AddGameObject(gameObject);
}

void Tests::CreateStackTest(Game* game)
{
	CreateBox(game, fVec3(32.0, 32.0, 1.0), 0.0, dVec3(0.0, 0.0, -16.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));

	CreateBox(game, fVec3(32.0, 1.0, 4.0), 0.0, dVec3(0.0, -33.01, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(32.0, 1.0, 4.0), 0.0, dVec3(0.0, 33.01, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(1.0, 32.0, 4.0), 0.0, dVec3(-33.01, 0.0, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(1.0, 32.0, 4.0), 0.0, dVec3(33.01, 0.0, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));

	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -14.01), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -12.02), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -10.03), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -8.04), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -6.05), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -4.06), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -2.07), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
	CreateSphere(game, 1.0, 1.0, dVec3(0.0, 0.0, -0.08), dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
}

void Tests::CreateBVTest(Game* game)
{
	dVec3 sceneCenter = dVec3(0.0, 0.0, 0.0);
	dVec3 sceneHalfDim = dVec3(1.0, 1.0, 1.0).Scale(8.0);
	dVec3 sceneMin = sceneCenter - sceneHalfDim;
	dVec3 sceneMax = sceneCenter + sceneHalfDim;

	for (int i = 0; i < 1; ++i)
	{
		dVec3 alpha(
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX,
			(double)std::rand() / (double)RAND_MAX
		);

		dVec3 pos = alpha.Times(sceneMin) + (dVec3(1.0, 1.0, 1.0) - alpha).Times(sceneMax);

//		if (i==3)
//		CreateCylinder(game, 1.0, 2.0, 1.0, pos, dQuat::Identity(), fVec3(0.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
		CreateCone(game, 1.0, 2.0, 1.0, pos, dQuat::Identity(), fVec3(0.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
//		CreateSphere(game, 1.0, 1.0, pos, dQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
//		CreateCylinder(game, 1.0, 2.0, 1.0, pos, dQuat::Identity(), fVec3(0.0f, 0.0f, 0.0f), fVec3(1.0f, 0.0f, 0.0f));
//		CreateBox(game, fVec3(1.0, 1.0, 1.0), 1.0, pos, dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	}

	CreateBox(game, fVec3(32.0, 32.0, 1.0), 0.0, dVec3(0.0, 0.0, -16.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));

	CreateBox(game, fVec3(32.0, 1.0, 4.0), 0.0, dVec3(0.0, -33.01, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(32.0, 1.0, 4.0), 0.0, dVec3(0.0, 33.01, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(1.0, 32.0, 4.0), 0.0, dVec3(-33.01, 0.0, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
	CreateBox(game, fVec3(1.0, 32.0, 4.0), 0.0, dVec3(33.01, 0.0, -11.0), dQuat::Identity(), fVec3(1.0f, 1.0f, 1.0f), fVec3(1.0f, 1.0f, 1.0f));
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
