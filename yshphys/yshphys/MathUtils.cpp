#include "stdafx.h"
#include "MathUtils.h"


MathUtils::MathUtils()
{
}


MathUtils::~MathUtils()
{
}

Vec3 MathUtils::ClosestPointOnLineToPoint(const Vec3& a, const Vec3& b, const Vec3& pt)
{
	Vec3 ab(b - a);
	return a + ab.Scale((pt - a).Dot(ab) / ab.Dot(ab));
}
Vec3 MathUtils::ClosestPointOnTriangleToPoint(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& pt)
{
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
		return a;
	}
	if (bx.Dot(bc) <= 0.0 && bx.Dot(ab) >= 0.0)
	{
		return b;
	}
	if (cx.Dot(ca) <= 0.0 && cx.Dot(bc) >= 0.0)
	{
		return c;
	}

	// EDGE
	if (
		bc.Cross(ab).Cross(ab).Dot(bx) >= 0.0 &&
		ax.Dot(ab) >= 0.0 &&
		bx.Dot(ab) <= 0.0
		)
	{
		return a + ab.Scale(ax.Dot(ab) / ab.Dot(ab));
	}
	if (
		ca.Cross(bc).Cross(bc).Dot(cx) >= 0.0 &&
		bx.Dot(bc) >= 0.0 &&
		cx.Dot(bc) <= 0.0
		)
	{
		return b + bc.Scale(bx.Dot(bc) / bc.Dot(bc));
	}
	if (
		ab.Cross(ca).Cross(ca).Dot(ax) >= 0.0 &&
		cx.Dot(ca) >= 0.0 &&
		ax.Dot(ca) <= 0.0
		)
	{
		return c + ca.Scale(cx.Dot(ca) / ca.Dot(ca));
	}

	// FACE 
	return x;
}
Vec3 MathUtils::ClosestPointOnTetrahedronToPoint(const Vec3& a, const Vec3& b, const Vec3& c_, const Vec3& d_, const Vec3& x)
{
	// Reorder the vertices such that the "v[0] tip" points in the same direction as the CCW (i.e. right-handed) rotation of the base v[1]-->v[2]-->v[3]
	// Naturally v[2] will also have the same CCW rotating base v[3]-->v[0]-->v[1]
	// In contrast, v[1] and v[3] will have CW (left-handed) rotating bases

	Vec3 c(c_);
	Vec3 d(d_);
	if ((a - b).Dot((c_ - b).Cross(d_ - b)) < 0.0);
	{
		c = d_;
		d = c_;
	}

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
		return a;
	}
	if (cBlocked >= 0.0 && dBlocked >= 0.0 && aBlocked >= 0.0)
	{
		return b;
	}
	if (dBlocked >= 0.0 && aBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return c;
	}
	if (aBlocked >= 0.0 && bBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return d;
	}

	//EDGE
	if (aBlocked <= 0.0 && bBlocked <= 0.0 && cBlocked >= 0.0 && dBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(a, b, x);
	}
	if (bBlocked <= 0.0 && cBlocked <= 0.0 && dBlocked >= 0.0 && aBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(b, c, x);
	}
	if (cBlocked <= 0.0 && dBlocked <= 0.0 && aBlocked >= 0.0 && bBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(c, d, x);
	}
	if (dBlocked <= 0.0 && aBlocked <= 0.0 && bBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(d, a, x);
	}
	if (aBlocked <= 0.0 && cBlocked <= 0.0 && bBlocked >= 0.0 && dBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(a, c, x);
	}
	if (bBlocked <= 0.0 && dBlocked <= 0.0 && cBlocked >= 0.0 && aBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(b, d, x);
	}
	if (cBlocked <= 0.0 && aBlocked <= 0.0 && dBlocked >= 0.0 && bBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(c, a, x);
	}
	if (dBlocked <= 0.0 && bBlocked <= 0.0 && aBlocked >= 0.0 && cBlocked >= 0.0)
	{
		return ClosestPointOnLineToPoint(d, b, x);
	}

	// FACE
	if (aBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(b, c, d, x);
	}
	if (bBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(c, d, a, x);
	}
	if (cBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(d, a, b, x);
	}
	if (dBlocked >= 0.0)
	{
		return ClosestPointOnTriangleToPoint(a, b, c, x);
	}

	// INTERIOR
	return x;
}
