#pragma once
#include "Vec3.h"
#include "Quat.h"
#include "Simplex3D.h"
#include "BoundingBox.h"

class Geometry
{
public:
	Geometry();
	virtual ~Geometry();

	dVec3 GetPosition() const;
	dQuat GetRotation() const;

	BoundingBox GetLocalOOBB() const;

	void SetPosition(const dVec3& pos);
	void SetRotation(const dQuat& rot);

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual dVec3 SupportLocal(const dVec3& v) const;

	// ptSelf and ptGeoms are the points on the respective Geometries
	// that consititue the smallest separation between the Geometries.
	// separation is the distance between ptSelf and ptGeom (negative if penetrating)
	static double ComputeSeparation(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1);

protected:

	double ComputePenetration(const Geometry* geom, dVec3& ptSelf, dVec3& ptGeom, Simplex3D tetrahedron) const;

	dVec3 m_pos; // position relative to linked RigidBody
	dQuat m_rot; // rotation relative to linked RigidBody 

	BoundingBox m_localOOBB; // assuming m_pos and m_rot are both zero
};

