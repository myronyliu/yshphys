#include "stdafx.h"
#include "Vec2.h"
#include "Vec4.h"

template <class T>
Vec2_t<T>::Vec2_t()
{
	// Leave unitialized for efficiency
}

template <class T>
Vec2_t<T>::Vec2_t(T x_, T y_)
	: x(x_), y(y_)
{
}

template <class T> template <class S>
Vec2_t<T>::Vec2_t(const Vec2_t<S>& v)
{
	x = (T)v.x;
	y = (T)v.y;
}

template <class T>
Vec2_t<T>::~Vec2_t()
{
}


template <class T>
void Vec2_t<T>::GetData(T* const components) const
{
	components[0] = x;
	components[1] = y;
}

template <class T>
void Vec2_t<T>::SetData(const T* const components)
{
	x = components[0];
	y = components[1];
}

template <class T>
T Vec2_t<T>::Dot(const Vec2_t<T>& v) const
{
	return x*v.x + y*v.y;
}

template <class T>
Vec2_t<T> Vec2_t<T>::Scale(T k) const
{
	return Vec2_t<T>(k*x, k*y);
}

template <class T>
Vec2_t<T> Vec2_t<T>::Times(const Vec2_t<T>& v) const
{
	return Vec2_t<T>(x*v.x, y*v.y);
}

template <class T>
Vec2_t<T> Vec2_t<T>::operator + (const Vec2_t<T>& v) const
{
	return Vec2_t<T>(x + v.x, y + v.y);
}

template <class T>
Vec2_t<T> Vec2_t<T>::operator - (const Vec2_t<T>& v) const
{
	return Vec2_t<T>(x - v.x, y - v.y);
}

template <class T>
Vec2_t<T> Vec2_t<T>::operator - () const
{
	return Vec2_t<T>(-x, -y);
}

template <class T>
bool Vec2_t<T>::operator == (const Vec2_t<T>& v) const
{
	return x == v.x && y == v.y;
}

template <class T>
bool Vec2_t<T>::operator != (const Vec2_t<T>& v) const
{
	return x != v.x || y != v.y;
}

template <class T>
T Vec2_t<T>::operator [] (int i) const
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		assert(false);
		return x;
	}
}

template <class T>
T& Vec2_t<T>::operator [] (int i)
{
	switch (i)
	{
	case 0:
		return x;
	case 1:
		return y;
	default:
		assert(false);
		return x;
	}
}

template class Vec2_t<float>;
template Vec2_t<float>::Vec2_t(const Vec2_t<double>&);
template Vec2_t<float>::Vec2_t(const Vec2_t<float>&);

template class Vec2_t<double>;
template Vec2_t<double>::Vec2_t(const Vec2_t<float>&);
template Vec2_t<double>::Vec2_t(const Vec2_t<double>&);