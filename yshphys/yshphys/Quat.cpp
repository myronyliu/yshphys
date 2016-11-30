#include "stdafx.h"
#include "Quat.h"

template <class T>
Quat_t<T>::Quat_t()
	: x(T(0.0)), y(T(0.0)), z(T(0.0)), w(T(1.0))
{
}

template <class T>
Quat_t<T>::Quat_t(T x_, T y_, T z_, T w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

template <class T>
Quat_t<T>::Quat_t(const Vec3_t<T>& axis, T angle)
	: w(cos(T(0.5) * angle))
{
	T k = sin(T(0.5) *angle);
	x = k * axis.x;
	y = k * axis.y;
	z = k * axis.z;
}

template <class T>
Quat_t<T>::~Quat_t()
{
}

template <class T>
Vec3_t<T> Quat_t<T>::Transform(const Vec3_t<T>& v) const
{
	Quat_t<T> p(v.x, v.y, v.z, 0.0);
	const Quat_t<T>& q = *this;
	Quat_t<T> pRotated = q*p*(-q);
	return Vec3_t<T>(pRotated.x, pRotated.y, pRotated.z);
}

template <class T>
Quat_t<T> Quat_t<T>::operator * (const Quat_t& q) const
{
	return Quat_t<T>
	(
		w*q.x + x*q.w + y*q.z - z*q.y,
		w*q.y - x*q.z + y*q.w + z*q.x,
		w*q.z + x*q.y - y*q.x + z*q.w,
		w*q.w - x*q.x - y*q.y - z*q.z
	);
}

template <class T>
Quat_t<T> Quat_t<T>::operator - () const
{
	return Quat_t<T>(-x, -y, -z, w);
}

template class Quat_t<float>;
template class Quat_t<double>;