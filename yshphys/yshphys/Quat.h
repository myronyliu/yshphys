#pragma once
#include "Vec3.h"

class Quat
{
public:
	// For constructors, we adopt the convention of AXIS first and ANGLE second
	Quat();
	Quat(double x_, double y_, double z_, double w_);
	Quat(const Vec3& axis, double angle);
	virtual ~Quat();

	Vec3 Transform(const Vec3& v) const;

	Quat operator * (const Quat& q) const;
	Quat operator - () const; // conjugate http://mathworld.wolfram.com/QuaternionConjugate.html
	
protected:

	///////////////
	// VARIABLES //
	///////////////

	double x, y, z;
	double w;
};

