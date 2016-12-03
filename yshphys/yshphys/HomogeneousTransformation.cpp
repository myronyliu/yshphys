#include "stdafx.h"
#include "HomogeneousTransformation.h"
#include "Mat44.h"
#include "Vec3.h"
#include "Quat.h"

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

	R(0, 0) = (T)1.0 - (yy + zz);
	R(0, 1) = xy - wz;
	R(0, 2) = xz + wy;
	R(1, 0) = xy + wz;
	R(1, 1) = (T)1.0 - (xx + zz);
	R(1, 2) = yz - wx;
	R(2, 0) = xz - wy;
	R(2, 1) = yz + wx;
	R(2, 2) = (T)1.0 - (xx + yy);

	R(3, 0) = (T)0.0;
	R(3, 1) = (T)0.0;
	R(3, 2) = (T)0.0;
	R(3, 3) = (T)1.0;
	R(0, 3) = (T)0.0;
	R(1, 3) = (T)0.0;
	R(2, 3) = (T)0.0;

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
template <class T>
Mat44_t<T> HomogeneousTransformation_t<T>::CreateProjection(T fov, T aspect, T near, T far)
{
	// http://ogldev.atspace.co.uk/www/tutorial12/tutorial12.html
	Mat44_t<T> P;

	const T dProjPlane(1.0f / tan((T)0.5*fov));
	const T near_far(near - far);

	P(0, 0) = dProjPlane / aspect;
	P(0, 1) = (T)0.0;
	P(0, 2) = (T)0.0;
	P(0, 3) = (T)0.0;

	P(1, 0) = (T)0.0;
	P(1, 1) = dProjPlane;
	P(1, 2) = (T)0.0;
	P(1, 3) = (T)0.0;

	P(2, 0) = (T)0.0;
	P(2, 1) = (T)0.0;
	P(2, 2) = (near + far) / near_far;
	P(2, 3) = (T)2.0*near*far / near_far;

	P(3, 0) = (T)0.0;
	P(3, 1) = (T)0.0;
	P(3, 2) = (T)-1.0;
	P(3, 3) = (T)0.0;

	return P;
}

template class HomogeneousTransformation_t<float>;
template class HomogeneousTransformation_t<double> ;