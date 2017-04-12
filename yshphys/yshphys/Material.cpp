#include "stdafx.h"
#include "Material.h"

double Material::g_RestitutionTable[Material::Type::N_MATERIAL_TYPES][Material::Type::N_MATERIAL_TYPES];
FrictionCoefficients Material::g_FrictionTable[Material::Type::N_MATERIAL_TYPES][Material::Type::N_MATERIAL_TYPES];

double Material::Restitution(Material::Type type0, Material::Type type1)
{
	return g_RestitutionTable[type0][type1];
}
FrictionCoefficients Material::Friction(Material::Type type0, Material::Type type1)
{
	return g_FrictionTable[type0][type1];
}

void Material::InitializeTables()
{
	g_RestitutionTable[Material::Type::WOOD][Material::Type::WOOD] = 0.5;

	g_FrictionTable[Material::Type::WOOD][Material::Type::WOOD].uKinetic = 0.25;
	g_FrictionTable[Material::Type::WOOD][Material::Type::WOOD].uStatic = 0.5;
}