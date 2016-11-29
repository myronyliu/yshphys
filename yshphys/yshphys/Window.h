#pragma once

#include <SDL.h>

class Window
{
public:
	Window();
	virtual ~Window();

	void CreateWindow();

	SDL_Window* m_window;
	SDL_Surface* m_screenSurface;
};

