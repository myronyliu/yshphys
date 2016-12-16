#pragma once
#include "Geometry.h"
class LineSegment:
	public Geometry
{
public:
	LineSegment();
	virtual ~LineSegment();

	virtual dVec3 SupportLocal(const dVec3& v, bool & degenerate) const;
protected:

	double m_halfLength;
};

