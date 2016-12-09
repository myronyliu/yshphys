#include "stdafx.h"
#include "Vec4.h"
#include "Vec3.h"

template <class T>
Vec4_t<T>::Vec4_t()
{
	// Leave unitialized for efficiency
}

template <class T>
Vec4_t<T>::Vec4_t(T x_, T y_, T z_, T w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

template <class T>
Vec4_t<T>::Vec4_t(const Vec3_t<T>& v3)
	: x(v3.x), y(v3.y), z(v3.z), w(T(1.0))
{
}

template <class T>
Vec4_t<T>::~Vec4_t()
{
}

template <class T>
T Vec4_t<T>::Dot(const Vec4_t<T>& v) const
{
	return x*v.x + y*v.y + z*v.z + w*v.w;
}

template <class T>
Vec4_t<T> Vec4_t<T>::Scale(T k) const
{
	return Vec4_t<T>(k*x, k*y, k*z, k*w);
}

template <class T>
Vec4_t<T> Vec4_t<T>::Times(const Vec4_t<T>& v) const
{
	return Vec4_t<T>(x*v.x, y*v.y, z*v.z, w*v.w);
}

template <class T>
Vec4_t<T> Vec4_t<T>::operator + (const Vec4_t<T>& v) const
{
	return Vec4_t<T>(x + v.x, y + v.y, z + v.z, w + v.w);
}

template <class T>
Vec4_t<T> Vec4_t<T>::operator - (const Vec4_t<T>& v) const
{
	return Vec4_t<T>(x - v.x, y - v.y, z - v.z, w - v.w);
}

template <class T>
Vec4_t<T> Vec4_t<T>::operator - () const
{
	return Vec4_t<T>(-x, -y, -z, -w);
}

template <class T>
bool Vec4_t<T>::operator == (const Vec4_t<T>& v) const
{
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

template <class T>
T Vec4_t<T>::operator [] (int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		assert(false);
		return x;
	}
}

template <class T>
T& Vec4_t<T>::operator [] (int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		assert(false);
		return x;
	}
}

template class Vec4_t<float>;
template class Vec4_t<double>;
