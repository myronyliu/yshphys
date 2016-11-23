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
	Vec3 Scale(double x) const;
	Vec3 Times(const Vec3& v) const; // Componentwise multiplication

	Vec3 operator + (const Vec3& v) const;
	Vec3 operator - (const Vec3& v) const;
	Vec3 operator - () const;

	///////////////
	// VARIABLES //
	///////////////

	double x, y, z;
};

