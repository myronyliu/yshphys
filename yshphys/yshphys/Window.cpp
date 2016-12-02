#include "stdafx.h"
#include "Window.h"
#include <glew.h>

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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	m_window = SDL_CreateWindow("yshphys", 88, 88, 888, 888, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	m_screenSurface = SDL_GetWindowSurface(m_window);
	m_glContext = SDL_GL_CreateContext(m_window);
	if (m_glContext == NULL)
	{
		std::string asdf(SDL_GetError());
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
	}
	glewExperimental = GL_TRUE;
	glewInit();
}


