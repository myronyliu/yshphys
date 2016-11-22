#pragma once
#include "Vec3.h"
#include "Quat.h"

class Geometry
{
public:
	Geometry();
	virtual ~Geometry();

	virtual Vec3 Support(const Vec3& v) const;

protected:
	Vec3 m_pos; // position
	Quat m_ori; // orientation
};

