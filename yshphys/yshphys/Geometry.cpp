#include "stdafx.h"
#include "Geometry.h"
#include "MathUtils.h"

#define MIN_SUPPORT_SQR 0.0001
#define GJK_TERMINATION_RATIO 0.001

Geometry::Geometry()
{
}


Geometry::~Geometry()
{
}

Vec3 Geometry::GetPosition() const
{
	return m_pos;
}
Quat Geometry::GetRotation() const
{
	return m_rot;
}

Vec3 Geometry::Support(const Vec3& v) const
{
	return Vec3(0.0, 0.0, 0.0);
}

double Geometry::ComputePenetration(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom, Vec3* tetrahedron) const
{
	return -1.0;
}

Vec3 GetClosestPointOnSimplexToSOrigin(int nSimplexPts, const Vec3** vClosestPointArgs, bool** bClosestPointArgs, int& nClosestFeaturePts)
{
	Vec3 v;
	if (nSimplexPts == 1)
	{
		v = *vClosestPointArgs[0];
		*bClosestPointArgs[0] = false;
	}
	else if (nSimplexPts == 2)
	{
		v = MathUtils::ClosestPointOnLineToPoint
		(
			*vClosestPointArgs[0], *bClosestPointArgs[0],
			*vClosestPointArgs[1], *bClosestPointArgs[1],
			Vec3(0.0, 0.0, 0.0)
		);
		nClosestFeaturePts = (int)bClosestPointArgs[0] + (int)bClosestPointArgs[1];
	}
	else if (nSimplexPts == 3)
	{
		v = MathUtils::ClosestPointOnTriangleToPoint
		(
			*vClosestPointArgs[0], *bClosestPointArgs[0],
			*vClosestPointArgs[1], *bClosestPointArgs[1],
			*vClosestPointArgs[2], *bClosestPointArgs[2],
			Vec3(0.0, 0.0, 0.0)
		);
		nClosestFeaturePts = (int)bClosestPointArgs[0] + (int)bClosestPointArgs[1] + (int)bClosestPointArgs[2];
	}
	else
	{
		v = MathUtils::ClosestPointOnTetrahedronToPoint
		(
			*vClosestPointArgs[0], *bClosestPointArgs[0],
			*vClosestPointArgs[1], *bClosestPointArgs[1],
			*vClosestPointArgs[2], *bClosestPointArgs[2],
			*vClosestPointArgs[3], *bClosestPointArgs[3],
			Vec3(0.0, 0.0, 0.0)
		);
		nClosestFeaturePts = (int)bClosestPointArgs[0] + (int)bClosestPointArgs[1] + (int)bClosestPointArgs[2] + (int)bClosestPointArgs[3];
	}
	return v;
}

double Geometry::ComputeSeparation(const Geometry* geom, Vec3& ptSelf, Vec3& ptGeom) const
{
	bool bSimplex[4] = { false, false, false, false };
	Vec3 vSimplex[4];

	// First pass is special. Don't check for termination since v is just a guess.
	Vec3 v(m_pos - geom->m_pos);
	ptSelf = Support(-v);
	ptGeom = geom->Support(v);
	vSimplex[0] = ptSelf - ptGeom;
	bSimplex[0] = true;

	int nIter = 0;
	bool originGrazed = false;

	while (nIter < 16)
	{
		int nSimplex = 0;
		const Vec3* vClosestPointArgs[4];
		bool* bClosestPointArgs[4];
		int iNextSimplexPt = 5; // The new simplex point will boot out this element in the original array
		int nFeaturePoints = 0;

		for (int i = 0; i < 4; ++i)
		{
			if (bSimplex[i])
			{
				vClosestPointArgs[nSimplex] = &vSimplex[i];
				bClosestPointArgs[nSimplex] = &bSimplex[i];
				nSimplex++;
			}
			else
			{
				iNextSimplexPt = i;
			}
		}
		// Get the closest point on the convex hull of the simplex, set it to the new support direction "v"
		// and discard any existing points on the simplex that are not needed to express "v"
		GetClosestPointOnSimplexToSOrigin(nSimplex, vClosestPointArgs, bClosestPointArgs, nFeaturePoints);
		if (nSimplex == 4)
		{
			// ORIGIN ENCLOSED
			if (nFeaturePoints == 4)
			{
				return ComputePenetration(geom, ptSelf, ptGeom, vSimplex);
			}

			for (int i = 0; i < 4; ++i)
			{
				if (!bSimplex[i])
				{
					iNextSimplexPt = i;
					break;
				}
			}
		}

		double vSqr = v.Dot(v);
		if (vSqr < MIN_SUPPORT_SQR)
		{
			if (nFeaturePoints == 2)
			{
				if (nSimplex == 2)
				{
					// Just make an orthogonal vector in the xy plane for now
					v = (*vClosestPointArgs[1] - *vClosestPointArgs[0]);
					v.z = -v.x;
					v.x = v.y;
					v.y = v.z;
					v.z = 0.0;
				}
				else if (nSimplex == 3)
				{
					int iFarSimplexPt = -1;
					for (int i = 0; i < 3; ++i)
					{
						if (!bClosestPointArgs[i])
						{
							iFarSimplexPt = i;
							break;
						}
					}
					Vec3 e01(*vClosestPointArgs[1] - *vClosestPointArgs[0]);
					Vec3 e12(*vClosestPointArgs[2] - *vClosestPointArgs[1]);

					v = e01 - e12.Scale(e01.Dot(e12) / e12.Dot(e12));
				}
				else
				{

				}
			}
			else if (nFeaturePoints == 3)
			{
				if (nSimplex == 3)
				{
					Vec3 e01(*vClosestPointArgs[1] - *vClosestPointArgs[0]);
					Vec3 e02(*vClosestPointArgs[2] - *vClosestPointArgs[0]);

					v = e01.Cross(e02);
				}
				else // set the support vector to point away from the tetrahedron
				{
					int iFarSimplexPt = -1;
					for (int i = 0; i < 4; ++i)
					{
						if (!bClosestPointArgs[i])
						{
							iFarSimplexPt = i;
							break;
						}
					}
					Vec3 e12(*vClosestPointArgs[(iFarSimplexPt + 2) % 4] - *vClosestPointArgs[(iFarSimplexPt + 1) % 4]);
					Vec3 e13(*vClosestPointArgs[(iFarSimplexPt + 3) % 4] - *vClosestPointArgs[(iFarSimplexPt + 1) % 4]);

					Vec3 e01(*vClosestPointArgs[(iFarSimplexPt + 1) % 4] - *vClosestPointArgs[(iFarSimplexPt + 0) % 4]);

					Vec3 faceNormal(e12.Cross(e13)); // This might be pointing to the interior. We don't want that
					if (faceNormal.Dot(e01) > 0)
					{
						faceNormal = -faceNormal;
					}

					v = faceNormal;
				}
			}
			// No need to check nFeaturePoints == 4 since we would have returned a negative distance already if that were the case

			v.Scale(sqrt(v.Dot(v))); // normalize it just to be safe against prematurely exiting on the termination ratio check
		}
		ptSelf = Support(-v);
		ptGeom = geom->Support(v);
		vSimplex[iNextSimplexPt] = ptSelf - ptGeom;
		bSimplex[iNextSimplexPt] = true;
		double d(vSimplex[iNextSimplexPt].Dot(vSimplex[iNextSimplexPt]));

		if (d < MIN_SUPPORT_SQR || fabs(vSimplex[iNextSimplexPt].Dot(v) - vSqr) / vSqr < GJK_TERMINATION_RATIO)
		{
			return sqrt(d);
		}
	}
}