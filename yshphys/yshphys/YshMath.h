#pragma once

// Template classes are causing a headache so we will predeclare all of them here

template <typename> class Vec3_t;
typedef Vec3_t<float> fVec3;
typedef Vec3_t<double> dVec3;

template <typename> class Vec4_t;
typedef Vec4_t<float> fVec4;
typedef Vec4_t<double> dVec4;

template <typename> class Quat_t;
typedef Quat_t<float> fQuat;
typedef Quat_t<double> dQuat;

template <typename> class Mat33_t;
typedef Mat33_t<float> fMat33;
typedef Mat33_t<double> dMat33;

template <typename> class Mat44_t;
typedef Mat44_t<float> fMat44;
typedef Mat44_t<double> dMat44;

template <typename> class HomogeneousTransformation_t;
typedef HomogeneousTransformation_t<float> fHomogeneousTransformation;
typedef HomogeneousTransformation_t<double> dHomogeneousTransformation;

#define fPI 3.141592653589793238462643383279502884197169399375105820974f
#define dPI 3.141592653589793238462643383279502884197169399375105820974

// Now we include all of the headers with the member functions. This way, we force the .cpp code to be compiled for
// relevant template data-types (specifically <float> and <double>). Anything that wishes to use the math library
// merely needs to include this file YshPhys.h

#include "Vec3.h"
#include "Vec4.h"
#include "Quat.h"
#include "Mat33.h"
#include "Mat44.h"
#include "MathUtils.h"
#include "HomogeneousTransformation.h"