#pragma once

#include "Vec4.h"

template <class T>
class Mat44_t
{
public:
	Mat44_t();
	virtual ~Mat44_t();

	Mat44_t<T> Transpose() const;
	T Determinant() const;
	Mat44_t<T> Inverse() const;
	Vec4_t<T> Transform(const Vec4_t<T>& v) const;
	Mat44_t<T> Scale(T scalar) const;

	T operator () (unsigned int i, unsigned int j) const;
	T& operator () (unsigned int i, unsigned int j);

	Mat44_t<T> operator * (const Mat44_t& M) const;

private:
	T M_ij[4][4];
};

typedef Mat44_t<float> fMat44;
typedef Mat44_t<double> dMat44;
