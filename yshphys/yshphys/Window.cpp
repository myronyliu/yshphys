#include "stdafx.h"
#include "Window.h"

Window::Window() :
	m_window(nullptr),
	m_screenSurface(nullptr)
{
}


Window::~Window()
{
}

void Window::CreateWindow()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("yshphys", 88, 88, 888, 888, SDL_WINDOW_SHOWN);
	m_screenSurface = SDL_GetWindowSurface(m_window);
}
