#include "stdafx.h"
#include "QuickHull.h"

void QuickHull::FaceFIFO::Push(QuickHull::Face* face)
{
	face->FIFO = this;

	if (m_iStart == m_iEnd)
	{
		face->iFIFO = m_iEnd;
		m_queue[m_iEnd].face = face;
		m_queue[m_iEnd].valid = true;
		m_iEnd = (m_iEnd + 1) % QUICKHULL_FACEFIFO_MAXSIZE;
	}
	else
	{
		while (m_iStart != m_iEnd)
		{
			int iLast = (m_iEnd - 1 + QUICKHULL_FACEFIFO_MAXSIZE) % QUICKHULL_FACEFIFO_MAXSIZE;
			if (m_queue[iLast].valid)
			{
				face->iFIFO = m_iEnd;
				m_queue[m_iEnd].face = face;
				m_queue[m_iEnd].valid = true;
				m_iEnd = (m_iEnd + 1) % QUICKHULL_FACEFIFO_MAXSIZE;
			}
			else
			{
				m_queue[iLast].face = nullptr;
				m_iEnd = iLast;
			}
		}
	}
}
QuickHull::Face* QuickHull::FaceFIFO::Pop()
{
	while (m_iStart != m_iEnd)
	{
		if (m_queue[m_iStart].valid)
		{
			m_queue[m_iStart].valid = false;
			const int i = m_iStart;
			m_iStart = (m_iStart + 1) % QUICKHULL_FACEFIFO_MAXSIZE;
			m_queue[i].face->FIFO = nullptr;
			m_queue[i].face->iFIFO = -1;
			return m_queue[i].face;
		}
		else
		{
			m_queue[m_iStart].face = nullptr;
			m_iStart = (m_iStart + 1) % QUICKHULL_FACEFIFO_MAXSIZE;
		}
	}
	return nullptr;
}
int QuickHull::FaceFIFO::Size()
{
	return (m_iStart - m_iEnd + QUICKHULL_FACEFIFO_MAXSIZE) % QUICKHULL_FACEFIFO_MAXSIZE;
}

QuickHull::QuickHull(const fVec3* verts, int nVerts) :
	m_verts(verts),
	m_nVerts(nVerts),
	m_nHorizonEdges(0)
{
}

QuickHull::~QuickHull()
{
	for (HalfEdge* e : m_allocatedEdges)
	{
		delete e;
	}
	for (Face* f : m_allocatedFaces)
	{
		delete f;
	}
}

void QuickHull::CarveHorizon(const fVec3& fEye, Face* visibleFace)
{
	const dVec3 dEye(fEye);

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->isHorizon = false;
	}
	m_nHorizonEdges = 0;

	m_orphanedVerts.clear();

	struct EdgeStack
	{
	public:
		void Push(HalfEdge* edge) { e[n++] = edge; }
		HalfEdge* Pop() { return e[--n]; }
		bool Empty() const { return n == 0; }
	private:
		HalfEdge* e[1024];
		int n = 0;
	}
	edgeStack;

	std::vector<Face*> visitedFaces;
	auto MarkFaceAsVisited = [&](Face* face)
	{
		visitedFaces.push_back(face);
		face->visited = true;
	};

	for (HalfEdge* e : { visibleFace->edge, visibleFace->edge->prev, visibleFace->edge->next })
	{
		edgeStack.Push(e);
		MarkFaceAsVisited(e->twin->face);
	}
	MarkFaceAsVisited(visibleFace);
	visibleFace->visible = true;

	int nFreedEdges = 0;
	std::vector<HalfEdge*> internalEdges;

	HalfEdge* horizonStart = nullptr;

	while (!edgeStack.Empty())
	{
		HalfEdge* edge = edgeStack.Pop();
		HalfEdge* twin = edge->twin;

		Face* opposingFace = twin->face;

		const dVec3& n = opposingFace->normal;
		const dVec3 x(*edge->vert);
		const double dot = (dEye - x).Dot(n);

		//		assert(abs(dot) > 8.0*FLT_MIN);

		if ((float)dot > 0.0f)
		{
			if (!twin->prev->twin->face->visited)
			{
				edgeStack.Push(twin->prev);
				MarkFaceAsVisited(twin->prev->twin->face);
			}
			if (!twin->next->twin->face->visited)
			{
				edgeStack.Push(twin->next);
				MarkFaceAsVisited(twin->next->twin->face);
			}
			internalEdges.push_back(edge);
			internalEdges.push_back(twin);

			opposingFace->visible = true;
		}
		else
		{
			horizonStart = edge;
		}
	}

	HalfEdge* edge = horizonStart;
	do
	{
		assert(m_nHorizonEdges < QUICKHULL_MAXHORIZONEDGES);
		m_horizonEdges[m_nHorizonEdges++] = edge;
		edge->isHorizon = true;

		HalfEdge* e0 = edge;

		do
		{
			edge = edge->next->twin;

		} while (edge->face->visible);

		/////////////////////////////////////////
		// Make sure the horizon is not "pinched"
		HalfEdge* e = edge;
		while (e != e0)
		{
			assert(!e->face->visible);
			e = e->next->twin;
		}
		/////////////////////////////////////////

		edge = edge->twin;

	} while (edge != horizonStart);

	std::vector<Face*> visibleFaces;
	for (Face* face : visitedFaces)
	{
		if (face->visible)
		{
			visibleFaces.push_back(face);

			m_orphanedVerts.insert(m_orphanedVerts.end(), face->vertSet.begin(), face->vertSet.end());
		}

		face->visible = false;
		face->visited = false;
	}

	for (HalfEdge* edge : internalEdges)
	{
//		delete edge;
	}
	for (Face* face : visibleFaces)
	{
//		delete face;
	}
}


bool QuickHull::PatchHorizon(const fVec3* eye, double dSlack)
{
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		HalfEdge* curr = m_horizonEdges[i];

		HalfEdge* prev = new HalfEdge;
		HalfEdge* next = new HalfEdge;
		m_allocatedEdges.push_back(prev);
		m_allocatedEdges.push_back(next);

		Face* face = new Face();
		m_allocatedFaces.push_back(face);
		m_faceFIFO.Push(face);

		face->edge = curr;
		const dVec3 B(*(curr->vert));
		const dVec3 A(*(curr->twin->vert));
		const dVec3 D(*eye);
		dVec3 n = (A - D).Cross(B - D);
//		assert(n.Dot(n) > FLT_MIN);
		if (n.Dot(n) < FLT_MIN)
		{
			return false;
		}
		n = n.Scale(1.0 / sqrt(n.Dot(n)));
		const double d = D.Dot(n);
		face->normal = n;

//		assert(d >= 0.0);

		prev->face = face;
		curr->face = face;
		next->face = face;

		prev->vert = curr->twin->vert;
		next->vert = eye;

		prev->next = curr;
		curr->next = next;
		next->next = prev;

		prev->prev = next;
		curr->prev = prev;
		next->prev = curr;
	}

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		m_horizonEdges[i]->next->twin = m_horizonEdges[(i + 1) % m_nHorizonEdges]->prev;
		m_horizonEdges[i]->prev->twin = m_horizonEdges[(i - 1 + m_nHorizonEdges) % m_nHorizonEdges]->next;
	}

	for (const fVec3* vert : m_orphanedVerts)
	{
		for (int i = 0; i < m_nHorizonEdges; ++i)
		{
			Face* f = m_horizonEdges[i]->face;
			dVec3 n = f->normal;

			if (n.Dot(dVec3(*vert)) > dSlack)
			{
				f->vertSet.push_back(vert);
				break;
			}
		}
	}
	return true;
}

bool QuickHull::Expand(double dSlack)
{
	while (m_faceFIFO.Size() > 0)
	{
		Face* f = m_faceFIFO.Pop();
		const dVec3 n(f->normal);
		std::vector<const fVec3*>& vSet = f->vertSet;
		if (!vSet.empty())
		{
			const fVec3* vFarthest = nullptr;
			double dFarthest = 0.0;

			for (const fVec3* v : vSet)
			{
				const double d = n.Dot(dVec3(*v) - dVec3(*(f->edge->vert)));
				if (d > dFarthest)
				{
					vFarthest = v;
					dFarthest = d;
				}
			}
			assert(vFarthest != nullptr);

			CarveHorizon(*vFarthest, f);

			if (!PatchHorizon(vFarthest, dSlack))
			{
				return false;
			}

//			SanityCheck();

			return true;
		}
	}
	assert(false);
	return false;
}
