#pragma once

#include <SDL.h>
#include <glew.h>

class Window
{
public:
	Window();
	virtual ~Window();

	void CreateWindow();
	void InitGL();
	void UpdateGLRender();

	SDL_Window* m_window;
	SDL_Surface* m_screenSurface;
	SDL_GLContext m_glContext;
	GLuint m_defaultShader;

};