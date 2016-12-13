#include "stdafx.h"
#include "Vec3.h"
#include "Vec4.h"

template <class T>
Vec3_t<T>::Vec3_t()
{
	// Leave unitialized for efficiency
}

template <class T>
Vec3_t<T>::Vec3_t(T x_, T y_, T z_)
	: x(x_), y(y_), z(z_)
{
}

template <class T>
Vec3_t<T>::Vec3_t(const Vec4_t<T>& v4)
	: x(v4.x / v4.w), y(v4.y / v4.w), z(v4.z / v4.w)
{
}

template <class T>
Vec3_t<T>::~Vec3_t()
{
}


template <class T>
void Vec3_t<T>::GetData(T* const components) const
{
	components[0] = x;
	components[1] = y;
	components[2] = z;
}

template <class T>
void Vec3_t<T>::SetData(const T* const components)
{
	x = components[0];
	y = components[1];
	z = components[2];
}

template <class T>
T Vec3_t<T>::Dot(const Vec3_t<T>& v) const
{
	return x*v.x + y*v.y + z*v.z;
}

template <class T>
Vec3_t<T> Vec3_t<T>::Cross(const Vec3_t<T>& v) const
{
	return Vec3_t<T>
	(
		y*v.z - z*v.y,
		z*v.x - x*v.z,
		x*v.y - y*v.x
	);
}

template <class T>
Vec3_t<T> Vec3_t<T>::Scale(T k) const
{
	return Vec3_t<T>(k*x, k*y, k*z);
}

template <class T>
Vec3_t<T> Vec3_t<T>::Times(const Vec3_t<T>& v) const
{
	return Vec3_t<T>(x*v.x, y*v.y, z*v.z);
}

template <class T>
Vec3_t<T> Vec3_t<T>::operator + (const Vec3_t<T>& v) const
{
	return Vec3_t<T>(x + v.x, y + v.y, z + v.z);
}

template <class T>
Vec3_t<T> Vec3_t<T>::operator - (const Vec3_t<T>& v) const
{
	return Vec3_t<T>(x - v.x, y - v.y, z - v.z);
}

template <class T>
Vec3_t<T> Vec3_t<T>::operator - () const
{
	return Vec3_t<T>(-x, -y, -z);
}

template <class T>
bool Vec3_t<T>::operator == (const Vec3_t<T>& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

template <class T>
bool Vec3_t<T>::operator != (const Vec3_t<T>& v) const
{
	return x != v.x || y != v.y || z != v.z;
}

template <class T>
T Vec3_t<T>::operator [] (int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		assert(false);
		return x;
	}
}

template <class T>
T& Vec3_t<T>::operator [] (int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	default:
		assert(false);
		return x;
	}
}

template class Vec3_t<float>;
template class Vec3_t<double>;