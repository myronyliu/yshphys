#pragma once
#include "Vec3.h"
#include "Quat.h"

class Geometry
{
public:
	Geometry();
	virtual ~Geometry();

	Vec3 GetPosition() const;
	Quat GetRotation() const;

	virtual Vec3 Support(const Vec3& v) const;

	// ptSelf and ptGeoms are the points on the respective Geometries
	// that consititue the smallest separation between the Geometries.
	// Return value is the distance between ptSelf and ptGeom.
	double ComputeSeparation(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom) const;

protected:
	double ComputePenetration(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom, Vec3* tetrahedron) const;

	Vec3 m_pos; // position
	Quat m_rot; // rotation
};

