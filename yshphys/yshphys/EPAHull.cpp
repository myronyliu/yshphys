#include "stdafx.h"
#include "Simplex3D.h"


EPAHull::HalfEdge::~HalfEdge()
{
}

void EPAHull::HalfEdge::SetVertex(Vertex* vertex)
{
	if (vert != nullptr)
	{
		vert->nRefs--;
	}
	if (vertex != nullptr)
	{
		vertex->nRefs++;
	}
	if (vert->nRefs == 0)
	{
		delete vert;
	}
	vert = vertex;
}

void EPAHull::InvalidateFace(EPAHull::Face& face)
{
	face.distance = -88888888.0;
	EPAHull::HalfEdge* edges[3];
	edges[0] = face.edge;
	edges[1] = edges[0]->next;
	edges[2] = edges[1]->next;

	for (EPAHull::HalfEdge* const edge : edges)
	{
		edge->face = nullptr;
		edge->next = nullptr;
		edge->prev = nullptr;
		edge->twin->twin = nullptr;
		edge->twin = nullptr;
		edge->SetVertex(nullptr);
	}
}