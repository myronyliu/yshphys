#pragma once
#include "YshMath.h"
#include "Vec3.h"

class Simplex3D
{
public:
	Simplex3D();
	virtual ~Simplex3D();

	void SetVertices(unsigned int nVertices, const dVec3* vertices);
	void AddVertex(const dVec3& vertex);

	int GetNumVertices() const;
	int GetVertices(dVec3* const vertices) const;

	// Returns the closest point on this simplex to the point x.
	// closestFeature is the feature on this simplex that is closest to x
	// (e.g. If this simplex is a tetrahedron and x lies just above a face, closestFeature will be a triangle simplex)
	// featureNormal may not be unique. For instance, if closestFeature is a line, the normal is ambiguous.
	// If closestFeature is a tetrahedral face, featureNormal is unique in the outward facing direction.
	dVec3 ClosestPoint(const dVec3& x, Simplex3D& closestFeature) const;
	
private:

	inline static dVec3 ClosestPoint1(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature);
	inline static dVec3 ClosestPoint2(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature);
	inline static dVec3 ClosestPoint3(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature);
	inline static dVec3 ClosestPoint4(const dVec3* const v, const dVec3& x, Simplex3D& closestFeature);

	int m_nVertices;
	dVec3 m_vertices[4];
};

