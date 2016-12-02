#pragma once

#include "Mat44.h"
#include "Vec3.h"
#include "Quat.h"

template <class T>
class HomogeneousTransformation_t
{
public:
	static Mat44_t<T> CreateRotation(const Quat_t<T>& q);
	static Mat44_t<T> CreateTranslation(const Vec3_t<T>& t);
	static Mat44_t<T> CreateScale(T sx, T sy, T sz);

private:
	HomogeneousTransformation_t();
	virtual ~HomogeneousTransformation_t();
};

