#pragma once
#include "YshMath.h"

template<class T> class Vec2_t
{
public:
	///////////////
	// FUNCTIONS //
	///////////////

	Vec2_t();
	Vec2_t(T x_, T y_);
	template <class S> Vec2_t(const Vec2_t<S>& v);
	virtual ~Vec2_t();

	void GetData(T* const components) const;
	void SetData(const T* const components);

	T Dot(const Vec2_t<T>& v) const;
	Vec2_t<T> Scale(T x) const;
	Vec2_t<T> Times(const Vec2_t<T>& v) const; // Componentwise multiplication

	Vec2_t<T> operator + (const Vec2_t<T>& v) const;
	Vec2_t<T> operator - (const Vec2_t<T>& v) const;
	Vec2_t<T> operator - () const;
	bool operator == (const Vec2_t<T>& v) const;
	bool operator != (const Vec2_t<T>& v) const;

	T operator [] (int i) const;
	T& operator [] (int i);

	///////////////
	// VARIABLES //
	///////////////

	T x, y;
};
