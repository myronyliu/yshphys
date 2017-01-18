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

	////////////////////////////////////////////////////////////////////////////////////////////
	// Credits to Julio Jerez (Newton Dynamics) for showing me his implementation of GaussSeidel
	// This is basically a carbon copy of his, differing only in stylistic minutiae.
	////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	void GaussSeidel(const T* const A, const T* const b, const T* const xMin, const T* const xMax, int n, T* const x)
	{
		T* const invDiag = new T[n];
		for (int i = 0; i < n; ++i)
		{
			x[i] = std::max(xMin[i], std::min(x[i], xMax[i]));
			invDiag[i] = T(1.0) / A[n*i + i];
		}

		const int maxIter = n*n*n*n + 256;
		const T rrThresh(0.000001*0.000001);
		T rr(88888888.0);

		for (int iter = 0; (iter < maxIter) && (rr > rrThresh); ++iter)
		{
			rr = (T)0.0;
			for (int i = 0; i < n; ++i)
			{
				// Compute r_i : the change in A_ii*x_i (NOTE: not quite the same as the residual)
				// If r_i becomes small for all i, then we have convergence

				T r_i(0.0);
				for (int j = 0; j < n; ++j)
				{
					r_i -= A[n*i + j] * x[j];
				}
				r_i += b[i];

				// Update and clamp x_i to the limits. If the x_i hit the limits, we don't consider the ith
				// component as part of the termination criteria, since we aren't free to vary it.

				x[i] = (r_i + A[n*i + i] * x[i]) * invDiag[i];

				if (x[i] < xMin[i])
				{
					x[i] = xMin[i];
				}
				else if (x[i] > xMax[i])
				{
					x[i] = xMax[i];
				}
				else
				{
					rr += r_i*r_i;
				}
			}
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