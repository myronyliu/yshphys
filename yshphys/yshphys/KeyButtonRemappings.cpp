#include "stdafx.h"
#include <SDL.h>

bool IsKeyboardInput(unsigned short ysh_input_code)
{
	return ysh_input_code < (unsigned short)std::numeric_limits<Uint8>::max();
}
