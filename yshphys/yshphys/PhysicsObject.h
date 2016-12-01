#pragma once
#include "Geometry.h"

class PhysicsObject
{
public:
	PhysicsObject();
	virtual ~PhysicsObject();
private:
	
	dVec3 m_pos;
	dVec3 m_rot;

	Geometry* m_rootGeom;
};

