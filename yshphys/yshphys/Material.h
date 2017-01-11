#pragma once

struct FrictionCoefficients
{
	double uStatic;
	double uKinetic;
};

class Material
{
public:
	enum Type
	{
		WOOD = 0,
		METAL,
		PLASTIC,

		N_MATERIAL_TYPES
	};

	static double Restitution(Material::Type type0, Material::Type type1);
	static FrictionCoefficients Friction(Material::Type type0, Material::Type type1);

	static void InitializeTables();

private:

	static double g_RestitutionTable[Material::Type::N_MATERIAL_TYPES][Material::Type::N_MATERIAL_TYPES];
	static FrictionCoefficients g_FrictionTable[Material::Type::N_MATERIAL_TYPES][Material::Type::N_MATERIAL_TYPES];
};