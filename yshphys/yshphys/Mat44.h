#pragma once
#include "YshMath.h"

template <class T>
class Mat44_t
{
public:
	Mat44_t();
	virtual ~Mat44_t();

	void GetData(T* const rowMajorEntries) const;

	Vec4_t<T> GetRow(int i) const;
	Vec4_t<T> GetColumn(int j) const;

	void SetRow(int i, const Vec4_t<T>& v);
	void SetColumn(int j, const Vec4_t<T>& v);

	Mat44_t<T> Transpose() const;
	T Determinant() const;
	Mat44_t<T> Inverse() const;
	Vec4_t<T> Transform(const Vec4_t<T>& v) const;
	Mat44_t<T> Scale(T scalar) const;

	T operator () (unsigned int i, unsigned int j) const;
	T& operator () (unsigned int i, unsigned int j);

	Mat44_t<T> operator + (const Mat44_t<T>& M) const;
	Mat44_t<T> operator * (const Mat44_t<T>& M) const;
	bool operator == (const Mat44_t<T>& M) const;

private:
	T M_ij[4][4];
};
