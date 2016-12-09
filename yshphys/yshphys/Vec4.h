#pragma once
#include "YshMath.h"

template<class T> class Vec4_t
{
public:
	///////////////
	// FUNCTIONS //
	///////////////

	Vec4_t();
	Vec4_t(T x_, T y_, T z_, T w_);
	Vec4_t(const Vec3_t<T>& v3);
	virtual ~Vec4_t();

	T Dot(const Vec4_t<T>& v) const;
	Vec4_t<T> Scale(T x) const;
	Vec4_t<T> Times(const Vec4_t<T>& v) const; // Componentwise multiplication

	Vec4_t<T> operator + (const Vec4_t<T>& v) const;
	Vec4_t<T> operator - (const Vec4_t<T>& v) const;
	Vec4_t<T> operator - () const;
	bool operator == (const Vec4_t<T>& v) const;

	T operator [] (int i) const;
	T& operator [] (int i);

	///////////////
	// VARIABLES //
	///////////////

	T x, y, z, w;
};
