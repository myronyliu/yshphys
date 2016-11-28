// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <cstdlib>
#include <math.h>
#include <cassert>

// Returns (-1, 0 , 1). See http://stackoverflow.com/a/4609795
template <typename T> int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

// TODO: reference additional headers your program requires here
