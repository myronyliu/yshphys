#include "stdafx.h"
#include "MathUtils.h"

template <class T>
T MathUtils_t<T>::Determinant(const T* A, int n)
{
	assert(n > 1);
	if (n == 2)
	{
		return A[0] * A[3] - A[1] * A[2];
	}

	T det(0.0);
	
	for (int i = 0; i < n; ++i) // Go down the column
	{
		const int parity(1 - 2 * (i % 2));
		T* a = new T[(n - 1)*(n - 1)];

		for (int k = 0; k < i; ++k)
		{
			std::memcpy(&a[k*(n - 1)], &A[k*n + 1], (n - 1) * sizeof(T));
		}
		for (int k = i + 1; k < n; ++k)
		{
			std::memcpy(&a[(k - 1)*(n - 1)], &A[k*n + 1], (n - 1) * sizeof(T));
		}

		det += (T)parity * A[i*n] * Determinant(a, n - 1);
		delete[] a;
	}
	return det;
}

template class MathUtils_t<float>;
template class MathUtils_t<double>;