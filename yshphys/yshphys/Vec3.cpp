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

void Vec3::operator *= (double k)
{
	x *= k;
	y *= k;
	z *= k;
}

void Vec3::operator /= (double k)
{
	double kInv = 1.0 / k;
	x *= kInv;
	y *= kInv;
	z *= kInv;
}

void Vec3::operator += (const Vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}
void Vec3::operator -= (const Vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

Vec3 operator * (double k, const Vec3& v)
{
	return Vec3(k*v.x, k*v.y, k*v.z);
}
Vec3 operator * (const Vec3& v, double k)
{
	return Vec3(k*v.x, k*v.y, k*v.z);
}
Vec3 operator / (const Vec3& v, double k)
{
	double kInv = 1.0 / k;
	return Vec3(kInv*v.x, kInv*v.y, kInv*v.z);
}
Vec3 operator + (const Vec3& u, const Vec3& v)
{
	return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}
Vec3 operator - (const Vec3& u, const Vec3& v)
{
	return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}
