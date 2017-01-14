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

	dVec2 AB(m_vertices[0] - m_vertices[m_nVertices - 1]);
	AB = AB.Scale(1.0 / sqrt(AB.Dot(AB)));

	for (int i = 0; i < m_nVertices; ++i)
	{
		heap[i].iVertex = i;

		dVec2 BC(m_vertices[(i + 1) % m_nVertices] - m_vertices[i]);
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

			const dVec2 a(m_vertices[ia]);
			const dVec2 b(m_vertices[ib]);
			const dVec2 c(m_vertices[ic]);
			const dVec2 d(m_vertices[id]);

			dVec2 ab(b - a);
			dVec2 bc(c - b);
			dVec2 cd(d - c);

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

void Polygon::BuildEdges(Edge* edges) const
{
	edges[0].vert = m_vertices[0];
	edges[0].next = &edges[1];
	edges[0].prev = &edges[m_nVertices - 1];

	for (int i = 1; i < m_nVertices - 1; ++i)
	{
		edges[i].vert = m_vertices[i];
		edges[i].next = &edges[i + 1];
		edges[i].prev= &edges[i - 1];
	}

	edges[m_nVertices - 1].vert = m_vertices[m_nVertices - 1];
	edges[m_nVertices - 1].next = &edges[0];
	edges[m_nVertices - 1].prev = &edges[m_nVertices - 2];
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

bool IntersectLineSegments(const dVec2& A, const dVec2& u, double& t0, const dVec2& B, const dVec2& v, double& t1)
{
	double uu = u.Dot(u);
	double vv = v.Dot(v);
	double uv = u.Dot(v);

	double L00 = sqrt(uu);
	double L11 = sqrt(vv - uv*uv / uu);
	double L01 = -uv / L00;

	double b0 = (B - A).Dot(u);
	double b1 = (A - B).Dot(v);

	double y0 = b0 / L00;
	double y1 = (b1 - L01*y0) / L11;

	t1 = y1 / L11;
	t0 = (y0 - L01*t1) / L00;
	return 0.0f < (float)t0 && (float)t0 < 1.0f;
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

		Edge edges0[3 * MAX_POLYGON_VERTICES];
		Edge* const edges1 = edges0 + n0;
		Edge* const freeEdges = edges0 + n0 + n1;
		Edge* nextFreeEdge = freeEdges;
		nextFreeEdge->prev = nullptr;
		nextFreeEdge->next = nullptr;

		BuildEdges(edges0);
		poly.BuildEdges(edges1);

		Edge* e0 = edges0;
		Edge* e1 = edges1;
		do
		{
			const dVec2 A(e0->vert);
			const dVec2 B(e0->next->vert);

			const dVec2 C(e1->vert);
			const dVec2 D(e1->next->vert);

			double s, t;
			const dVec2 u = B - A;
			const dVec2 v = D - C;
			if (IntersectLineSegments(A, u, s, C, v, t))
			{
				fVec2 vert = A + (B - A).Scale(s);

				for (Edge** e : { &e0, &e1 })
				{
					Edge* freeEdge = nextFreeEdge++;
					freeEdge->vert = (*e)->vert;
					freeEdge->prev = (*e);
					freeEdge->next = (*e)->next;
					freeEdge->prev->next = freeEdge;
					freeEdge->next->prev = freeEdge;

					(*e)->vert = vert;
					*e = freeEdge;
				}
			}

			const float uxv = (float)(u.x*v.y - u.y*v.x);

			if (uxv > 0.0f)
			{
				e0 = e0->next;
			}
			else
			{
				e1 = e1->next;
			}
		} while (e0 != edges0 && e1 != edges1);

		if (freeEdges->prev != nullptr && freeEdges->next != nullptr)
		{
			Polygon poly;
			Edge* e = freeEdges;
			do
			{
				poly.AddVertex(e->vert);
			} while (e != freeEdges);
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