#include "stdafx.h"
#include "Vec3.h"


Vec3::Vec3()
{
	// Leave unitialized for efficiency
}

Vec3::Vec3(double x_, double y_, double z_)
	: x(x_), y(y_), z(z_)
{
}

Vec3::~Vec3()
{
}

double Vec3::Dot(const Vec3& v) const
{
	return x*v.x + y*v.y + z*v.z;
}

Vec3 Vec3::Cross(const Vec3& v) const
{
	return Vec3
	(
		y*v.z - z*v.y,
		z*v.x - x*v.z,
		x*v.y - y*v.x
	);
}

Vec3 Vec3::Scale(double k) const
{
	return Vec3(k*x, k*y, k*z);
}

Vec3 Vec3::Times(const Vec3& v) const
{
	return Vec3(x*v.x, y*v.y, z*v.z);
}

Vec3 Vec3::operator + (const Vec3& v) const
{
	return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::operator - (const Vec3& v) const
{
	return Vec3(x - v.x, y - v.y, z - v.z);
}
