#include "stdafx.h"
#include "Mesh.h"
#include "QuickHull.h"

Mesh::Mesh() :
	m_halfEdges(nullptr),
	m_faces(nullptr),
	m_verts(nullptr),
	m_nHalfEdges(0),
	m_nFaces(0),
	m_nVerts(0)
{
}

Mesh::Mesh(const fVec3* const pointCloud, int nPoints) : Mesh()
{
	QuickHull qh(pointCloud, nPoints, 0.001);
	qh.BuildHull();
	qh.ExportConvexMesh(*this);
}

Mesh::~Mesh()
{
	delete[] m_halfEdges;
	delete[] m_faces;
	delete[] m_verts;
}

void Mesh::AllocateMesh(int nEdges, int nFaces, int nVerts)
{
	delete[] m_halfEdges;
	delete[] m_faces;
	delete[] m_verts;
	
	m_nHalfEdges = 2 * nEdges;
	m_nFaces = nFaces;
	m_nVerts = nVerts;

	m_halfEdges = new HalfEdge[m_nHalfEdges];
	m_faces = new Face[m_nFaces];
	m_verts = new fVec3[m_nVerts];
}

void Mesh::FaceList::AddFace(const fVec3* verts, const int nVerts, const dVec3& normal)
{
	FacePartition fp;
	fp.iFirstVert = (int)m_verts.size();
	fp.normal = normal;
	m_verts.insert(m_verts.end(), verts, verts + nVerts);
	m_faces.push_back(fp);
}

int Mesh::FaceList::GetNumFaces() const
{
	return (int)m_faces.size();
}

Mesh::FaceList::Face Mesh::FaceList::GetFace(int iFace) const
{
	Mesh::FaceList::Face f;
	if (iFace > (int)m_faces.size() - 1)
	{
		return f;
	}
	else
	{
		int jMax = (iFace == (int)m_faces.size() - 1) ? (int)m_verts.size() : m_faces[iFace + 1].iFirstVert;
		for (int j = m_faces[iFace].iFirstVert; j < jMax; ++j)
		{
			f.verts.push_back(m_verts[j]);
		}
		f.normal = m_faces[iFace].normal;
		return f;
	}
}

Mesh::FaceList Mesh::GetFaces() const
{
	Mesh::FaceList fl;

	for (int i = 0; i < m_nFaces; ++i)
	{
		const Face& f = m_faces[i];
		std::vector<fVec3> faceVerts;
		int e_i = f.iEdge;
		int e_i_0 = e_i;
		do
		{
			const HalfEdge& e = m_halfEdges[e_i];
			faceVerts.push_back(m_verts[e.iVert]);
			e_i = e.iNext;

		} while (e_i != e_i_0);

		fl.AddFace(&faceVerts[0], (int)faceVerts.size(), f.normal);
	}
	return fl;
}

int Mesh::ISupportEdgeLocal(const dVec3& v, int i_) const
{
	int i = i_;
	while (true)
	{
		const HalfEdge* e = &m_halfEdges[i];
		const int t = e->iTwin;
		int i1 = e->iNext;
		int i1_best = -1;
		double d_best = (double)FLT_MIN;

		while (i1 != t)
		{
			HalfEdge* e1 = &m_halfEdges[i1];

			const int j1 = e1->iTwin;
			const HalfEdge* t1 = &m_halfEdges[j1];

			const dVec3 B(m_verts[e1->iVert]);
			const dVec3 A(m_verts[t1->iVert]);
			const double d = v.Dot(B - A);

			if (d > d_best)
			{
				i1_best = i1;
				d_best = d;
			}
			i1 = t1->iNext;
		}

		if (i1_best < 0)
		{
			break;
		}
		else
		{
			i = i1_best;
		}
	}
	return i;
}

dVec3 Mesh::SupportLocal(const dVec3& v) const
{
	int i = -1;
	const dVec3 v_v = v.Times(v);
	for (int dim = 0; dim < 3; dim++)
	{
		if (v_v[dim] >= v_v[(dim + 1) % 3] && v_v[dim] >= v_v[(dim + 2) % 3])
		{
			i = (v[dim] < 0.0) ? m_iCardinalEdges[dim][0] : m_iCardinalEdges[dim][1];
			const HalfEdge* e = &m_halfEdges[i];
			const int j = e->iTwin;
			const HalfEdge* t = &m_halfEdges[j];
			const dVec3 B(m_verts[e->iVert]);
			const dVec3 A(m_verts[t->iVert]);
			if (v.Dot(B - A) < 0.0)
			{
				i = j;
			}
			break;
		}
	}
	const int iSupportEdge = ISupportEdgeLocal(v, i);
	const HalfEdge& supportEdge = m_halfEdges[iSupportEdge];
	const int iSupportVert = supportEdge.iVert;
	return dVec3(m_verts[iSupportVert]);
}

void Mesh::InitCardinalEdges()
{
	m_iCardinalEdges[0][0] = ISupportEdgeLocal(dVec3(-1.0, 0.0, 0.0), 0);

	m_iCardinalEdges[1][0] = ISupportEdgeLocal(dVec3(0.0, -1.0, 0.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[1][1] = ISupportEdgeLocal(dVec3(0.0, 1.0, 0.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[2][0] = ISupportEdgeLocal(dVec3(0.0, 0.0, -1.0), m_iCardinalEdges[0][0]);
	m_iCardinalEdges[2][1] = ISupportEdgeLocal(dVec3(0.0, 0.0, 1.0), m_iCardinalEdges[0][0]);

	m_iCardinalEdges[0][1] = ISupportEdgeLocal(dVec3(1.0, 0.0, 0.0), m_iCardinalEdges[2][1]);

	for (int dim = 0; dim < 3; ++dim)
	{
		m_localOOBB.min[dim] = m_verts[m_halfEdges[m_iCardinalEdges[dim][0]].iVert][dim];
		m_localOOBB.max[dim] = m_verts[m_halfEdges[m_iCardinalEdges[dim][1]].iVert][dim];
	}
}

dVec3 Mesh::CenterOfMassLocal_Solid(double& VOLUME) const
{
	dVec3 o(0.0, 0.0, 0.0);
	for (int i = 0; i < m_nVerts; ++i)
	{
		o = o + dVec3(m_verts[i]);
	}
	o = o.Scale(m_nVerts);

	VOLUME = 0.0;
	dVec3 COM(0.0, 0.0, 0.0);

	for (int i = 0; i < m_nFaces; ++i)
	{
		dVec3 v[64];
		int n = 0;

		const Face& f = m_faces[i];

		int iEdge = f.iEdge;
		int iEdge0 = iEdge;
		do
		{
			const HalfEdge& edge = m_halfEdges[iEdge];
			int iVert = edge.iVert;
			v[n++] = dVec3(m_verts[iVert]);
			iEdge = edge.iNext;

		} while (iEdge != iEdge0);

		int iA = 0;
		int iB = 1;
		int iC = n - 1;
		int iD = n - 2;

		double volume = 0.0;
		dVec3 com(0.0, 0.0, 0.0);

		while (iB < iD)
		{
			const dVec3& A = v[iA];
			const dVec3& B = v[iB];
			const dVec3& C = v[iC];
			const dVec3& D = v[iD];

			double volume0 = volume;
			double dVolume = (B - A).Cross(C - A).Dot(A - o);
			volume += dVolume;
			com = com.Scale(volume0 / volume) + (A + B + C + o).Scale(dVolume*0.25 / volume);

			volume0 = volume;
			dVolume = (C - D).Cross(B - D).Dot(D - o);
			volume += dVolume;
			com = com.Scale(volume0 / volume) + (D + C + B + o).Scale(dVolume*0.25 / volume);

			iA++;
			iB++;
			iC--;
			iD--;
		}
		if (iB == iD)
		{
			const dVec3& A = v[iA];
			const dVec3& B = v[iB];
			const dVec3& C = v[iC];

			double volume0 = volume;
			double dVolume = (B - A).Cross(C - A).Dot(A - o);
			volume += dVolume;
			com = com.Scale(volume0 / volume) + (A + B + C + o).Scale(dVolume*0.25 / volume);
		}

		double VOLUME0 = VOLUME;
		VOLUME += volume;
		COM = COM.Scale(VOLUME0 / VOLUME) + com.Scale(volume / VOLUME);
	}
	VOLUME /= 6.0f;
	return COM;
}

// http://docsdrive.com/pdfs/sciencepublications/jmssp/2005/8-11.pdf
dMat33 TetrahedronInertia(const dVec3& v1, const dVec3& v2, const dVec3& v3, const dVec3& v4)
{
	const double& x1 = v1.x;
	const double& x2 = v2.x;
	const double& x3 = v3.x;
	const double& x4 = v4.x;

	const double& y1 = v1.y;
	const double& y2 = v2.y;
	const double& y3 = v3.y;
	const double& y4 = v4.y;

	const double& z1 = v1.z;
	const double& z2 = v2.z;
	const double& z3 = v3.z;
	const double& z4 = v4.z;

	const double det = abs((v2 - v1).Cross(v3 - v1).Dot(v4 - v1));

	const double a = (det / 60.0)*(
		y1*y1 + y1*y2 + y2*y2 + y1*y3 + y2*y3 + y3*y3 + y1*y4 + y2*y4 + y3*y4 + y4*y4 +
		z1*z1 + z1*z2 + z2*z2 + z1*z3 + z2*z3 + z3*z3 + z1*z4 + z2*z4 + z3*z4 + z4*z4
		);
	const double b = (det / 60.0)*(
		x1*x1 + x1*x2 + x2*x2 + x1*x3 + x2*x3 + x3*x3 + x1*x4 + x2*x4 + x3*x4 + x4*x4 +
		z1*z1 + z1*z2 + z2*z2 + z1*z3 + z2*z3 + z3*z3 + z1*z4 + z2*z4 + z3*z4 + z4*z4
		);
	const double c = (det / 60.0)*(
		x1*x1 + x1*x2 + x2*x2 + x1*x3 + x2*x3 + x3*x3 + x1*x4 + x2*x4 + x3*x4 + x4*x4 +
		y1*y1 + y1*y2 + y2*y2 + y1*y3 + y2*y3 + y3*y3 + y1*y4 + y2*y4 + y3*y4 + y4*y4
		);
	const double aPrime = (det / 120.0)*(
		2.0*y1*z1 + y2*z1 + y3*z1 + y4*z1 + y1*z2 +
		2.0*y2*z2 + y3*z2 + y4*z2 + y1*z3 + y2*z3 +
		2.0*y3*z3 + y4*z3 + y1*z4 + y2*z4 + y3*z4 +
		2.0*y4*z4
		);
	const double bPrime = (det / 120.0)*(
		2.0*x1*z1 + x2*z1 + x3*z1 + x4*z1 + x1*z2 +
		2.0*x2*z2 + x3*z2 + x4*z2 + x1*z3 + x2*z3 +
		2.0*x3*z3 + x4*z3 + x1*z4 + x2*z4 + x3*z4 +
		2.0*x4*z4
		);
	const double cPrime = (det / 120.0)*(
		2.0*x1*y1 + x2*y1 + x3*y1 + x4*y1 + x1*y2 +
		2.0*x2*y2 + x3*y2 + x4*y2 + x1*y3 + x2*y3 +
		2.0*x3*y3 + x4*y3 + x1*y4 + x2*y4 + x3*y4 +
		2.0*x4*y4
		);

	dMat33 I;

	I(0, 0) = a;
	I(0, 1) = -bPrime;
	I(0, 2) = -cPrime;

	I(1, 0) = -bPrime;
	I(1, 1) = b;
	I(1, 2) = -aPrime;

	I(2, 0) = -cPrime;
	I(2, 1) = -aPrime;
	I(2, 2) = c;

	return I;
}

dMat33 Mesh::InertiaLocal_Solid(double density, double& mass) const
{
	double volume;
	dVec3 o = CenterOfMassLocal_Solid(volume);
	mass = density*volume;

	dMat33 I;
	I(0, 0) = 0.0;
	I(0, 1) = 0.0;
	I(0, 2) = 0.0;
	I(1, 0) = 0.0;
	I(1, 1) = 0.0;
	I(1, 2) = 0.0;
	I(2, 0) = 0.0;
	I(2, 1) = 0.0;
	I(2, 2) = 0.0;

	for (int i = 0; i < m_nFaces; ++i)
	{
		dVec3 v[64];
		int n = 0;

		const Face& f = m_faces[i];

		int iEdge = f.iEdge;
		int iEdge0 = iEdge;
		do
		{
			const HalfEdge& edge = m_halfEdges[iEdge];
			int iVert = edge.iVert;
			v[n++] = dVec3(m_verts[iVert]);
			iEdge = edge.iNext;

		} while (iEdge != iEdge0);

		int iA = 0;
		int iB = 1;
		int iC = n - 1;
		int iD = n - 2;

		while (iB < iD)
		{
			const dVec3& A = v[iA];
			const dVec3& B = v[iB];
			const dVec3& C = v[iC];
			const dVec3& D = v[iD];

			I = I + TetrahedronInertia(A, B, C, o) + TetrahedronInertia(D, C, B, o);

			iA++;
			iB++;
			iC--;
			iD--;
		}
		if (iB == iD)
		{
			const dVec3& A = v[iA];
			const dVec3& B = v[iB];
			const dVec3& C = v[iC];

			I = I + TetrahedronInertia(A, B, C, o);
		}
	}
	return I.Scale(density);
}

void Mesh::ShiftToCenterOfMass_Solid()
{
	double v;
	const dVec3 com = CenterOfMassLocal_Solid(v);
	for (int i = 0; i < m_nVerts; ++i)
	{
		m_verts[i] = m_verts[i] - com;
	}

	const dVec3 c = CenterOfMassLocal_Solid(v);
	assert(c.Dot(c) < 0.0001);
}