#pragma once
#include "Vec3.h"
#include "Quat.h"

namespace MathUtils
{
	// Returns (-1, 0 , 1). See http://stackoverflow.com/a/4609795
	template <typename T>
	T sgn(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	template <typename T>
	T Determinant(const T* A, int n)
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

//	typedef void(*dydt_func)(double t, const double y[], double yDot[]);

	template <typename T>
	void IntegrateRK4
	(
		const T y0[], T y1[], int nVars,
		T t0, T t1,
		void(*f)(T t, const T y[], T yDot[])
	)
	{
		T k1[256];
		T k2[256];
		T k3[256];
		T k4[256];
		T k[256]; // temporary container for intermediate steps

		const T h(t1 - t0);
		const T h_half(h*(T)0.5);
		const T h_sixth(h / (T)(6.0));

		f(t0, y0, k1);

		for (int i = 0; i < nVars; ++i)
		{
			k[i] = y0[i] + k1[i] * h_half;
		}

		f(t0 + h_half, k, k2);

		for (int i = 0; i < nVars; ++i)
		{
			k[i] = y0[i] + k2[i] * h_half;
		}

		f(t0 + h_half, k, k3);

		for (int i = 0; i < nVars; ++i)
		{
			k[i] = y0[i] + k3[i] * h;
		}

		f(t0 + h, k, k4);

		for (int i = 0; i < nVars; ++i)
		{
			y1[i] = y0[i] + h_sixth*(k1[i] + (T)2.0 *k2[i] + T(2.0)*k3[i] + k4[i]);
		}
	}
};