#pragma once
#include "Vec3.h"
#include "Quat.h"

template <class T> class MathUtils_t
{
public:
	static T Determinant(const T* matrix, int size);
private:
	MathUtils_t() {}
	~MathUtils_t() {}
};