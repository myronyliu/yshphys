#include "stdafx.h"
#include "Mat33.h"
#include "Vec3.h"
#include "Quat.h"

template <class T>
Mat33_t<T>::Mat33_t()
{
}

template <class T>
Mat33_t<T>::Mat33_t(const Quat_t<T>& q)
{
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

	M_ij[0][0] = (T)1.0 - (yy + zz);
	M_ij[0][1] = xy - wz;
	M_ij[0][2] = xz + wy;
	M_ij[1][0] = xy + wz;
	M_ij[1][1] = (T)1.0 - (xx + zz);
	M_ij[1][2] = yz - wx;
	M_ij[2][0] = xz - wy;
	M_ij[2][1] = yz + wx;
	M_ij[2][2] = (T)1.0 - (xx + yy);
}

template <class T>
Mat33_t<T>::~Mat33_t()
{
}

template <class T>
Vec3_t<T> Mat33_t<T>::GetRow(int i) const
{
	return Vec3_t<T>(M_ij[i][0], M_ij[i][1], M_ij[i][2]);
}

template <class T>
Vec3_t<T> Mat33_t<T>::GetColumn(int j) const
{
	return Vec3_t<T>(M_ij[0][j], M_ij[1][j], M_ij[2][j]);
}

template <class T>
void Mat33_t<T>::GetData(T* const rowMajorElementArray) const
{
	std::memcpy(rowMajorElementArray, M_ij, 9 * sizeof(T));
}

template <class T>
void Mat33_t<T>::SetRow(int i, const Vec3_t<T>& v)
{
	M_ij[i][0] = v.x;
	M_ij[i][1] = v.y;
	M_ij[i][2] = v.z;
}

template <class T>
void Mat33_t<T>::SetColumn(int j, const Vec3_t<T>& v)
{
	M_ij[0][j] = v.x;
	M_ij[1][j] = v.y;
	M_ij[2][j] = v.z;
}

template <class T>
void Mat33_t<T>::SetData(const T* const rowMajorElementArray)
{
	std::memcpy(M_ij, rowMajorElementArray, 9 * sizeof(T));
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
Mat33_t<T> Mat33_t<T>::Abs() const
{
	Mat33_t<T> A;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			A.M_ij[i][j] = abs(M_ij[i][j]);
		}
	}
	return A;
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
		for (int j = 0; j < 3; ++j)
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