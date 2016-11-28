#include "stdafx.h"
#include "Simplex3D.h"


Simplex3D::Simplex3D() : m_nVertices(0)
{
}


Simplex3D::~Simplex3D()
{
}

void Simplex3D::SetVertices(unsigned int nVertices, const Vec3* vertices)
{
	assert(nVertices <= 4);
	m_nVertices = nVertices;
	for (unsigned int i = 0; i < nVertices; ++i)
	{
		m_vertices[i] = vertices[i];
	}
}

void Simplex3D::AddVertex(const Vec3& vertex)
{
	assert(m_nVertices < 4);
	m_vertices[m_nVertices] = vertex;
	m_nVertices++;
}

int Simplex3D::GetNumVertices() const
{
	return m_nVertices;
}

Vec3 Simplex3D::ClosestPoint(const Vec3& x, Simplex3D& closestFeature) const
{
	if (m_nVertices == 1)
	{
		closestFeature.m_nVertices = 0;
		return m_vertices[0];
	}

	const Vec3& a = m_vertices[0];
	const Vec3& b = m_vertices[1];
	const Vec3& c = m_vertices[2];

	if (m_nVertices == 2)
	{
		Vec3 ax(x - a);
		Vec3 bx(x - b);
		Vec3 ab(b - a);

		// POINT
		closestFeature.m_nVertices = 1;
		if (ax.Dot(ab) <= 0.0)
		{
			closestFeature.m_vertices[0] = a;
			return a;
		}
		if (bx.Dot(ab) >= 0.0)
		{
			closestFeature.m_vertices[0] = b;
			return b;
		}

		// EDGE 
		closestFeature.m_nVertices = 2;
		closestFeature.m_vertices[0] = a;
		closestFeature.m_vertices[1] = b;
		return a + ab.Scale((ax).Dot(ab) / ab.Dot(ab));
	}
	else if (m_nVertices == 3)
	{
		const Vec3 ab(b - a);
		const Vec3 bc(c - b);
		const Vec3 ca(a - c);

		const Vec3 ax(x - a);
		const Vec3 bx(x - b);
		const Vec3 cx(x - c);

		// POINT
		closestFeature.m_nVertices = 1;
		if (ax.Dot(ab) <= 0.0 && ax.Dot(ca) >= 0.0)
		{
			closestFeature.m_vertices[0] = a;
			return a;
		}
		if (bx.Dot(bc) <= 0.0 && bx.Dot(ab) >= 0.0)
		{
			closestFeature.m_vertices[0] = b;
			return b;
		}
		if (cx.Dot(ca) <= 0.0 && cx.Dot(bc) >= 0.0)
		{
			closestFeature.m_vertices[0] = c;
			return c;
		}

		// EDGE
		closestFeature.m_nVertices = 2;
		if (
			bc.Cross(ab).Cross(ab).Dot(ax) >= 0.0 &&
			ax.Dot(ab) >= 0.0 &&
			bx.Dot(ab) <= 0.0
			)
		{
			closestFeature.m_vertices[0] = a;
			closestFeature.m_vertices[1] = b;
			return a + ab.Scale(ax.Dot(ab) / ab.Dot(ab));
		}
		if (
			ca.Cross(bc).Cross(bc).Dot(bx) >= 0.0 &&
			bx.Dot(bc) >= 0.0 &&
			cx.Dot(bc) <= 0.0
			)
		{
			closestFeature.m_vertices[0] = b;
			closestFeature.m_vertices[1] = c;
			return b + bc.Scale(bx.Dot(bc) / bc.Dot(bc));
		}
		if (
			ab.Cross(ca).Cross(ca).Dot(cx) >= 0.0 &&
			cx.Dot(ca) >= 0.0 &&
			ax.Dot(ca) <= 0.0
			)
		{
			closestFeature.m_vertices[0] = c;
			closestFeature.m_vertices[1] = a;
			return c + ca.Scale(cx.Dot(ca) / ca.Dot(ca));
		}

		// FACE
		closestFeature.m_nVertices = 3;
		closestFeature.m_vertices[0] = a;
		closestFeature.m_vertices[0] = b;
		closestFeature.m_vertices[0] = c;
		Vec3 n(ab.Cross(bc));
		return a + ax - n.Scale(ax.Dot(n) / n.Dot(n)); // the origin projected onto the plane of the triangle
	}
	else if (m_nVertices == 4)
	{
		// POINT
		for (int i = 0; i < 4; ++i)
		{
			const Vec3& v0 = m_vertices[i];
			const Vec3 v0_x(x - v0);
			const Vec3 v0_v1(m_vertices[(i + 1) % 4] - v0);
			const Vec3 v0_v2(m_vertices[(i + 2) % 4] - v0);
			const Vec3 v0_v3(m_vertices[(i + 3) % 4] - v0);

			if (
				v0_x.Dot(v0_v1) <= 0.0 &&
				v0_x.Dot(v0_v2) <= 0.0 &&
				v0_x.Dot(v0_v3) <= 0.0
				)
			{
				closestFeature.m_nVertices = 1;
				closestFeature.m_vertices[0] = v0;
				return v0;
			}
		}

		// EDGE
		for (int i = 0; i < 3; ++i)
		{
			const Vec3& v0 = m_vertices[i];
			const Vec3 v0_x(x - v0);

			for (int di = 1; di < 4 - i; ++di)
			{
				const Vec3& v1 = m_vertices[i + di];
				const Vec3& v2 = m_vertices[(i + di + 1) % 4];
				const Vec3& v3 = m_vertices[(i + di + 2) % 4];
				const Vec3 v0_v1(v1 - v0);
				const Vec3 v1_x(x - v1);
				const Vec3 n_2((v2 - v1).Cross(v0_v1).Cross(v0_v1)); // normal of the edge v0_v1 on triangle (v0,v1,v2)
				const Vec3 n_3((v3 - v1).Cross(v0_v1).Cross(v0_v1)); // normal of the edge v0_v1 on triangle (v0,v1,v3)

				if (n_2.Dot(v0_x) >= 0.0 &&
					n_3.Dot(v0_x) >= 0.0 &&

					v0_x.Dot(v0_v1) >= 0.0 &&
					v1_x.Dot(v0_v1) <= 0.0)
				{
					closestFeature.m_nVertices = 2;
					closestFeature.m_vertices[0] = v0;
					closestFeature.m_vertices[1] = v1;
					return v0 + v0_v1.Scale(v0_x.Dot(v0_v1) / v0_v1.Dot(v0_v1));
				}
			}
		}

		// FACE
		for (int i = 0; i < 4; ++i)
		{
			const Vec3& v0 = m_vertices[i];
			const Vec3& v1 = m_vertices[(i + 1) % 4];
			const Vec3& v2 = m_vertices[(i + 2) % 4];
			const Vec3& v3 = m_vertices[(i + 3) % 4];

			Vec3 n((v2 - v1).Cross(v3 - v1));
			n.Scale(n.Dot(v1 - v0)); // make sure the normal is facing outward

			const Vec3 v1_x(x - v1);
			const float d = n.Dot(v1_x);

			if (d >= 0.0)
			{
				closestFeature.m_nVertices = 3;
				closestFeature.m_vertices[0] = v1;
				closestFeature.m_vertices[1] = v2;
				closestFeature.m_vertices[2] = v3;
				return v1 + v1_x - n.Scale(d / n.Dot(n));
			}
		}

		// INTERIOR
		closestFeature.m_nVertices = 4;
		closestFeature.m_vertices[0] = m_vertices[0];
		closestFeature.m_vertices[1] = m_vertices[1];
		closestFeature.m_vertices[2] = m_vertices[2];
		closestFeature.m_vertices[3] = m_vertices[3];
		return x;
	}

	return Vec3(0.0, 0.0, 0.0);
}
