#include "stdafx.h"
#include "Quat.h"


Quat::Quat()
	: x(0.0), y(0.0), z(0.0), w(1.0)
{
}

Quat::Quat(double x_, double y_, double z_, double w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

Quat::Quat(const Vec3& axis, double angle)
	: w(cos(0.5f * angle))
{
	double k = sin(0.5f *angle);
	x = k * axis.x;
	y = k * axis.y;
	z = k * axis.z;
}

Quat::~Quat()
{
}

Vec3 Quat::Transform(const Vec3& v) const
{
	Quat p(v.x, v.y, v.z, 0.0);
	const Quat& q = *this;
	Quat pRotated = q*p*(-q);
	return Vec3(pRotated.x, pRotated.y, pRotated.z);
}

Quat Quat::operator * (const Quat& q) const
{
	return Quat
	(
		w*q.x + x*q.w + y*q.z - z*q.y,
		w*q.y - x*q.z + y*q.w + z*q.x,
		w*q.z + x*q.y - y*q.x + z*q.w,
		w*q.w - x*q.x - y*q.y - z*q.z
	);
}

Quat Quat::operator - () const
{
	return Quat(-x, -y, -z, w);
}
