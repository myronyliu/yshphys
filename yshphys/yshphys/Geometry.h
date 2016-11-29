#pragma once
#include "Vec3.h"
#include "Quat.h"
#include "Simplex3D.h"

class Geometry
{
public:
	Geometry();
	virtual ~Geometry();

	Vec3 GetPosition() const;
	Quat GetRotation() const;

	void SetPosition(const Vec3& pos);
	void SetRotation(const Quat& rot);

	virtual Vec3 Support(const Vec3& v) const;

	// ptSelf and ptGeoms are the points on the respective Geometries
	// that consititue the smallest separation between the Geometries.
	// separation is the distance between ptSelf and ptGeom (negative if penetrating)
	double ComputeSeparation(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom) const;

protected:
	double ComputePenetration(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom, Simplex3D tetrahedron) const;

	Vec3 m_pos; // position
	Quat m_rot; // rotation
};

