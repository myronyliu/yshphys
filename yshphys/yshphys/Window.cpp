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
	m_window = SDL_CreateWindow("yshphys", 88, 88, 1200, 900, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	m_screenSurface = SDL_GetWindowSurface(m_window);
	SDL_SetRelativeMouseMode(SDL_TRUE);
	InitGL();
}

void Window::InitGL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	m_glContext = SDL_GL_CreateContext(m_window);
	if (m_glContext == NULL)
	{
		std::string asdf(SDL_GetError());
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
	}
	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7f, 0.0f, 0.3f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void Window::UpdateGLRender()
{
	SDL_GL_SwapWindow(m_window);
}