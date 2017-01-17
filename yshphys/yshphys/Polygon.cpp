#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{
	m_boundingSquare.min.x = FLT_MAX;
	m_boundingSquare.min.y = FLT_MAX;

	m_boundingSquare.max.x = -FLT_MAX;
	m_boundingSquare.max.x = -FLT_MAX;
}

Polygon::Polygon(const fVec2* const x, int n) : m_nVertices(n)
{
	m_boundingSquare.min.x = FLT_MAX;
	m_boundingSquare.min.y = FLT_MAX;

	m_boundingSquare.max.x = -FLT_MAX;
	m_boundingSquare.max.x = -FLT_MAX;

	for (int i = 0; i < n; ++i)
	{
		m_boundingSquare.min.x = std::min(m_boundingSquare.min.x, x[i].x);
		m_boundingSquare.min.y = std::min(m_boundingSquare.min.y, x[i].y);

		m_boundingSquare.max.x = std::max(m_boundingSquare.max.x, x[i].x);
		m_boundingSquare.max.y = std::max(m_boundingSquare.max.y, x[i].y);

		m_vertices[i] = x[i];
	}
}

void Polygon::AddVertex(const fVec2& vertex)
{
	assert(m_nVertices < MAX_POLYGON_VERTICES);

	assert(fabs(vertex.x) < 10000.0f);
	assert(fabs(vertex.y) < 10000.0f);

	m_vertices[m_nVertices++] = vertex;

	m_boundingSquare.min.x = std::min(m_boundingSquare.min.x, vertex.x);
	m_boundingSquare.min.y = std::min(m_boundingSquare.min.y, vertex.y);

	m_boundingSquare.max.x = std::max(m_boundingSquare.max.x, vertex.x);
	m_boundingSquare.max.y = std::max(m_boundingSquare.max.y, vertex.y);
}

const fVec2* Polygon::GetVertices(int& n) const
{
	n = m_nVertices;
	return m_vertices;
}

Polygon Polygon::ReflectX() const
{
	Polygon poly;
	poly.m_nVertices = m_nVertices;

	poly.m_boundingSquare.min.y = m_boundingSquare.min.y;
	poly.m_boundingSquare.max.y = m_boundingSquare.max.y;

	poly.m_boundingSquare.min.x = -m_boundingSquare.max.x;
	poly.m_boundingSquare.max.x = -m_boundingSquare.min.x;

	for (int i = 0; i < m_nVertices; ++i)
	{
		poly.m_vertices[m_nVertices - 1 - i].x = -m_vertices[i].x;
		poly.m_vertices[m_nVertices - 1 - i].y = m_vertices[i].y;
	}
	return poly;
}

Polygon Polygon::PruneColinearVertices(double cosThresh) const
{
	return Polygon();
}

void Polygon::BuildEdges(HalfEdge* edges) const
{
	const int nEdges = 2 * m_nVertices;

	for (int i = 0; i < nEdges; i += 2)
	{
		const fVec2& B = m_vertices[(i / 2 + 0) % m_nVertices];
		const fVec2& C = m_vertices[(i / 2 + 1) % m_nVertices];

		HalfEdge* BC = &edges[i];
		HalfEdge* CB = BC + 1;

		HalfEdge* AB = &edges[(i - 2 + nEdges) % nEdges];
		HalfEdge* BA = AB + 1;

		HalfEdge* CD = &edges[(i + 2) % nEdges];
		HalfEdge* DC = CD + 1;

		BC->vert = C;
		CB->vert = B;

		BC->prev = AB;
		BC->next = CD;

		CB->prev = DC;
		CB->next = BA;

		BC->twin = CB;
		CB->twin = BC;

		BC->visited = false;
	}
}

bool PointOnLineSegment(const dVec2& x, const dVec2& A, const dVec2& B)
{
	const dVec2 u(x - A);
	const dVec2 v(B - A);
	const double t = u.Dot(v) / v.Dot(v);
	if ((float)t<0.0f || (float)t>1.0f)
	{
		return false;
	}
	const dVec2 uPerp = u - v.Scale(t);
	return ((float)uPerp.Dot(uPerp) < (float)v.Dot(v)*0.001f);
}

bool IntersectLineSegments(const dVec2& A_, const dVec2& u_, double& t0, const dVec2& B_, const dVec2& v_, double& t1)
{
	dVec2 A(A_);
	dVec2 B(B_);
	dVec2 u(u_);
	dVec2 v(v_);

	double uu = u.Dot(u);
	double vv = v.Dot(v);
	double uv = u.Dot(v);

	if ((float)abs(uu*vv - uv*uv) < FLT_EPSILON)
	{
		A = A + dVec2(u.y, -u.x).Scale((double)FLT_EPSILON);
		u = u + dVec2(-u.y, u.x).Scale((double)FLT_EPSILON);

		B = B + dVec2(-v.y, v.x).Scale((double)FLT_EPSILON);
		v = v + dVec2(v.y, -v.x).Scale((double)FLT_EPSILON);

		uu = u.Dot(u);
		vv = v.Dot(v);
		uv = u.Dot(v);
	}

	double L00 = sqrt(uu);
	double L11 = sqrt(vv - uv*uv / uu);
	double L01 = -uv / L00;

	double b0 = (B - A).Dot(u);
	double b1 = (A - B).Dot(v);

	double y0 = b0 / L00;
	double y1 = (b1 - L01*y0) / L11;

	t1 = y1 / L11;
	t0 = (y0 - L01*t1) / L00;

	return
		0.0f <= (float)t0 && (float)t0 <= 1.0f &&
		0.0f <= (float)t1 && (float)t1 <= 1.0f;
}

bool Polygon::PointEnclosed(const dVec2& x) const
{
	switch (m_nVertices)
	{
	case 0:
		return false;
	case 1:
		return m_vertices[0] == x;
	case 2:
	{
		return PointOnLineSegment(x, m_vertices[0], m_vertices[1]);
	}
	default:
	{
		for (int i = 0; i < m_nVertices; ++i)
		{
			const dVec2 u(x - m_vertices[i]);
			const dVec2 v(m_vertices[(i + 1) % m_nVertices] - m_vertices[i]);
			if (float(u.x*v.y - u.y*v.x) > 0.0f)
			{
				return false;
			}
		}
		return true;
	}
	}
}

Polygon Polygon::IntersectLineSegment(const fVec2& fA, const fVec2& fB) const
{
	const dVec2 A(fA);
	const dVec2 B(fB);
	
	switch (m_nVertices)
	{
	case 0:
		return Polygon();
	case 1:
		if (PointOnLineSegment(m_vertices[0], A, B))
		{
			return Polygon(m_vertices, 1);
		}
		else
		{
			return Polygon();
		}
	case 2:
	{
		double s, t;
		if (IntersectLineSegments(A, B - A, s, m_vertices[0], m_vertices[1] - m_vertices[0], t))
		{
			Polygon poly;
			poly.AddVertex(A + (B - A).Scale(s));
			return poly;
		}
		else
		{
			return Polygon();
		}
	}
	default:
	{
		Polygon poly;
		for (int i = 0; i < m_nVertices; ++i)
		{
			double s, t;
			if (IntersectLineSegments(A, B - A, s, m_vertices[i], m_vertices[(i + 1) % m_nVertices] - m_vertices[i], t))
			{
				poly.AddVertex(A + (B - A).Scale(s));
			}
		}
		if (poly.m_nVertices == 0)
		{
			if (PointEnclosed(A))
			{
				poly.AddVertex(A);
				poly.AddVertex(B);
			}
		}
		return poly;
	}
	}
}

// http://www.iitg.ernet.in/rinkulu/compgeom/slides/cvxpolyintersect.pdf
Polygon Polygon::Intersect(const Polygon& poly) const
{
	if (m_nVertices == 0 || poly.m_nVertices == 0)
	{
		return Polygon();
	}
	else if (m_nVertices == 1)
	{
		if (poly.PointEnclosed(m_vertices[0]))
		{
			return Polygon(m_vertices, 1);
		}
		else
		{
			return Polygon();
		}
	}
	else if (poly.m_nVertices == 1)
	{
		if (PointEnclosed(poly.m_vertices[0]))
		{
			return Polygon(poly.m_vertices, 1);
		}
		else
		{
			return Polygon();
		}
	}
	else if (m_nVertices == 2)
	{
		return poly.IntersectLineSegment(m_vertices[0], m_vertices[1]);
	}
	else if (poly.m_nVertices == 2)
	{
		return IntersectLineSegment(poly.m_vertices[0], poly.m_vertices[1]);
	}
	else if (
		m_boundingSquare.max.x < poly.m_boundingSquare.min.x ||
		m_boundingSquare.min.x > poly.m_boundingSquare.max.x ||
		m_boundingSquare.max.y < poly.m_boundingSquare.min.y ||
		m_boundingSquare.min.y > poly.m_boundingSquare.max.y)
	{
		return Polygon();
	}
	else
	{
		const int n0 = m_nVertices;
		const int n1 = poly.m_nVertices;

		HalfEdge edges0[6 * MAX_POLYGON_VERTICES];

		for (HalfEdge& e : edges0)
		{
			e.next = nullptr;
			e.prev = nullptr;
			e.twin = nullptr;
			e.visited = false;
			e.vert.x = FLT_MAX;
			e.vert.y = FLT_MAX;
		}


		HalfEdge* const edges1 = edges0 + 2 * n0;
		HalfEdge* const freeEdges = edges0 + 2 * (n0 + n1);
		HalfEdge* nextFreeEdge = freeEdges;

		BuildEdges(edges0);
		poly.BuildEdges(edges1);

		HalfEdge* e0 = edges0;
		HalfEdge* e1 = edges1;

		HalfEdge* eIntersection = nullptr;

		do
		{
			const dVec2 A(e0->prev->vert);
			const dVec2 B(e0->vert);

			const dVec2 C(e1->prev->vert);
			const dVec2 D(e1->vert);

			double s, t;
			const dVec2 AB = B - A;
			const dVec2 CD = D - C;

			const float ABxCD = (float)(AB.x*CD.y - AB.y*CD.x);

			if (IntersectLineSegments(A, AB, s, C, CD, t))
			{
				fVec2 X = A + AB.Scale(s);

				HalfEdge* XB = e0;
				HalfEdge* XD = e1;
				HalfEdge* BX = e0->twin;
				HalfEdge* DX = e1->twin;

				HalfEdge* EA = e0->prev;
				HalfEdge* BF = e0->next;

				HalfEdge* GC = e1->prev;
				HalfEdge* DH = e1->next;

				HalfEdge* AE = EA->twin;
				HalfEdge* FB = BF->twin;
				HalfEdge* CG = GC->twin;
				HalfEdge* HD = DH->twin;

				//         G
				//         | 
				//         C
				//         |
				// E___A___X___B___F
				//	       |
				//         D
				//         |
				//         H

				HalfEdge* AX = nextFreeEdge++;
				HalfEdge* XA = nextFreeEdge++;
				HalfEdge* CX = nextFreeEdge++;
				HalfEdge* XC = nextFreeEdge++;

				AX->twin = XA;
				XA->twin = AX;
				CX->twin = XC;
				XC->twin = CX;

				AX->vert = X;
				BX->vert = X;
				CX->vert = X;
				DX->vert = X;

				XA->vert = A;
				XC->vert = C;

				// E --> A --> X

				EA->next = AX;
				AX->prev = EA;

				XA->next = AE;
				AE->prev = XA;

				// X --> B --> F

				XB->next = BF;
				BF->prev = XB;

				FB->next = BX;
				BX->prev = FB;

				// G --> C --> X

				GC->next = CX;
				CX->prev = GC;

				XC->next = CG;
				CG->prev = XC;

				// X --> D --> H

				XD->next = DH;
				DH->prev = XD;

				HD->next = DX;
				DX->prev = HD;

				if (ABxCD > 0.0f)
				{
					eIntersection = AX;

					// A --> X --> D

					AX->next = XD;
					XD->prev = AX;

					// B --> X --> C

					BX->next = XC;
					XC->prev = BX;

					// C --> X --> A

					CX->next = XA;
					XA->prev = CX;

					// D --> X --> B

					DX->next = XB;
					XB->prev = DX;
				}
				else
				{
					eIntersection = CX;

					// A --> X --> C

					AX->next = XC;
					XC->prev = AX;

					// B --> X --> D

					BX->next = XD;
					XD->prev = BX;

					// C --> X --> B

					CX->next = XB;
					XB->prev = CX;

					// D --> X --> A

					DX->next = XA;
					XA->prev = DX;
				}
			}

			const float eps = FLT_EPSILON;

			if (ABxCD > FLT_EPSILON)
			{
				e0->visited = true;
				e0 = e0->next;
			}
			else if (ABxCD < -FLT_EPSILON)
			{
				e1->visited = true;
				e1 = e1->next;
			}
			else
			{
				const dVec2 AC(C - A);
				const dVec2 ABperp(AB.y, -AB.x);
				const float dot = (float)AC.Dot(ABperp);

				if (dot < 0.0f)
				{
					e0->visited = true;
					e0 = e0->next;
				}
				else
				{
					e1->visited = true;
					e1 = e1->next;
				}
			}
		} while (!e0->visited || !e1->visited);

		if (eIntersection != nullptr)
		{
			Polygon poly;
			HalfEdge* e = eIntersection;
			do
			{
				poly.AddVertex(e->vert);
				e = e->next;
			} while (e != eIntersection);

			return poly;
		}
		else
		{
			if (
				m_boundingSquare.min.x >= poly.m_boundingSquare.min.x &&
				m_boundingSquare.max.x <= poly.m_boundingSquare.max.x &&
				m_boundingSquare.min.y >= poly.m_boundingSquare.min.y &&
				m_boundingSquare.max.y <= poly.m_boundingSquare.max.y)
			{
				return *this;
			}
			else if (
				m_boundingSquare.min.x <= poly.m_boundingSquare.min.x &&
				m_boundingSquare.max.x >= poly.m_boundingSquare.max.x &&
				m_boundingSquare.min.y <= poly.m_boundingSquare.min.y &&
				m_boundingSquare.max.y >= poly.m_boundingSquare.max.y)
			{
				return poly;
			}
			else
			{
				assert(false);
				return Polygon();
			}
		}
	}
}