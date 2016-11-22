#pragma once
class Vec3
{
public:
	///////////////
	// FUNCTIONS //
	///////////////

	Vec3();
	Vec3(double x_, double y_, double z_);
	virtual ~Vec3();

	double Dot(const Vec3& v) const;
	Vec3 Cross(const Vec3& v) const;

	void operator *= (double k);
	void operator /= (double k);
	void operator += (const Vec3& v);
	void operator -= (const Vec3& v);

	///////////////
	// VARIABLES //
	///////////////

	double x, y, z;
};


Vec3 operator * (double k, const Vec3& v);
Vec3 operator * (const Vec3& v, double k);
Vec3 operator / (const Vec3& v, double k);
Vec3 operator + (const Vec3& u, const Vec3& v);
Vec3 operator - (const Vec3& u, const Vec3& v);
