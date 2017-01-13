#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{
	m_boundingSquare.min.x = FLT_MAX;
	m_boundingSquare.min.y = FLT_MAX;

	m_boundingSquare.max.x = -FLT_MAX;
	m_boundingSquare.max.x = -FLT_MAX;
}

void Polygon::AddVertex(const fVec2& vertex)
{
	m_vertices[m_nVertices++] = vertex;

	m_boundingSquare.min.x = std::min(m_boundingSquare.min.x, vertex.x);
	m_boundingSquare.min.y = std::min(m_boundingSquare.min.y, vertex.y);

	m_boundingSquare.max.x = std::max(m_boundingSquare.max.x, vertex.x);
	m_boundingSquare.max.y = std::max(m_boundingSquare.max.y, vertex.y);
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

// http://www.iitg.ernet.in/rinkulu/compgeom/slides/cvxpolyintersect.pdf
Polygon Polygon::Intersect(const Polygon& poly) const
{
	if (m_nVertices == 0 || poly.m_nVertices == 0 ||
		m_boundingSquare.max.x < poly.m_boundingSquare.min.x ||
		m_boundingSquare.min.x > poly.m_boundingSquare.max.x ||
		m_boundingSquare.max.y < poly.m_boundingSquare.min.y ||
		m_boundingSquare.min.y > poly.m_boundingSquare.max.y)
	{
		return Polygon();
	}

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

	auto IntersectLineSegments = [](const dVec2& A, const dVec2& u, double& t0, const dVec2& B, const dVec2& v, double& t1)
	{
		double uu = u.Dot(u);
		double vv = v.Dot(v);
		double uv = u.Dot(v);

		double L00 = sqrt(uu);
		double L11 = sqrt(vv - uv*uv / uu);
		double L01 = uv / L00;

		double b0 = (B - A).Dot(u);
		double b1 = (A - B).Dot(v);

		double y0 = b0 / L00;
		double y1 = (b1 - L01*y0) / L11;

		t1 = y1 / L11;
		t0 = (y0 - L01*y1) / L00;
	};

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
		IntersectLineSegments(A, u, s, C, v, t);

		if (0.0f <= (float)s && 1.0f <= (float)t)
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

		const float uxv(u.x*v.y - u.y*v.x);

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
			m_boundingSquare.min.y >= poly.m_boundingSquare.min.y &&
			m_boundingSquare.max.y >= poly.m_boundingSquare.max.y)
		{
			return poly;
		}
		else
		{
			assert(false);
		}
	}
}