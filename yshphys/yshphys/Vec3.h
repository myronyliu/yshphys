#pragma once
#include "YshMath.h"

template<class T> class Vec3_t
{
public:
	///////////////
	// FUNCTIONS //
	///////////////

	Vec3_t();
	Vec3_t(T x_, T y_, T z_);
	Vec3_t(const Vec4_t<T>& v4);
	virtual ~Vec3_t();

	T Dot(const Vec3_t<T>& v) const;
	Vec3_t<T> Cross(const Vec3_t<T>& v) const;
	Vec3_t<T> Scale(T x) const;
	Vec3_t<T> Times(const Vec3_t<T>& v) const; // Componentwise multiplication

	Vec3_t<T> operator + (const Vec3_t<T>& v) const;
	Vec3_t<T> operator - (const Vec3_t<T>& v) const;
	Vec3_t<T> operator - () const;
	bool operator == (const Vec3_t<T>& v) const;
	bool operator != (const Vec3_t<T>& v) const;

	T operator [] (int i) const;
	T& operator [] (int i);

	///////////////
	// VARIABLES //
	///////////////

	T x, y, z;
};
