#pragma once
#include "YshMath.h"

template <class T>
class Mat33_t
{
public:
	Mat33_t();
	Mat33_t(const Quat_t<T>& q);
	virtual ~Mat33_t();

	Vec3_t<T> GetRow(int i) const;
	Vec3_t<T> GetColumn(int j) const;

	void SetRow(int i, const Vec3_t<T>& v);
	void SetColumn(int j, const Vec3_t<T>& v);

	Mat33_t<T> Transpose() const;
	T Determinant() const;
	Mat33_t<T> Inverse() const;
	Vec3_t<T> Transform(const Vec3_t<T>& v) const;
	Mat33_t<T> Scale(T scalar) const;
	
	T operator () (unsigned int i, unsigned int j) const;
	T& operator () (unsigned int i, unsigned int j);

	Mat33_t<T> operator * (const Mat33_t& M) const;

private:
	T M_ij[3][3];
};
