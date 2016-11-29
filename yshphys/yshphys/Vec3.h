#pragma once
template<class T> class Vec3_t
{
public:
	///////////////
	// FUNCTIONS //
	///////////////

	Vec3_t();
	Vec3_t(T x_, T y_, T z_);
	virtual ~Vec3_t();

	T Dot(const Vec3_t<T>& v) const;
	Vec3_t<T> Cross(const Vec3_t<T>& v) const;
	Vec3_t<T> Scale(T x) const;
	Vec3_t<T> Times(const Vec3_t<T>& v) const; // Componentwise multiplication

	Vec3_t<T> operator + (const Vec3_t<T>& v) const;
	Vec3_t<T> operator - (const Vec3_t<T>& v) const;
	Vec3_t<T> operator - () const;

	T operator [] (int i) const;
	T& operator [] (int i);

	///////////////
	// VARIABLES //
	///////////////

	T x, y, z;
};

typedef Vec3_t<double> dVec3;
typedef Vec3_t<float> fVec3;