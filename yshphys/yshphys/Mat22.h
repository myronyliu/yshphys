#pragma once
#include "YshMath.h"

template <class T>
class Mat22_t
{
public:
	Mat22_t();
	virtual ~Mat22_t();

	Vec2_t<T> GetRow(int i) const;
	Vec2_t<T> GetColumn(int j) const;

	void SetRow(int i, const Vec2_t<T>& v);
	void SetColumn(int j, const Vec2_t<T>& v);

	void SetData(const T* const rowMajorElementArray);
	void GetData(T* const rowMajorElementArray) const;

	Mat22_t<T> Transpose() const;
	T Determinant() const;
	Mat22_t<T> Inverse() const;
	Vec2_t<T> Transform(const Vec2_t<T>& v) const;
	Mat22_t<T> Scale(T scalar) const;

	static Mat22_t<T> Identity();

	Mat22_t<T> Abs() const;
	
	T operator () (unsigned int i, unsigned int j) const;
	T& operator () (unsigned int i, unsigned int j);

	Mat22_t<T> operator * (const Mat22_t& M) const;
	bool operator == (const Mat22_t& M) const;

private:
	T M_ij[2][2];
};
