#pragma once
#include "YshMath.h"

template <class T>
class HomogeneousTransformation_t
{
public:
	static Mat44_t<T> CreateRotation(const Quat_t<T>& q);
	static Mat44_t<T> CreateTranslation(const Vec3_t<T>& t);
	static Mat44_t<T> CreateScale(T sx, T sy, T sz);

	static Mat44_t<T> CreateProjection(T fov, T aspect, T near, T far);

private:
	HomogeneousTransformation_t();
	virtual ~HomogeneousTransformation_t();
};

