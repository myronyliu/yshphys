#include "stdafx.h"
#include "Polygon.h"

Polygon::Polygon() : m_nVertices(0)
{

}

void Polygon::AddVertex(const dVec2& vertex)
{
	m_vertices[m_nVertices++] = vertex;
}