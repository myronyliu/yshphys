#include "stdafx.h"
#include "Box.h"
#include "Ray.h"


Box::Box()
{
}


Box::~Box()
{
}

void Box::GetDimensions(double& halfDimX, double& halfDimY, double& halfDimZ) const
{
	halfDimX = m_halfDim.x;
	halfDimY = m_halfDim.y;
	halfDimZ = m_halfDim.z;
}

void Box::SetDimensions(double halfDimX, double halfDimY, double halfDimZ)
{
	m_halfDim.x = abs(halfDimX);
	m_halfDim.y = abs(halfDimY);
	m_halfDim.z = abs(halfDimZ);

	m_localOOBB.min = -m_halfDim;
	m_localOOBB.max = m_halfDim;
}

dVec3 Box::SupportLocal(const dVec3& v) const
{
	return dVec3(
		m_halfDim.x * MathUtils::sgn(v.x),
		m_halfDim.y * MathUtils::sgn(v.y),
		m_halfDim.z * MathUtils::sgn(v.z));
}

bool Box::RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hit) const
{
	Ray r;
	r.SetOrigin((-rot).Transform((ray.GetOrigin() - pos)));
	r.SetDirection((-rot).Transform(ray.GetDirection()));
	AABB aabb;
	aabb.min = -m_halfDim;
	aabb.max = m_halfDim;

	double tMin, tMax;
	if (r.IntersectAABB(aabb, tMin, tMax))
	{
		hit = ray.GetOrigin() + ray.GetDirection().Scale(tMin);
		return true;
	}
	else
	{
		return false;
	}
}

Polygon Box::IntersectPlaneLocal(const dVec3& planeOrigin, const dVec3& z, const dVec3& x, const dVec3& y) const
{
	Polygon poly;

	const double cx = planeOrigin.Dot(x);
	const double cy = planeOrigin.Dot(y);

	for (int i = 0; i < 3; ++i)
	{
		if (abs(z[i]) > 0.999)
		{
			const int j = (i + 1) % 3;
			const int k = (i + 2) % 3;

			const double sgn = MathUtils::sgn(z[i]);

			dVec3 o[4];
			dVec2 v2[4];

			o[0][i] = sgn*m_halfDim[i];
			o[1][i] = sgn*m_halfDim[i];
			o[2][i] = sgn*m_halfDim[i];
			o[3][i] = sgn*m_halfDim[i];

			o[0][j] = -m_halfDim[j];
			o[1][j] = m_halfDim[j];
			o[2][j] = m_halfDim[j];
			o[3][j] = -m_halfDim[j];

			o[0][k] = -m_halfDim[k];
			o[1][k] = -m_halfDim[k];
			o[2][k] = m_halfDim[k];
			o[3][k] = m_halfDim[k];

			dVec3 d(0.0, 0.0, 0.0);
			d[i] = -sgn;

			for (int n = 0; n < 4; ++n)
			{
				// Ray intersection of the box edge against the plane
				const double t = (o[n] - planeOrigin).Dot(z) / d.Dot(z);
				const dVec3 v3 = o[n] + d.Scale(t);
				v2[n].x = v3.Dot(x) - cx;
				v2[n].y = v3.Dot(y) - cy;
			}

			if (sgn > 0.0)
			{
				poly.AddVertex(v2[0]);
				poly.AddVertex(v2[1]);
				poly.AddVertex(v2[2]);
				poly.AddVertex(v2[3]);
			}
			else
			{
				poly.AddVertex(v2[3]);
				poly.AddVertex(v2[2]);
				poly.AddVertex(v2[1]);
				poly.AddVertex(v2[0]);
			}
			return poly;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		if (abs(z[i]) < 0.001)
		{
			const int j = (i + 1) % 3;
			const int k = (i + 2) % 3;

			const double sgnj = MathUtils::sgn(z[j]);
			const double sgnk = MathUtils::sgn(z[k]);

			dVec3 v;

			v[j] = sgnj*m_halfDim[j];
			v[k] = sgnk*m_halfDim[k];

			v[i] = -m_halfDim[i];
			poly.AddVertex(dVec2(v.Dot(x) - cx, v.Dot(y) - cy));
			v[i] = m_halfDim[i];
			poly.AddVertex(dVec2(v.Dot(x) - cx, v.Dot(y) - cy));

			return poly;
		}
	}

	// If we reach this point, the plane intersects at a corner of the box
	poly.AddVertex(dVec2(0.0, 0.0));
	return poly;
}