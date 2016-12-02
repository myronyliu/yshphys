#include "stdafx.h"
#include "HomogeneousTransformation.h"

template <class T>
HomogeneousTransformation_t<T>::HomogeneousTransformation_t()
{
}

template <class T>
HomogeneousTransformation_t<T>::~HomogeneousTransformation_t()
{
}

template <class T>
Mat44_t<T> HomogeneousTransformation_t<T>::CreateRotation(const Quat_t<T>& q)
{
	Mat44_t<T> R;
	// https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
	const T n(q.w*q.w + q.x*q.x + q.y*q.y + q.z*q.z);
	const T s(T(2.0) / n);
	const T	wx(s*q.w*q.x);
	const T wy(s*q.w*q.y);
	const T wz(s*q.w*q.z);
	const T xx(s*q.x*q.x);
	const T xy(s*q.x*q.y);
	const T xz(s*q.x*q.z);
	const T yy(s*q.y*q.y);
	const T yz(s*q.y*q.z);
	const T zz(s*q.z*q.z);

	Mat33_t<T> R;
	R.M_ij(0, 0) = (T)1.0 - (yy + zz);
	R.M_ij(0, 1) = xy - wz;
	R_M_ij(0, 2) = xz + wy;
	R.M_ij(1, 0) = xy + wz;
	R.M_ij(1, 1) = (T)1.0 - (xx + zz);
	R.M_ij(1, 2) = yz - wx;
	R.M_ij(2, 0) = xz - wy;
	R.M_ij(2, 1) = yz + wx;
	R.M_ij(2, 2) = (T)1.0 - (xx + yy);

	R.M_ij(3, 0) = (T)0.0;
	R.M_ij(3, 1) = (T)0.0;
	R.M_ij(3, 2) = (T)0.0;
	R.M_ij(3, 3) = (T)1.0;
	R.M_ij(0, 3) = (T)0.0;
	R.M_ij(1, 3) = (T)0.0;
	R.M_ij(2, 3) = (T)0.0;

	return R;
}
template <class T>
Mat44_t<T> HomogeneousTransformation_t<T>::CreateTranslation(const Vec3_t<T>& t)
{
	Mat44_t<T> tMat;

	tMat(0, 0) = (T)1.0;
	tMat(0, 1) = (T)0.0;
	tMat(0, 2) = (T)0.0;
	tMat(0, 3) = t.x;

	tMat(1, 0) = (T)0.0;
	tMat(1, 1) = (T)1.0;
	tMat(1, 2) = (T)0.0;
	tMat(1, 3) = t.y;

	tMat(2, 0) = (T)0.0;
	tMat(2, 1) = (T)0.0;
	tMat(2, 2) = (T)1.0;
	tMat(2, 3) = t.z;

	tMat(3, 0) = (T)0.0;
	tMat(3, 1) = (T)0.0;
	tMat(3, 2) = (T)0.0;
	tMat(3, 3) = (T)1.0;

	return tMat;
}
template <class T>
Mat44_t<T> HomogeneousTransformation_t<T>::CreateScale(T sx, T sy, T sz)
{
	Mat44_t<T> S;

	S(0, 0) = sx;
	S(0, 1) = (T)0.0;
	S(0, 2) = (T)0.0;
	S(0, 3) = (T)0.0;

	S(1, 0) = (T)0.0;
	S(1, 1) = sy;
	S(1, 2) = (T)0.0;
	S(1, 3) = (T)0.0;

	S(2, 0) = (T)0.0;
	S(2, 1) = (T)0.0;
	S(2, 2) = sz;
	S(2, 3) = (T)0.0;

	S(3, 0) = (T)0.0;
	S(3, 1) = (T)0.0;
	S(3, 2) = (T)0.0;
	S(3, 3) = (T)1.0;

	return S;
}