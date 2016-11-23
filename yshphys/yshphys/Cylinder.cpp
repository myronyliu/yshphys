#include "stdafx.h"
#include "Cylinder.h"


Cylinder::Cylinder()
{
}


Cylinder::~Cylinder()
{
}

Vec3 Cylinder::Support(const Vec3& v) const
{
	Vec3 vLocal = (-m_rot).Transform(v);
	Vec3 supportLocal;
	double xySqr = vLocal.x*vLocal.x + vLocal.y*vLocal.y;

	if (xySqr > 0.0)
	{
		double xy = sqrt(xySqr);
		supportLocal.x = vLocal.x / (xySqr);
		supportLocal.y = vLocal.y / (xySqr);
	}
	else
	{
		supportLocal.x = 0.0;
		supportLocal.y = 0.0;
	}
	supportLocal.z = m_halfHeight * (double)((0.0 < vLocal.z) - (vLocal.z < 0.0));
	return m_pos + m_rot.Transform(supportLocal);
}