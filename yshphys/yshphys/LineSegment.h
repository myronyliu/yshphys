#pragma once
#include "Geometry.h"
class LineSegment:
	public Geometry
{
public:
	LineSegment();
	virtual ~LineSegment();

	dVec3 Support(const dVec3& v) const;
protected:

	double m_halfLength;
};

