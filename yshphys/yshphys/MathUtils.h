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
	T Determinant(const T* const A, int n)
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

	// A = L * LT
	template <typename T>
	void CholeskyFactorization(const T* const A, int n, T* const LT)
	{
		std::memcpy(LT, A, n*n * sizeof(T));

		for (int dim = 0; dim < n; ++dim)
		{
			std::memset(&LT[n*dim], 0, dim * sizeof(T));

			LT[n*dim + dim] = sqrt(LT[n*dim + dim]);
			const T s = (T)1.0 / LT[n*dim + dim];

			for (int j = dim + 1; j < n; ++j)
			{
				LT[n*dim + j] = LT[n*dim + j] * s;
			}
			for (int i = dim + 1; i < n; ++i)
			{
				for (int j = i; j < n; ++j)
				{
					LT[n*i + j] -= LT[n*dim + i] * LT[n*dim + j];
				}
			}
		}
	}

	template <typename T>
	void GaussSeidel(const T* const A, const T* const b, const T* const xMin, const T* const xMax, int n, T* const x)
	{
		T* dx = new T[n];
		T* b_r = new T[n]; // b-r
		std::memcpy(b_r, b, n*sizeof(T));
		std::memset(x, 0, n*sizeof(T));

		T nInv = (T)1.0 / (T)n;

		T drPrev;

		for (int iter = 0; iter < 64; ++iter)
		{
			for (int i = 0; i < n; ++i)
			{
				dx[i] = b_r[i];
				for (int j = 0; j < i; ++j)
				{
					dx[i] -= A[n*i + j] * dx[j];
				}
				dx[i] /= A[n*i + i];
				dx[i] = std::max(xMin[i] - x[i], std::min(dx[i], xMax[i] - x[i]));
				x[i] += dx[i];
			}

			T dr(0.0);
			for (int i = 0; i < n; ++i)
			{
				T dri(0.0);
				for (int j = 0; j < n; ++j)
				{
					dri += A[n*i + j] * dx[j];
				}
				b_r[i] -= dri;
				dr += dri*dri*nInv;
			}
			if (iter > 0)
			{
				if (dr < (T)0.0000001 || abs(dr - drPrev) / drPrev < (T)0.01)
				{
					return;
				}
			}
			drPrev = dr;
		}
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