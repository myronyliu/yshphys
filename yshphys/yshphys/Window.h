#pragma once

#include <SDL.h>
#include <glew.h>

class Window
{
public:
	Window();
	virtual ~Window();

	void CreateWindow(int x, int y, int width, int height);
	void InitGL();
	void UpdateGLRender();

	void GetUpperLeftCorner(int& x, int& y) const;
	void GetDimensions(int& width, int& height) const;

	SDL_Window* m_window;
	SDL_Surface* m_screenSurface;
	SDL_GLContext m_glContext;
	GLuint m_defaultShader;

protected:
	int m_x;
	int m_y;
	int m_w;
	int m_h;
};