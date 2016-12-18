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

SupportPolygon Box::SupportLocal(const dVec3& v) const
{
	SupportPolygon poly;

	for (int i = 0; i < 3; ++i)
	{
		const int j = (i + 1) % 3;
		const int k = (i + 2) % 3;

		const double vi_sqr = v[i] * v[i];
		const double vj_sqr = v[j] * v[j];
		const double vk_sqr = v[k] * v[k];

		if (vi_sqr / (vj_sqr + vk_sqr) < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
		{
			if (vj_sqr / vk_sqr < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
			{
				const double supportK = m_halfDim[k] * MathUtils::sgn(v[k]);

				poly.nVertices = 4;

				poly.vertices[0][i] = -m_halfDim[i];
				poly.vertices[0][j] = -m_halfDim[j];
				poly.vertices[0][k] = supportK;

				poly.vertices[1][i] = m_halfDim[i];
				poly.vertices[1][j] = -m_halfDim[j];
				poly.vertices[1][k] = supportK;

				poly.vertices[2][i] = m_halfDim[i];
				poly.vertices[2][j] = m_halfDim[j];
				poly.vertices[2][k] = supportK;

				poly.vertices[3][i] = m_halfDim[i];
				poly.vertices[3][j] = -m_halfDim[j];
				poly.vertices[3][k] = supportK;
			}
			else if (vk_sqr / vj_sqr < ZERO_ANGLE_THRESH*ZERO_ANGLE_THRESH)
			{
				const double supportJ = m_halfDim[j] * MathUtils::sgn(v[j]);

				poly.nVertices = 4;

				poly.vertices[0][i] = -m_halfDim[i];
				poly.vertices[0][j] = supportJ;
				poly.vertices[0][k] = -m_halfDim[k];

				poly.vertices[1][i] = -m_halfDim[i];
				poly.vertices[1][j] = supportJ;
				poly.vertices[1][k] = m_halfDim[k];

				poly.vertices[2][i] = m_halfDim[i];
				poly.vertices[2][j] = supportJ;
				poly.vertices[2][k] = m_halfDim[k];

				poly.vertices[3][i] = m_halfDim[i];
				poly.vertices[3][j] = supportJ;
				poly.vertices[3][k] = -m_halfDim[k];
			}
			else
			{
				const double vj_abs = abs(v[j]);
				const double vk_abs =  abs(v[k]);

				const double hj_abs = abs(m_halfDim[j]);
				const double hk_abs = abs(m_halfDim[k]);

				const double cos = (vj_abs*hj_abs + vk_abs*hk_abs) / ((vj_sqr + vk_sqr)*(hj_abs*hj_abs + hk_abs*hk_abs));
				const double ang = acos(cos);

				if (ang < ZERO_ANGLE_THRESH)
				{
					poly.nVertices = 2;

					poly.vertices[0][i] = -m_halfDim[i];
					poly.vertices[0][j] = m_halfDim[j] * MathUtils::sgn(v[j]);
					poly.vertices[0][k] = m_halfDim[k] * MathUtils::sgn(v[k]);

					poly.vertices[1][i] = m_halfDim[i];
					poly.vertices[1][j] = m_halfDim[j] * MathUtils::sgn(v[j]);
					poly.vertices[1][k] = m_halfDim[k] * MathUtils::sgn(v[k]);
				}
			}
		}
	}

	poly.nVertices = 1;
	poly.vertices[0] = dVec3(
		m_halfDim.x * MathUtils::sgn(v.x),
		m_halfDim.y * MathUtils::sgn(v.y),
		m_halfDim.z * MathUtils::sgn(v.z));
	return poly;
}

bool Box::RayIntersect(const dVec3& pos, const dQuat& rot, const Ray& ray, dVec3& hit) const
{
	Ray r;
	r.SetOrigin((-rot).Transform((ray.GetOrigin() - pos)));
	r.SetDirection((-rot).Transform(ray.GetDirection()));
	AABB aabb;
	aabb.min = m_halfDim;
	aabb.max = m_halfDim;

	double tMin, tMax;
	if (r.IntersectAABB(aabb, tMin, tMax))
	{
		hit = pos + ray.GetOrigin() + ray.GetDirection().Scale(tMin);
		return true;
	}
	else
	{
		return false;
	}
}