#include "stdafx.h"
#include "MathUtils.h"


MathUtils::MathUtils()
{
}


MathUtils::~MathUtils()
{
}

Vec3 MathUtils::ClosestPointOnLineToPoint
(
	const Vec3& a, bool& featureContainsA,
	const Vec3& b, bool& featureContainsB,
	const Vec3& x 
)
{
	featureContainsA = false;
	featureContainsB = false;

	Vec3 ax(x - a);
	Vec3 bx(x - b);
	Vec3 ab(b - a);

	// POINT
	if (ax.Dot(ab) <= 0.0)
	{
		featureContainsA = true;
		return a;
	}
	if (bx.Dot(ab) >= 0.0)
	{
		featureContainsB = true;
		return b;
	}

	// EDGE 
	featureContainsA = true;
	featureContainsB = true;
	return a + ab.Scale((ax).Dot(ab) / ab.Dot(ab));
}
Vec3 MathUtils::ClosestPointOnTriangleToPoint
(
	const Vec3& a, bool& featureContainsA,
	const Vec3& b, bool& featureContainsB,
	const Vec3& c, bool& featureContainsC,
	const Vec3& pt
)
{
	featureContainsA = false;
	featureContainsB = false;
	featureContainsC = false;

	Vec3 ab(b - a);
	Vec3 bc(c - b);
	Vec3 ca(a - c);

	Vec3 perp(ab.Cross(bc));
	Vec3 x((pt - a) + perp.Scale((pt - a).Dot(perp) / perp.Dot(perp))); // the origin projected onto the plane of the triangle

	Vec3 ax(x - a);
	Vec3 bx(x - b);
	Vec3 cx(x - c);

	// POINT
	if (ax.Dot(ab) <= 0.0 && ax.Dot(ca) >= 0.0)
	{
		featureContainsA = true;
		return a;
	}
	if (bx.Dot(bc) <= 0.0 && bx.Dot(ab) >= 0.0)
	{
		featureContainsB = true;
		return b;
	}
	if (cx.Dot(ca) <= 0.0 && cx.Dot(bc) >= 0.0)
	{
		featureContainsC = true;
		return c;
	}

	// EDGE
	if (
		bc.Cross(ab).Cross(ab).Dot(bx) >= 0.0 &&
		ax.Dot(ab) >= 0.0 &&
		bx.Dot(ab) <= 0.0
		)
	{
		return ClosestPointOnLineToPoint(a, featureContainsA, b, featureContainsB, pt);
	}
	if (
		ca.Cross(bc).Cross(bc).Dot(cx) >= 0.0 &&
		bx.Dot(bc) >= 0.0 &&
		cx.Dot(bc) <= 0.0
		)
	{
		return ClosestPointOnLineToPoint(b, featureContainsB, c, featureContainsC, pt);
	}
	if (
		ab.Cross(ca).Cross(ca).Dot(ax) >= 0.0 &&
		cx.Dot(ca) >= 0.0 &&
		ax.Dot(ca) <= 0.0
		)
	{
		return ClosestPointOnLineToPoint(c, featureContainsC, a, featureContainsA, pt);
	}

	// FACE
	featureContainsA = true;
	featureContainsB = true;
	featureContainsC = true;
	return x;
}
Vec3 MathUtils::ClosestPointOnTetrahedronToPoint
(
	const Vec3& a, bool& featureContainsA,
	const Vec3& b, bool& featureContainsB,
	const Vec3& c_, bool& featureContainsC_,
	const Vec3& d_, bool& featureContainsD_,
	const Vec3& x
)
{
	// Reorder the vertices such that the "v[0] tip" points in the same direction as the CCW (i.e. right-handed) rotation of the base v[1]-->v[2]-->v[3]
	// Naturally v[2] will also have the same CCW rotating base v[3]-->v[0]-->v[1]
	// In contrast, v[1] and v[3] will have CW (left-handed) rotating bases

	bool& featureContainsC(featureContainsC_);
	bool& featureContainsD(featureContainsD_);
	Vec3 c(c_);
	Vec3 d(d_);
	if ((a - b).Dot((c_ - b).Cross(d_ - b)) < 0.0);
	{
		featureContainsC = featureContainsD_;
		featureContainsD = featureContainsC_;
		c = d_;
		d = c_;
	}

	featureContainsA = false;
	featureContainsB = false;
	featureContainsC = false;
	featureContainsD = false;

	// Extend the planes of the tetrahedron to infinity and check to see whether x is "blocked" from each of the vertices
	// A positive value for aBlocked means that x is blocked from a by plane bcd.
	// A negative value for aBlocked means that x is on the same side of plane bcd as is a.
	// A zero value for aBlocked means that x coincides with the plane bcd, so it is ambiguous.
	Vec3 ax(x - a);
	Vec3 ab(b - a);
	Vec3 ac(c - a);
	Vec3 ad(d - a);
	double bBlocked = ax.Dot(ac.Cross(ad));
	double cBlocked = ax.Dot(ad.Cross(ab));
	double dBlocked = ax.Dot(ab.Cross(ac));
	Vec3 bx(x - b);
	Vec3 bc(c - b);
	Vec3 bd(d - b);
	double aBlocked = -bx.Dot(bc.Cross(bd));

	// POINT
	if (bBlocked >= 0.0 && cBlocked >= 0.0 && dBlocked >= 0.0)
	{
		featureContainsA = true;
		return a;
	}
	if (cBlocked >= 0.0 && dBlocked >= 0.0 && aBlocked >= 0.0)
	{
		featureContainsB = true;
		return b;
	}
	if (dBlocked >= 0.0 && aBlocked >= 0.0 && cBlocked >= 0.0)
	{
		featureContainsC = true;
		return c;
	}
	if (aBlocked >= 0.0 && bBlocked >= 0.0 && cBlocked >= 0.0)
	{
		featureContainsD = true;
		return d;
	}

	//EDGE
	if (aBlocked <= 0.0 && bBlocked <= 0.0 && cBlocked >= 0.0 && dBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(a, featureContainsA, b, featureContainsB, x);
	}
	if (bBlocked <= 0.0 && cBlocked <= 0.0 && dBlocked >= 0.0 && aBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(b, featureContainsB, c, featureContainsC, x);
	}
	if (cBlocked <= 0.0 && dBlocked <= 0.0 && aBlocked >= 0.0 && bBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(c, featureContainsC, d, featureContainsD, x);
	}
	if (dBlocked <= 0.0 && aBlocked <= 0.0 && bBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(d, featureContainsD, a, featureContainsA, x);
	}
	if (aBlocked <= 0.0 && cBlocked <= 0.0 && bBlocked >= 0.0 && dBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(a, featureContainsA, c, featureContainsC, x);
	}
	if (bBlocked <= 0.0 && dBlocked <= 0.0 && cBlocked >= 0.0 && aBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(b, featureContainsB, d, featureContainsD, x);
	}
	if (cBlocked <= 0.0 && aBlocked <= 0.0 && dBlocked >= 0.0 && bBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(c, featureContainsC, a, featureContainsA, x);
	}
	if (dBlocked <= 0.0 && bBlocked <= 0.0 && aBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(d, featureContainsD, b, featureContainsB, x);
	}

	// FACE
	if (aBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(b, featureContainsB, c, featureContainsC, d, featureContainsD, x);
	}
	if (bBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(c, featureContainsC, d, featureContainsD, a, featureContainsA, x);
	}
	if (cBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(d, featureContainsD, a, featureContainsA, b, featureContainsB, x);
	}
	if (dBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(a, featureContainsA, b, featureContainsB, c, featureContainsC, x);
	}

	// INTERIOR
	featureContainsA = true;
	featureContainsB = true;
	featureContainsC = true;
	featureContainsD = true;
	return x;
}
