#pragma once
#include "Vec3.h"

class Simplex3D
{
public:
	Simplex3D();
	virtual ~Simplex3D();

	void SetVertices(unsigned int nVertices, const Vec3* vertices);
	void AddVertex(const Vec3& vertex);

	int GetNumVertices() const;

	// Returns the closest point on this simplex to the point x.
	// closestFeature is the feature on this simplex that is closest to x
	// (e.g. If this simplex is a tetrahedron and x lies just above a face, closestFeature will be a triangle simplex)
	// featureNormal may not be unique. For instance, if closestFeature is a line, the normal is ambiguous.
	// If closestFeature is a tetrahedral face, featureNormal is unique in the outward facing direction.
	Vec3 ClosestPoint(const Vec3& x, Simplex3D& closestFeature) const;
	
private:
	int m_nVertices;
	Vec3 m_vertices[4];
};

