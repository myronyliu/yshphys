#pragma once
#include "Geometry.h"
class LineSegment:
	public Geometry
{
public:
	LineSegment();
	virtual ~LineSegment();

	virtual dVec3 SupportLocal(const dVec3& v) const;
protected:

	double m_halfLength;
};

