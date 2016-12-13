#include "stdafx.h"
#include "Quat.h"
#include "Mat33.h"
#include "Vec3.h"

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
Quat_t<T>::Quat_t(const Mat33_t<T>& R)
{
	// https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
	const T t(R(0, 0) + R(1, 1) + R(2, 2));
	const T r(sqrt((T)1.0 + t));
	const T s((T)0.5 / r);
	w = (T)0.5*r;
	x = (R(2, 1) - R(1, 2))*s;
	y = (R(0, 2) - R(2, 0))*s;
	z = (R(1, 0) - R(0, 1))*s;
}

template <class T> template <class S>
Quat_t<T>::Quat_t(const Quat_t<S>& q)
{
	w = (T)q.w;
	x = (T)q.x;
	y = (T)q.y;
	z = (T)q.z;
}

template <class T>
Quat_t<T>::~Quat_t()
{
}

template <class T>
void Quat_t<T>::GetData(T* const components) const
{
	components[0] = x;
	components[1] = y;
	components[2] = z;
	components[3] = w;
}

template <class T>
void Quat_t<T>::SetData(const T* const components)
{
	x = components[0];
	y = components[1];
	z = components[2];
	w = components[3];
}

template <class T>
Vec3_t<T> Quat_t<T>::Transform(const Vec3_t<T>& v) const
{
	Quat_t<T> p;
	p.x = v.x;
	p.y = v.y;
	p.z = v.z;
	p.w = (T)0.0;
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

template <class T>
Quat_t<T> Quat_t<T>::Normalize() const
{
	T nInv = (T)1.0 / (w*w + x*x + y*y + z*z);
	Quat_t<T> q;
	q.w = w * nInv;
	q.x = x * nInv;
	q.y = y * nInv;
	q.z = z * nInv;
	return q;
}

template <class T>
Quat_t<T> Quat_t<T>::Identity()
{
	return Quat_t<T>((T)0.0, (T)0.0, (T)0.0, (T)1.0);
}

template class Quat_t<float>;
template Quat_t<float>::Quat_t(const Quat_t<double>&);
template Quat_t<float>::Quat_t(const Quat_t<float>&);

template class Quat_t<double>;
template Quat_t<double>::Quat_t(const Quat_t<float>&);
template Quat_t<double>::Quat_t(const Quat_t<double>&);
