#include "stdafx.h"
#include "Mat22.h"
#include "Vec2.h"

template <class T>
Mat22_t<T>::Mat22_t()
{
}

template <class T>
Mat22_t<T>::~Mat22_t()
{
}

template <class T>
Vec2_t<T> Mat22_t<T>::GetRow(int i) const
{
	return Vec2_t<T>(M_ij[i][0], M_ij[i][1]);
}

template <class T>
Vec2_t<T> Mat22_t<T>::GetColumn(int j) const
{
	return Vec2_t<T>(M_ij[0][j], M_ij[1][j]);
}

template <class T>
void Mat22_t<T>::GetData(T* const rowMajorElementArray) const
{
	std::memcpy(rowMajorElementArray, M_ij, 4 * sizeof(T));
}

template <class T>
void Mat22_t<T>::SetRow(int i, const Vec2_t<T>& v)
{
	M_ij[i][0] = v.x;
	M_ij[i][1] = v.y;
}

template <class T>
void Mat22_t<T>::SetColumn(int j, const Vec2_t<T>& v)
{
	M_ij[0][j] = v.x;
	M_ij[1][j] = v.y;
}

template <class T>
void Mat22_t<T>::SetData(const T* const rowMajorElementArray)
{
	std::memcpy(M_ij, rowMajorElementArray, 4 * sizeof(T));
}

template <class T>
Mat22_t<T> Mat22_t<T>::Transpose() const
{
	Mat22_t<T> transpose;
	transpose.M_ij[0][0] = M_ij[0][0];
	transpose.M_ij[0][1] = M_ij[1][0];
	transpose.M_ij[1][0] = M_ij[0][1];
	transpose.M_ij[1][1] = M_ij[1][1];
	return transpose;
}

template <class T>
T Mat22_t<T>::Determinant() const
{
	return M_ij[0][0] * M_ij[1][1] - M_ij[0][1] - M_ij[1][0];
}

template <class T>
Mat22_t<T> Mat22_t<T>::Inverse() const
{
	Mat22_t<T> inv;
	const T detInv = (T)1.0 / (M_ij[0][0] * M_ij[1][1] - M_ij[0][1] - M_ij[1][0]);
	inv.M_ij[0][0] = M_ij[1][1] * detInv;
	inv.M_ij[0][1] = -M_ij[0][1] * detInv;
	inv.M_ij[1][0] = -M_ij[1][0] * detInv;
	inv.M_ij[1][1] = M_ij[0][0] * detInv;
	return inv;
}

template <class T>
Vec2_t<T> Mat22_t<T>::Transform(const Vec2_t<T>& v) const
{
	Vec2_t<T> product;
	product[0] = M_ij[0][0] * v[0] + M_ij[0][1] * v[1];
	product[1] = M_ij[1][0] * v[0] + M_ij[1][1] * v[1];
	return product;
}

template <class T>
Mat22_t<T> Mat22_t<T>::Scale(T scalar) const
{
	Mat22_t<T> product;
	product.M_ij[0][0] = scalar * M_ij[0][0];
	product.M_ij[0][1] = scalar * M_ij[0][1];
	product.M_ij[1][0] = scalar * M_ij[1][0];
	product.M_ij[1][1] = scalar * M_ij[1][1];
	return product;
}

template <class T>
Mat22_t<T> Mat22_t<T>::Identity()
{
	Mat22_t<T> mat;

	mat.M_ij[0][0] = (T)1.0;
	mat.M_ij[0][1] = (T)0.0;

	mat.M_ij[1][0] = (T)0.0;
	mat.M_ij[1][1] = (T)1.0;

	return mat;
}

template <class T>
Mat22_t<T> Mat22_t<T>::Abs() const
{
	Mat22_t<T> A;
	A.M_ij[0][0] = abs(M_ij[0][0]);
	A.M_ij[0][1] = abs(M_ij[0][1]);
	A.M_ij[1][0] = abs(M_ij[1][0]);
	A.M_ij[1][1] = abs(M_ij[1][1]);
	return A;
}

template <class T>
T Mat22_t<T>::operator () (unsigned int i, unsigned int j) const
{
	return M_ij[i][j];
}

template <class T>
T& Mat22_t<T>::operator () (unsigned int i, unsigned int j)
{
	return M_ij[i][j];
}

template <class T>
Mat22_t<T> Mat22_t<T>::operator * (const Mat22_t& B) const
{
	Mat22_t<T> product;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			T contraction(0.0);
			for (int k = 0; k < 3; ++k)
			{
				contraction += M_ij[i][k] * B.M_ij[k][j];
			}
			product.M_ij[i][j] = contraction;
		}
	}
	return product;
}

template class Mat22_t<float>;
template class Mat22_t<double>;