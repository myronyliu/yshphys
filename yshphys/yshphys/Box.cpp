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

Polygon Box::IntersectPlaneLocal(const dVec3& planeOrigin, const dQuat& planeOrientation) const
{
	Polygon poly;

	const dMat33 R(planeOrientation);
	const dVec3 x(R.GetColumn(0));
	const dVec3 y(R.GetColumn(1));
	const dVec3 z(R.GetColumn(2));

	const double cx = planeOrigin.Dot(R.GetColumn(0));
	const double cy = planeOrigin.Dot(R.GetColumn(1));

	for (int i = 0; i < 3; ++i)
	{
		if (abs(z[i]) > 0.9999)
		{
			const int j = (i + 1) % 3;
			const int k = (i + 2) % 3;

			const double sgn = MathUtils::sgn(z[i]);

			dVec3 v3[4];
			dVec2 v2[4];

			v3[0][i] = sgn*m_halfDim[i];
			v3[1][i] = sgn*m_halfDim[i];
			v3[2][i] = sgn*m_halfDim[i];
			v3[3][i] = sgn*m_halfDim[i];

			v3[0][j] = -m_halfDim[j];
			v3[1][j] = m_halfDim[j];
			v3[2][j] = m_halfDim[j];
			v3[3][j] = -m_halfDim[j];

			v3[0][k] = -m_halfDim[k];
			v3[1][k] = -m_halfDim[k];
			v3[2][k] = m_halfDim[k];
			v3[3][k] = m_halfDim[k];

			for (int n = 0; n < 4; ++n)
			{
				v2[n].x = v3[n].Dot(x) - cx;
				v2[n].y = v3[n].Dot(y) - cy;
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