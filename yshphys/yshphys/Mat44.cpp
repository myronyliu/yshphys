#include "stdafx.h"
#include "Mat44.h"
#include "Mat33.h"
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
	return MathUtils_t<T>::Determinant(&M_ij[0][0], 4);
}

template <class T>
Mat44_t<T> Mat44_t<T>::Inverse() const
{
	Mat44_t<T> inv;
	const T detInv = 1.0f / Determinant();
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
		for (int j = 0; j < 4; ++i)
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

template class Mat44_t<float>;
template class Mat44_t<double>;
