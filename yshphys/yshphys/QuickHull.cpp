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
				break;
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

QuickHull::QuickHull(const fVec3* verts, int nVerts, double dSlack) :
	m_verts(verts),
	m_nVerts(nVerts),
	m_nHorizonEdges(0),
	m_entryEdge(nullptr),
	m_dSlack(dSlack)
{
	InitTetrahedron();
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

void QuickHull::InitTetrahedron()
{
	float min[3] = { FLT_MAX, FLT_MAX, FLT_MAX };
	float max[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	const fVec3* v[6];
	const fVec3** const vMin = v + 0;
	const fVec3** const vMax = v + 3;

	for (int i = 0; i < m_nVerts; ++i)
	{
		for (int dim = 0; dim < 3; ++dim)
		{
			if (m_verts[i][dim] < min[dim])
			{
				vMin[dim] = &m_verts[i];
				min[dim] = m_verts[i][dim];
			}
			if (m_verts[i][dim] > max[dim])
			{
				vMax[dim] = &m_verts[i];
				max[dim] = m_verts[i][dim];
			}
		}
	}

	float maxVolume = 0.0f;
	const fVec3* vOptimal[4] = { nullptr, nullptr, nullptr, nullptr };

	for (int iA = 0; iA < 6; ++iA)
	{
		for (int iB = iA + 1; iB < 6; ++iB)
		{
			for (int iC = iB + 1; iC < 6; ++iC)
			{
				for (int iD = iC + 1; iD < 6; ++iD)
				{
					const fVec3* A = v[iA];
					const fVec3* B = v[iB];
					const fVec3* C = v[iC];
					const fVec3* D = v[iD];

					float volume = fabs((*B - *A).Cross(*C - *A).Dot(*D - *A));
					if (volume > maxVolume)
					{
						maxVolume = volume;
						vOptimal[0] = A;
						vOptimal[1] = B;
						vOptimal[2] = C;
						vOptimal[3] = D;
					}
				}
			}
		}
	}

	HalfEdge* edges[4][4];

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < i; ++j)
		{
			edges[i][j] = new HalfEdge;
			edges[i][j]->vert = vOptimal[j];

			edges[j][i] = new HalfEdge;
			edges[j][i]->vert = vOptimal[i];

			edges[i][j]->twin = edges[j][i];
			edges[j][i]->twin = edges[i][j];

			m_allocatedEdges.push_back(edges[i][j]);
			m_allocatedEdges.push_back(edges[j][i]);
		}
	}

	m_entryEdge = edges[0][1];

	Face* tetraFaces[4] = { nullptr, nullptr, nullptr, nullptr };

	for (int f = 0; f < 4; ++f)
	{
		const int iA = (f + 0) % 4;
		const int iB = (f + 1) % 4;
		const int iC = (f + 2) % 4;
		const int iD = (f + 3) % 4;
		const fVec3* A = vOptimal[iA];
		const fVec3* B = vOptimal[iB];
		const fVec3* C = vOptimal[iC];
		const fVec3* D = vOptimal[iD];

		dVec3 n = (*C - *B).Cross(*D - *B);
		assert(n.Dot(n) > FLT_MIN);
		n = n.Scale(1.0 / sqrt(n.Dot(n)));

		Face* face = new Face;
		tetraFaces[f] = face;
		m_faceFIFO.Push(face);

		HalfEdge* e[3];

		if (n.Dot(*B - *A) < 0.0)
		{
			e[0] = edges[iC][iB];
			e[1] = edges[iB][iD];
			e[2] = edges[iD][iC];

			n = -n;
		}
		else
		{
			e[0] = edges[iB][iC];
			e[1] = edges[iC][iD];
			e[2] = edges[iD][iB];
		}

		for (int i = 0; i < 3; ++i)
		{
			e[i]->next = e[(i + 1) % 3];
			e[i]->prev = e[(i + 2) % 3];
			e[i]->face = face;
		}
		face->edge = e[0];
		face->normal = n;
	}

	int iOptimal[4] =
	{
		(int)(vOptimal[0] - m_verts) / (int)sizeof(fVec3*),
		(int)(vOptimal[1] - m_verts) / (int)sizeof(fVec3*),
		(int)(vOptimal[2] - m_verts) / (int)sizeof(fVec3*),
		(int)(vOptimal[3] - m_verts) / (int)sizeof(fVec3*)
	};
	std::sort(iOptimal, iOptimal + 4);

	auto AssignVertToFace = [&](const fVec3* v)
	{
		for (Face* face : tetraFaces)
		{
			const double d = (*v - *face->edge->vert).Dot(face->normal);
			if (d > m_dSlack)
			{
				face->vertSet.push_back(v);
				break;
			}
		}
	};

	for (int i = 0; i < iOptimal[0]; ++i)
	{
		AssignVertToFace(&m_verts[i]);
	}
	for (int i = iOptimal[0]; i < iOptimal[1]; ++i)
	{
		AssignVertToFace(&m_verts[i]);
	}
	for (int i = iOptimal[1]; i < iOptimal[2]; ++i)
	{
		AssignVertToFace(&m_verts[i]);
	}
	for (int i = iOptimal[2]; i < iOptimal[3]; ++i)
	{
		AssignVertToFace(&m_verts[i]);
	}
	for (int i = iOptimal[3]; i < m_nVerts; ++i)
	{
		AssignVertToFace(&m_verts[i]);
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

			m_faceFIFO.InvalidateElement(face->iFIFO);
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


bool QuickHull::PatchHorizon(const fVec3* eye)
{
	if (m_nHorizonEdges > 0)
	{
		m_entryEdge = m_horizonEdges[0];
	}

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

	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		Face* f = m_horizonEdges[i]->face;
		assert(f->vertSet.empty());
	}
	for (const fVec3* vert : m_orphanedVerts)
	{
		for (int i = 0; i < m_nHorizonEdges; ++i)
		{
			Face* f = m_horizonEdges[i]->face;
			dVec3 n = f->normal;

			if (n.Dot(dVec3(*vert) - dVec3(*eye)) > m_dSlack)
			{
				f->vertSet.push_back(vert);
				break;
			}
		}
	}
	return true;
}

bool QuickHull::Expand()
{
	while (m_faceFIFO.Size() > 0)
	{
		Face* f = m_faceFIFO.Pop();

		/////////////////////////////////////////////////

		if (m_entryEdge != nullptr)
		{
			Face* entryFace = m_entryEdge->face;

			std::stack<Face*> faceStack;
			std::vector<Face*> visitedFaces;
			auto MarkFaceAsVisited = [&](Face* face)
			{
				visitedFaces.push_back(face);
				face->visited = true;
			};

			faceStack.push(entryFace);
			MarkFaceAsVisited(entryFace);

			bool foundFace = false;

			while (!faceStack.empty())
			{
				Face* face = faceStack.top();
				faceStack.pop();
				
				if (face == f)
				{
					foundFace = true;
					break;
				}

				HalfEdge* es[3];

				es[0] = face->edge;
				es[1] = face->edge->next;
				es[2] = face->edge->prev;

				for (HalfEdge* edge : es)
				{
					if (!edge->twin->face->visited)
					{
						faceStack.push(edge->twin->face);
						MarkFaceAsVisited(edge->twin->face);
					}
				}
			}
			for (Face* face : visitedFaces)
			{
				face->visited = false;
			}
			assert(foundFace);
		}

		/////////////////////////////////////////////////

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

			if (vFarthest != nullptr)
			{
				CarveHorizon(*vFarthest, f);

				if (!PatchHorizon(vFarthest))
				{
					assert(false);
					return false;
				}

				//			SanityCheck();

				return true;
			}
			else
			{
				assert(false);
				return false;
			}
		}
	}
	return false;
}

void QuickHull::BuildHull()
{
	while (Expand())
	{
	}
}

Mesh* QuickHull::ExportConvexMesh() const
{
	Mesh* mesh = new Mesh;

	if (m_entryEdge == nullptr)
	{
		return nullptr;
	}

	std::stack<Face*> faceStack;
	std::vector<Face*> visitedFaces;

	auto MarkFaceAsVisited = [&](Face* face)
	{
		visitedFaces.push_back(face);
		face->visited = true;
	};

	Face* entryFace = m_entryEdge->face;
	faceStack.push(entryFace);
	MarkFaceAsVisited(entryFace);

	std::map<const Face*, int> f_i;
	std::map<int, const Face*> i_f;

	std::map<const HalfEdge*, int> e_i;
	std::map<int, const HalfEdge*> i_e;

	std::map<const fVec3*, int> v_i;
	std::map<int, const fVec3*> i_v;

	int nFaces = 0;
	int nEdges = 0;
	int nVerts = 0;

	while (!faceStack.empty())
	{
		Face* face = faceStack.top();
		faceStack.pop();

		f_i[face] = nFaces;
		i_f[nFaces] = face;
		nFaces++;

		HalfEdge* e = face->edge;
		HalfEdge* e0 = e;
		do
		{
			e_i[e] = nEdges;
			i_e[nEdges] = e;
			nEdges++;
			std::map<const fVec3*, int>::const_iterator it = v_i.find(e->vert);
			if (it == v_i.end())
			{
				v_i[e->vert] = nVerts;
				i_v[nVerts] = e->vert;
				nVerts++;
			}
			if (!e->twin->face->visited)
			{
				faceStack.push(e->twin->face);
				MarkFaceAsVisited(e->twin->face);
			}
			e = e->next;

		} while (e != e0);
	}
	for (Face* face : visitedFaces)
	{
		face->visited = false;
	}

	assert(nEdges % 2 == 0);

	mesh->AllocateMesh(nEdges / 2, nFaces, nVerts);

	for (int i = 0; i < nEdges; ++i)
	{
		Mesh::HalfEdge& e_out = mesh->m_halfEdges[i];
		const HalfEdge* e = i_e[i];
		e_out.iFace = f_i[e->face];
		e_out.iNext = e_i[e->next];
		e_out.iPrev = e_i[e->prev];
		e_out.iTwin = e_i[e->twin];
		e_out.iVert = v_i[e->vert];
	}
	for (int i = 0; i < nFaces; ++i)
	{
		Mesh::Face& f_out = mesh->m_faces[i];
		const Face* f = i_f[i];
		f_out.iEdge = e_i[f->edge];
		f_out.normal = f->normal;
	}
	for (int i = 0; i < nVerts; ++i)
	{
		mesh->m_verts[i] = *i_v[i];
	}
}

void QuickHull::DebugDraw(DebugRenderer* renderer) const
{
	const float k = 0.02f;

	if (m_entryEdge != nullptr)
	{
		Face* entryFace = m_entryEdge->face;

		std::stack<Face*> faceStack;
		std::vector<Face*> visitedFaces;
		auto MarkFaceAsVisited = [&](Face* face)
		{
			visitedFaces.push_back(face);
			face->visited = true;
		};

		faceStack.push(entryFace);
		MarkFaceAsVisited(entryFace);

		while (!faceStack.empty())
		{
			Face* face= faceStack.top();
			faceStack.pop();

			HalfEdge* es[3];

			es[0] = face->edge;
			es[1] = face->edge->next;
			es[2] = face->edge->prev;

			fVec3 ABC[3];
			ABC[0] = *es[0]->vert;
			ABC[1] = *es[1]->vert;
			ABC[2] = *es[2]->vert;

			renderer->DrawPolygon(ABC, 3, fVec3(1.0f, 1.0f, 1.0f), false, true);
			fVec3 n = face->normal;
			fVec3 cross = fVec3(0.0f, 0.0f, 1.0f).Cross(n);
			float sin = sqrtf(cross.Dot(cross));
			float cos = n.z;

			fQuat rot = fQuat::Identity();
			if (sin > 0.00001f)
			{
				rot = fQuat(cross.Scale(1.0f / sin), std::atan2f(sin, cos));
			}

			auto FaceArea = [](const Face* face)
			{
				const fVec3 A = *face->edge->vert;
				const fVec3 B = *face->edge->next->vert;
				const fVec3 C = *face->edge->prev->vert;
				const fVec3 cross = (B - A).Cross(C - A);
				return sqrtf(cross.Dot(cross));
			};

			const float edgeWidth = sqrtf(FaceArea(face)) * 0.01f;
			const float len = 0.2f;
			const fVec3 faceCenter = (ABC[0] + ABC[1] + ABC[2]).Scale(1.0f / 3.0f);
			renderer->DrawBox(edgeWidth, edgeWidth, len, faceCenter + n.Scale(len), rot, fVec3(1.0f, 1.0f, 1.0f), false, true);

			for (const fVec3* vert : face->vertSet)
			{
//				renderer->DrawLine(*vert, faceCenter, fVec3(1.0f, 0.0f, 0.0f));
				renderer->DrawBox(k, k, k, *vert, fQuat::Identity(), fVec3(1.0f, 0.0f, 0.0f), false, false);
			}

			for (HalfEdge* edge : es)
			{
				if (!edge->twin->face->visited)
				{
					faceStack.push(edge->twin->face);
					MarkFaceAsVisited(edge->twin->face);
				}
			}
		}
		for (Face* face : visitedFaces)
		{
			face->visited = false;
		}
	}
	for (int i = 0; i < m_nHorizonEdges; ++i)
	{
		const HalfEdge* edge = m_horizonEdges[i];
		fVec3 color(0.0f, 0.0f, 0.0f);
		float kWidth = 0.01f;

		color = fVec3(0.0f, 1.0f, 0.0f);
		kWidth *= 2.0f;

		const fVec3 A = *edge->vert;
		const fVec3 B = *edge->twin->vert;

		fVec3 v = B - A;
		float vLen = sqrtf(v.Dot(v));
		fVec3 n = v.Scale(1.0f / vLen);
		fVec3 cross = fVec3(0.0f, 0.0f, 1.0f).Cross(n);
		float sin = sqrtf(cross.Dot(cross));
		float cos = n.z;

		fQuat rot = fQuat::Identity();
		if (sin > 0.00001f)
		{
			rot = fQuat(cross.Scale(1.0f / sin), std::atan2f(sin, cos));
		}

		auto FaceArea = [](const Face* face)
		{
			const fVec3 A = *face->edge->vert;
			const fVec3 B = *face->edge->next->vert;
			const fVec3 C = *face->edge->prev->vert;
			const fVec3 cross = (B - A).Cross(C - A);
			return sqrtf(cross.Dot(cross));
		};

		const float edgeWidth = sqrtf(std::min(FaceArea(edge->face), FaceArea(edge->twin->face)))* kWidth;

		renderer->DrawBox(edgeWidth, edgeWidth, vLen*0.5f, (A + B).Scale(0.5f), rot, color, false, false);
	}
}
