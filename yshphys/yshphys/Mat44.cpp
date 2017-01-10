#include "stdafx.h"
#include "Mat44.h"
#include "Vec4.h"
#include "MathUtils.h"

template <class T>
Mat44_t<T>::Mat44_t()
{
}

template <class T>
Mat44_t<T>::~Mat44_t()
{
}

template <class T>
Vec4_t<T> Mat44_t<T>::GetRow(int i) const
{
	return Vec4_t<T>(M_ij[i][0], M_ij[i][1], M_ij[i][2], M_ij[i][3]);
}

template <class T>
Vec4_t<T> Mat44_t<T>::GetColumn(int j) const
{
	return Vec4_t<T>(M_ij[0][j], M_ij[1][j], M_ij[2][j], M_ij[3][j]);
}

template <class T>
void Mat44_t<T>::SetRow(int i, const Vec4_t<T>& v)
{
	M_ij[i][0] = v.x;
	M_ij[i][1] = v.y;
	M_ij[i][2] = v.z;
	M_ij[i][3] = v.w;
}

template <class T>
void Mat44_t<T>::SetColumn(int j, const Vec4_t<T>& v)
{
	M_ij[0][j] = v.x;
	M_ij[1][j] = v.y;
	M_ij[2][j] = v.z;
	M_ij[3][j] = v.w;
}

template <class T>
Mat44_t<T> Mat44_t<T>::Transpose() const
{
	Mat44_t<T> transpose;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			transpose.M_ij[i][j] = M_ij[j][i];
		}
	}
	return transpose;
}

template <class T>
T Mat44_t<T>::Determinant() const
{
	return MathUtils::Determinant(&M_ij[0][0], 4);
}

template <class T>
Mat44_t<T> Mat44_t<T>::Inverse() const
{
	Mat44_t<T> inv;
	const T detInv = (T)1.0 / Determinant();
	for (int i = 0; i < 4; ++i)
	{
		const int i0((i + 1) % 4);
		const int i1((i + 2) % 4);
		const int i2((i + 3) % 4);
		for (int j = 0; j < 4; ++j)
		{
			const int j0((j + 1) % 4);
			const int j1((j + 2) % 4);
			const int j2((j + 3) % 4);
			inv.M_ij[i][j] = detInv * (
				M_ij[i0][j0] * (M_ij[i1][j1] * M_ij[i2][j2] - M_ij[i2][j1] * M_ij[i1][j2]) +
				M_ij[i1][j0] * (M_ij[i2][j1] * M_ij[i0][j2] - M_ij[i0][j1] * M_ij[i2][j2]) +
				M_ij[i2][j0] * (M_ij[i0][j1] * M_ij[i1][j2] - M_ij[i1][j1] * M_ij[i0][j2]));
		}
	}
	return inv;
}

template <class T>
Vec4_t<T> Mat44_t<T>::Transform(const Vec4_t<T>& v) const
{
	Vec3_t<T> product;
	for (int j = 0; j < 4; ++j)
	{
		T contraction(0.0);
		for (int k = 0; k < 4; ++k)
		{
			contraction += M_ij[j][k] * v[k];
		}
		product[j] = contraction;
	}
	return product;
}

template <class T>
Mat44_t<T> Mat44_t<T>::Scale(T scalar) const
{
	Mat44_t<T> product;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			product.M_ij[i][j] = scalar * M_ij[i][j];
		}
	}
	return product;
}

template <class T>
T Mat44_t<T>::operator () (unsigned int i, unsigned int j) const
{
	return M_ij[i][j];
}

template <class T>
T& Mat44_t<T>::operator () (unsigned int i, unsigned int j)
{
	return M_ij[i][j];
}

template <class T>
Mat44_t<T> Mat44_t<T>::operator * (const Mat44_t& B) const
{
	Mat44_t<T> product;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			T contraction(0.0);
			for (int k = 0; k < 4; ++k)
			{
				contraction += M_ij[i][k] * B.M_ij[k][j];
			}
			product.M_ij[i][j] = contraction;
		}
	}
	return product;
}

template <class T>
bool Mat44_t<T>::operator == (const Mat44_t<T>& M) const
{
	return
		M_ij[0][0] == M.M_ij[0][0] &&
		M_ij[0][1] == M.M_ij[0][1] &&
		M_ij[0][2] == M.M_ij[0][2] &&
		M_ij[0][3] == M.M_ij[0][3] &&

		M_ij[1][0] == M.M_ij[1][0] &&
		M_ij[1][1] == M.M_ij[1][1] &&
		M_ij[1][2] == M.M_ij[1][2] &&
		M_ij[1][3] == M.M_ij[1][3] &&

		M_ij[2][0] == M.M_ij[2][0] &&
		M_ij[2][1] == M.M_ij[2][1] &&
		M_ij[2][2] == M.M_ij[2][2] &&
		M_ij[2][3] == M.M_ij[2][3] &&

		M_ij[3][0] == M.M_ij[3][0] &&
		M_ij[3][1] == M.M_ij[3][1] &&
		M_ij[3][2] == M.M_ij[3][2] &&
		M_ij[3][3] == M.M_ij[3][3];
}

template class Mat44_t<float>;
template class Mat44_t<double>;
