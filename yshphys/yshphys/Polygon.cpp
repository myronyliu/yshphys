#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{
}

Polygon::Polygon(const fVec2* const x, int n) : m_nVertices(n)
{
	for (int i = 0; i < n; ++i)
	{
		m_vertices[i] = x[i];
	}
}

void Polygon::AddVertex(const fVec2& vertex)
{
	assert(m_nVertices < MAX_POLYGON_VERTICES);

	assert(fabs(vertex.x) < 10000.0f);
	assert(fabs(vertex.y) < 10000.0f);

	m_vertices[m_nVertices++] = vertex;
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

bool Solve2x2(double A00, double A01, double A11, double b0, double b1, double& x0, double& x1)
{
	double L00 = sqrt(A00);
	double L11 = sqrt(A11 - A01*A01 / A00);

	if (L11 < (double)FLT_EPSILON)
	{
		return false;
	}

	double L01 = A01 / L00;

	double y0 = b0 / L00;
	double y1 = (b1 - L01*y0) / L11;

	x1 = y1 / L11;
	x0 = (y0 - L01*x1) / L00;

	return true;
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
		Polygon poly;

		const dVec2& C = m_vertices[0];
		const dVec2& D = m_vertices[1];

		const dVec2 AB = B - A;
		const dVec2 CD = D - C;

		const dVec2 AC = C - A;

		const double AB_AB = AB.Dot(AB);

		double x0, x1;

		if (Solve2x2(AB_AB, -AB.Dot(CD), CD.Dot(CD), AC.Dot(AB), -AC.Dot(CD), x0, x1))
		{
			if (0.0f <= (float)x0 && (float)x0 <= 1.0f &&
				0.0f <= (float)x1 && (float)x1 <= 1.0f)
			{
				poly.AddVertex(fVec2(A + AB.Scale(x0)));
			}
		}
		else // the matrix was singular, meaning AB and CD are parallel
		{
			const dVec2 perp = dVec2(AB.y, -AB.x).Scale(1.0 / sqrt(AB_AB));
			const double d = abs((C - A).Dot(perp));
			if (d < (double)FLT_EPSILON)
			{
				const double tA = 0.0;
				const double tB = AB_AB;
				const double tC = (C - A).Dot(AB);
				const double tD = (D - A).Dot(AB);

				bool disjoint =
					std::max(tA, tB) < std::min(tC, tD) ||
					std::min(tA, tB) > std::max(tC, tD);

				if (!disjoint)
				{
					double t[4] = { tA, tB, tC, tD };
					std::sort(t, t + 4);
					poly.AddVertex(A + AB.Scale(t[1] / AB_AB));
					poly.AddVertex(A + AB.Scale(t[2] / AB_AB));
				}
			}
		}
		return poly;
	}
	default:
	{
		Polygon poly;

		const dVec2 AB = B - A;
		const dVec2 ABperp(-AB.y, AB.x);

		dVec2 C, D, E, F;
		C = m_vertices[m_nVertices - 1];
		int prevSide = MathUtils::iSgn((C - A).Dot(ABperp));

		for (int i = 0; i < m_nVertices; ++i)
		{
			D = m_vertices[i];
			int side = MathUtils::iSgn((D - A).Dot(ABperp));
			if (side != prevSide)
			{
				const int dSide = side - prevSide;
				prevSide = side;
				E = D;
				for (int j = i + 1; j < m_nVertices; ++j)
				{
					F = m_vertices[i];
					side = MathUtils::sgn((F - A).Dot(ABperp));
					if ((side - prevSide)*dSide < 0)
					{
						const dVec2 CD = D - C;
						const dVec2 EF = F - E;
						const dVec2 AC = C - A;
						const dVec2 AE = E - A;
						const double AB_AB = AB.Dot(AB);
						double t0, t1;

						assert(Solve2x2(AB_AB, -AB.Dot(CD), CD.Dot(CD), AC.Dot(AB), -AC.Dot(CD), t0, t1));
						assert(0.0 <= t1 && t1 <= 1.0);
						const dVec2 X = C + CD.Scale(t1);

						assert(Solve2x2(AB_AB, -AB.Dot(EF), EF.Dot(EF), AE.Dot(AB), -AE.Dot(EF), t0, t1));
						assert(0.0 <= t1 && t1 <= 1.0);
						const dVec2 Y = E + EF.Scale(t1);

						const double tA = 0.0;
						const double tB = AB_AB;
						const double tX = (X - A).Dot(AB);
						const double tY = (Y - A).Dot(AB);

						bool disjoint =
							std::max(tA, tB) < std::min(tX, tY) ||
							std::min(tA, tB) > std::max(tX, tY);
						if (!disjoint)
						{
							double t[4] = { tA, tB, tX, tY };
							std::sort(t, t + 4);
							poly.AddVertex(A + AB.Scale(t[1] / AB_AB));
							poly.AddVertex(A + AB.Scale(t[2] / AB_AB));
						}
						return poly;
					}
				}
			}
			else
			{
				prevSide = side;
				C = D;
			}
		}
		return poly;
	}
	}
}

Polygon Polygon::WalkingIntersect(const Polygon& poly) const
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

	std::vector<fVec2> pts;

	do
	{
		const dVec2 A(e0->prev->vert);
		const dVec2 B(e0->vert);

		const dVec2 C(e1->prev->vert);
		const dVec2 D(e1->vert);

		const dVec2 AB = B - A;
		const dVec2 CD = D - C;

		const double ABxCD = AB.x*CD.y - AB.y*CD.x;

		const double AB_AB = AB.Dot(AB);
		const double AB_CD = AB.Dot(CD);
		const double CD_CD = CD.Dot(CD);
		const double b0 = (C - A).Dot(AB);
		const double b1 = (A - C).Dot(CD);

		double t0, t1;

		if (Solve2x2(AB_AB, -AB_CD, CD_CD, b0, b1, t0, t1))
		{
			if (0.0 <= t0 && t0 <= 1.0 &&
				0.0 <= t1 && t1 <= 1.0)
			{
				fVec2 X = A + AB.Scale(t0);
				pts.push_back(X);

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
		}
		else
		{
			const dVec2 perp = dVec2(AB.y, -AB.x).Scale(1.0 / sqrt(AB_AB));
			const double d = abs((C - A).Dot(perp));
			if (d < (double)FLT_EPSILON)
			{
				const double tA = 0.0;
				const double tB = AB_AB;
				const double tC = (C - A).Dot(AB);
				const double tD = (D - A).Dot(AB);

				bool disjoint =
					std::max(tA, tB) < std::min(tC, tD) ||
					std::min(tA, tB) > std::max(tC, tD);

				if (!disjoint)
				{
					double t[4] = { tA, tB, tC, tD };
					std::sort(t, t + 4);
					// TODO: So we found two points on the colinear AB and CD. Now what?
					A + AB.Scale(t[1] / AB_AB);
					A + AB.Scale(t[2] / AB_AB);
				}
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
	} while (!e0->visited && !e1->visited);

	if (eIntersection != nullptr)
	{
		Polygon intersection;
		HalfEdge* e = eIntersection;
		do
		{
			intersection.AddVertex(e->vert);
			e = e->next;
		} while (e != eIntersection);

		return intersection;
	}
	else
	{
		return Polygon();
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
	else
	{
		return DoIntersect(poly);
	}
}

Polygon Polygon::DoIntersect(const Polygon& poly) const
{
	int n0 = m_nVertices;
	int n1 = poly.m_nVertices;

	const fVec2* x0 = m_vertices;
	const fVec2* x1 = poly.m_vertices;

	Vertex verts[2 * MAX_POLYGON_VERTICES];
	Vertex* freeVert = verts + n1;

	for (int i = 0; i < n1; ++i)
	{
		verts[i].x = x1[i];
		verts[i].next = &verts[(i + 1) % n1];
		verts[i].prev = &verts[(i - 1 + n1) % n1];
	}

	Vertex* vInit = verts;
	Vertex* vIntersection = nullptr;

	for (int i = 0; i < n0; ++i)
	{
		const dVec2 A = x0[(i + 0) % n0];
		const dVec2 B = x0[(i + 1) % n0];

		const dVec2 AB = B - A;
		const dVec2 ABperp(-AB.y, AB.x);

		Vertex* vi = nullptr;

		dVec2 C = vInit->prev->x;
		dVec2 AC = C - A;
		int prevSide = MathUtils::iSgn(AC.Dot(ABperp));

		Vertex* v = vInit;

		int dSide;

		do
		{
			C = v->x;
			AC = C - A;
			const int side = MathUtils::iSgn(AC.Dot(ABperp));
			if (side != prevSide)
			{
				vi = v;
				v = v->next;
				dSide = side - prevSide;
				prevSide = side;
				break;
			}
			v = v->next;
			prevSide = side;
		} while (v != vInit);

		if (vi != nullptr)
		{
			Vertex* vf = nullptr;
			while (v != vInit)
			{
				C = v->x;
				AC = C - A;
				const int side = MathUtils::iSgn(AC.Dot(ABperp));
				if ((side - prevSide)*dSide < 0)
				{
					vf = v;
					break;
				}
				v = v->next;
				prevSide = side;
			}
			assert(vf != nullptr);
			assert(vi != vf);

			if (dSide > 0)
			{
				Vertex* tmp = vi;
				vi = vf;
				vf = tmp;
			}

			Vertex* u[2] = { vi, vf };
			dVec2 X[2];

			const double AB_AB = AB.Dot(AB);

			for (int j : { 0, 1 })
			{
				const dVec2 C = u[j]->prev->x;
				const dVec2 D = u[j]->x;
				const dVec2 CD = D - C;
				const dVec2 AC = C - A;

				const double AB_CD = AB.Dot(CD);
				const double CD_CD = CD.Dot(CD);
				const double b0 = AC.Dot(AB);
				const double b1 = -AC.Dot(CD);

				double t0, t1;

				assert(Solve2x2(AB_AB, -AB_CD, CD_CD, b0, b1, t0, t1));
				assert(0.0 <= t1 && t1 <= 1.0);

//				X[j] = A + AB.Scale(t0);
				X[j] = C + CD.Scale(t1);
			}

			Vertex* const newVert = freeVert++;

			vi->x = X[0];
			newVert->x = X[1];

			vi->next = newVert;

			newVert->next = vf;
			newVert->prev = vi;

			vf->prev = newVert;

			vInit = newVert;

			vIntersection = vi;
		}
	}

	Polygon intersection;
	if (vIntersection != nullptr)
	{
		Vertex* v = vIntersection;
		do
		{
			intersection.AddVertex(v->x);
			v = v->next;
		} while (v != vIntersection);
	}

	return intersection;
}
