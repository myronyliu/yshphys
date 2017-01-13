#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{

}

void Polygon::AddVertex(const dVec2& vertex)
{
	m_vertices[m_nVertices++] = vertex;
}

Polygon Polygon::PruneColinearVertices() const
{
	struct VertexAngle
	{
		int iVertex;
		double cosAngle;
	};
	bool(*heapCmp)(const VertexAngle&, const VertexAngle&) = [](const VertexAngle& va0, const VertexAngle& va1)
	{
		if (va0.cosAngle < va1.cosAngle)
		{
			return true;
		}
		else if (va0.cosAngle > va1.cosAngle)
		{
			return false;
		}
		else
		{
			return va0.iVertex > va1.iVertex;
		}
	};

	VertexAngle heap[2 * MAX_POLYGON_VERTICES];
	int nHeap = m_nVertices;
	int nVert = m_nVertices;

	dVec2 AB = m_vertices[0] - m_vertices[m_nVertices - 1];
	AB = AB.Scale(1.0 / sqrt(AB.Dot(AB)));

	for (int i = 0; i < m_nVertices; ++i)
	{
		heap[i].iVertex = i;

		dVec2 BC = m_vertices[(i + 1) % m_nVertices] - m_vertices[i];
		BC = BC.Scale(1.0 / sqrt(BC.Dot(BC)));

		heap[i].cosAngle = AB.Dot(BC);

		AB = BC;
	}

	std::make_heap(heap, heap + m_nVertices, heapCmp);

	while (true)
	{
		std::pop_heap(heap, heap + nHeap, heapCmp);
		VertexAngle top = heap[--nHeap];
		if (top.cosAngle > 0.999)
		{
			const int ia = (top.iVertex - 2 + m_nVertices) % m_nVertices;
			const int ib = (top.iVertex - 1 + m_nVertices) % m_nVertices;
			const int ic = (top.iVertex + 1) % m_nVertices;
			const int id = (top.iVertex + 2) % m_nVertices;

			const dVec2& a = m_vertices[ia];
			const dVec2& b = m_vertices[ib];
			const dVec2& c = m_vertices[ic];
			const dVec2& d = m_vertices[id];

			dVec2 ab = b - a;
			dVec2 bc = c - b;
			dVec2 cd = d - c;

			ab = ab.Scale(1.0 / sqrt(ab.Dot(ab)));
			bc = bc.Scale(1.0 / sqrt(bc.Dot(bc)));
			cd = cd.Scale(1.0 / sqrt(cd.Dot(cd)));

			heap[nHeap].iVertex = ib;
			heap[nHeap].cosAngle = ab.Dot(bc);

			heap[nHeap + 1].iVertex = ic;
			heap[nHeap + 1].cosAngle = bc.Dot(cd);

			std::push_heap(heap, heap + nHeap + 1, heapCmp);
			std::push_heap(heap, heap + nHeap + 2, heapCmp);

			nHeap += 2;
			nVert--;
		}
		else
		{
			bool(*sortCmp)(const VertexAngle&, const VertexAngle&) = [](const VertexAngle& va0, const VertexAngle& va1)
			{
				return va0.iVertex < va1.iVertex;
			};

			std::sort(heap, heap + nVert, sortCmp);

			Polygon poly;

			for (int i = 0; i < nVert; ++i)
			{
				poly.m_vertices[i] = m_vertices[heap[i].iVertex];
			}
			poly.m_nVertices = nVert;
			
			return poly;
		}
	}
	assert(false);
	return Polygon();
}

// http://stackoverflow.com/questions/13101288/intersection-of-two-convex-polygons
// http://www.iitg.ernet.in/rinkulu/compgeom/slides/cvxpolyintersect.pdf
Polygon Polygon::Intersect(const Polygon& poly) const
{
	const dVec2* const p0 = m_vertices;
	const dVec2* const p1 = poly.m_vertices;

	const int n0 = m_nVertices;
	const int n1 = poly.m_nVertices;

	struct Quad
	{
		dVec2 A;
		dVec2 B;
		dVec2 C;
		dVec2 D;

		void Intersect(const Quad& quad,
			dVec2* const L, int& nL,
			dVec2* const R, int& nR) const
		{
			dVec2 p[5];

			auto IntersectLineSegments = [](const dVec2& A0, const dVec2& B0, double& t0, const dVec2& A1, const dVec2& B1, double& t1)
			{
				const dVec2 u = B0 - A0;
				const dVec2 v = B1 - A1;

				double uu = u.Dot(u);
				double vv = v.Dot(v);
				double uv = u.Dot(v);

				double L00 = sqrt(uu);
				double L11 = sqrt(vv - uv*uv / uu);
				double L01 = uv / L00;

				double b0 = (A1 - A0).Dot(u);
				double b1 = (A0 - A1).Dot(v);

				double y0 = b0 / L00;
				double y1 = (b1 - L01*y0) / L11;

				t1 = y1 / L11;
				t0 = (y0 - L01*y1) / L00;
			};

			double s, t;

			IntersectLineSegments(A, D, s, quad.A, quad.D, t);

			dVec2 L0[3];
			int nL0 = 0;

			if (0.0 < s && s < 1.0)
			{
				L0[nL0++] = (A.x > quad.A.x) ? A : quad.A;
				L0[nL0++] = A + (D - A).Scale(s);
				L0[nL0++] = (D.x > quad.D.x) ? D : quad.D;
			}

		}
	};

	struct AscendingEdge
	{
		const dVec2* A;
		AscendingEdge* next;

		bool operator <(const AscendingEdge& e) const
		{
			if (A->y < e.A->y)
			{
				return true;
			}
			else if (A->y > e.A->y)
			{
				return false;
			}
			else
			{
				return A < e.A;
			}
		}
	};

	AscendingEdge e0[MAX_POLYGON_VERTICES];
	AscendingEdge e1[MAX_POLYGON_VERTICES];

	auto FindExtremePoints = [](
		const dVec2* const p, int n,
		int& iLeft, double& xMin,
		int& iRight, double& xMax,
		int& iBottom, double& yMin,
		int& iTop, double& yMax,
		AscendingEdge* e)
	{
		yMin = p[0].y;
		yMax = p[0].y;

		xMin = p[0].x;
		xMax = p[0].y;

		iTop = 0;
		iLeft = 0;
		iRight = 0;
		iBottom = 0;

		for (int i = 1; i < n; ++i)
		{
			if (p[i].y < yMin)
			{
				yMin = p[i].y;
				iBottom = i;
			}
			else if (p[i].y > yMax)
			{
				yMax = p[i].y;
				iTop = i;
			}
			//////////////////////////

			if (p[i].x < xMin)
			{
				xMin = p[i].x;
				iLeft = i;
			}
			else if (p[i].x > xMax)
			{
				xMax = p[i].x;
				iRight = i;
			}
		}

		int iA, iB;
		int j = 0;

		e[j++].A = &p[iBottom];
		iA = (iBottom + 1) % n;
		while (iA != iTop)
		{
			iB = (iA + 1) % n;
			assert(p[iA].y <= p[iB].y);
			e[j].A = &p[iA];
			e[j - 1].next = &e[j];
			iA = iB;
			j++;
		}
		e[j - 1].next = nullptr;

		e[j++].A = &p[iBottom];
		iA = (iBottom + 1) % n;
		while (iA != iTop)
		{
			iB = (iA - 1 + n) % n;
			assert(p[iA].y <= p[iB].y);
			e[j].A = &p[iA];
			e[j - 1].next = &e[j];
			iA = iB;
			j++;
		}
		e[j - 1].next = nullptr;

		std::sort(e, e + n);

		/////////////////////////////////////

		iA = iLeft;
		while (iA != iRight)
		{
			iB = (iA + 1) % n;
			assert(p[iA].x <= p[iB].x);
			iA = iB;
		}
		while (iB != iLeft)
		{
			iA = (iB - 1 + n) % n;
			assert(p[iA].x <= p[iB].x);
			iB = iA;
		}
	};

	int iTrough0, iTrough1, iPeak0, iPeak1;
	int iLeft0, iLeft1, iRight0, iRight1;
	double xMin0, xMin1, xMax0, xMax1;
	double yMin0, yMin1, yMax0, yMax1;
	FindExtremePoints(p0, n0, iLeft0, xMin0, iRight0, xMax0, iTrough0, yMin0, iPeak0, yMax0, e0);
	FindExtremePoints(p1, n1, iLeft1, xMin1, iRight1, xMax1, iTrough1, yMin1, iPeak1, yMax1, e1);

	if (xMax0 < xMin1 ||
		xMax1 < xMin0 ||
		yMax0 < yMin1 ||
		yMax1 < yMin0)
	{
		return Polygon();
	}

	double y0 = std::min(yMin0, yMin1);

	const AscendingEdge* eL0 = e0;
	const AscendingEdge* eR0 = e0;

	const AscendingEdge* eL1 = e1;
	const AscendingEdge* eR1 = e1;

	while (true)
	{
		for (const AscendingEdge* e : { eL0, eR0, eL1, eR1 })
		{
			while (e->next->A->y < y0)
			{
				e = e->next;
			}
		}
		double ys[4] = { eL0->A->y, eR0->A->y, eL1->A->y, eR1->A->y };
		double y1 = *std::min_element(ys, ys + 4);

	}
}