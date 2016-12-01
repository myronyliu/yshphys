#include "stdafx.h"
#include "Mat33.h"

template <class T>
Mat33_t<T>::Mat33_t()
{
}

template <class T>
Mat33_t<T>::~Mat33_t()
{
}

template <class T>
Mat33_t<T> Mat33_t<T>::Transpose() const
{
	Mat33_t<T> transpose;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			transpose.M_ij[i][j] = M_ij[j][i];
		}
	}
	return transpose;
}

template <class T>
T Mat33_t<T>::Determinant() const
{
	return
		M_ij[0][0] * (M_ij[1][1] * M_ij[2][2] - M_ij[2][1] * M_ij[1][2]) +
		M_ij[1][0] * (M_ij[2][1] * M_ij[0][2] - M_ij[0][1] * M_ij[2][2]) +
		M_ij[2][0] * (M_ij[0][1] * M_ij[1][2] - M_ij[1][1] * M_ij[0][2]);
}

template <class T>
Mat33_t<T> Mat33_t<T>::Inverse() const
{
	Mat33_t<T> inv;
	const T detInv = 1.0f / Determinant();
	for (int i = 0; i < 3; ++i)
	{
		const int i0((i + 1) % 3);
		const int i1((i + 2) % 3);
		for (int j = 0; j < 3; ++j)
		{
			const int j0((j + 1) % 3);
			const int j1((j + 2) % 3);
			inv.M_ij[i][j] = detInv * (M_ij[i0][j0] * M_ij[i1][j1] - M_ij[i0][j1] * M_ij[i1][j0]);
		}
	}
	return inv;
}

template <class T>
Vec3_t<T> Mat33_t<T>::Transform(const Vec3_t<T>& v) const
{
	Vec3_t<T> product;
	for (int j = 0; j < 3; ++j)
	{
		T contraction(0.0);
		for (int k = 0; k < 3; ++k)
		{
			contraction += M_ij[j][k] * v[k];
		}
		product[j] = contraction;
	}
	return product;
}

template <class T>
Mat33_t<T> Mat33_t<T>::Scale(T scalar) const
{
	Mat33_t<T> product;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			product.M_ij[i][j] = scalar * M_ij[i][j];
		}
	}
	return product;
}

template <class T>
T Mat33_t<T>::operator () (unsigned int i, unsigned int j) const
{
	return M_ij[i][j];
}

template <class T>
T& Mat33_t<T>::operator () (unsigned int i, unsigned int j)
{
	return M_ij[i][j];
}

template <class T>
Mat33_t<T> Mat33_t<T>::operator * (const Mat33_t& B) const
{
	Mat33_t<T> product;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++i)
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

template class Mat33_t<float>;
template class Mat33_t<double>;