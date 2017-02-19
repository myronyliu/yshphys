#pragma once
#include "Game.h"

namespace Tests
{
	void CreateCylinder(Game* game, double radius, double halfHeight, double mass, const dVec3& position, const dQuat& rotation, const fVec3& diffuse, const fVec3& specular);
	void CreateCone(Game* game, double radius, double height, double mass, const dVec3& position, const dQuat& rotation, const fVec3& diffuse, const fVec3& specular);
	void CreateSphere(Game* game, double radius, double mass, const dVec3& position, const dQuat& rotation, const fVec3& diffuse, const fVec3& specular);
	void CreateBox(Game* game, const fVec3& halfDim, double mass, const dVec3& position, const dQuat& rotation, const fVec3& diffuse, const fVec3& specular);

	void CreateBVTest(Game* game);
	void CreateGJKTest(Game* game);
	void CreateStackTest(Game* game);
};