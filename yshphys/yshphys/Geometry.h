#pragma once
#include "Vec3.h"
#include "Quat.h"
#include "Simplex3D.h"
#include "BoundingBox.h"
#include "Polygon.h"
#include "Material.h"

// Well, this is kinda ugly. We are going to store the material with the geometry (as opposed to with the rigidbody)
// This way, a trimesh geometry can be textured with many different physics materials, and we only need one rigidbody.

enum EGeomType
{
	SPHERE = 0,
	BOX,
	GENERIC
};

class Ray;

class Geometry
{
public:
	Geometry();
	virtual ~Geometry();

	virtual void UpdateLocalOOBB();
	BoundingBox GetLocalOOBB() const;

	virtual bool RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hitPt) const;

	virtual dVec3 Support(const dVec3& pos, const dQuat& rot, const dVec3& v) const;
	virtual dVec3 SupportLocal(const dVec3& v) const;

	virtual Material::Type GetMaterialLocal(const dVec3& x) const;

	void SetUniformMaterial(Material::Type material);

	// ptSelf and ptGeoms are the points on the respective Geometries
	// that consititue the smallest separation between the Geometries.
	// separation is the distance between ptSelf and ptGeom (negative if penetrating)
	// contactNormal is the normal to geom0 pointing outward.
	static bool Intersect(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0, dVec3& n0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1, dVec3& n1);

	static bool Intersect(
		const Geometry* geom0, const dVec3& pos0, const dQuat& rot0, dVec3& pt0, dVec3& n0,
		const Geometry* geom1, const dVec3& pos1, const dQuat& rot1, dVec3& pt1, dVec3& n1,
		GJKSimplex& simplex, bool bypassPenetration = false);

	virtual Polygon IntersectPlane(const dVec3& pos, const dQuat& rot, const dVec3& planeOrigin, const dVec3& planeNormal, const dVec3& xAxis, const dVec3& yAxis) const;
	virtual Polygon IntersectPlaneLocal(const dVec3& planeOrigin, const dVec3& planeNormal, const dVec3& xAxis, const dVec3& yAxis) const;

	virtual EGeomType GetType() const { return EGeomType::GENERIC; }

protected:

	BoundingBox m_localOOBB; // assuming m_pos and m_rot are both zero

	Material::Type m_material;

	static dVec3 QuantizeDirection(const dVec3& v);
};

